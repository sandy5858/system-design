/*

System Design: Rate Limiter Service

Objects:
1. User
   - string userId
   - string userName
   - UserTier tier

2. Enum UserTier
   - FREE
   - PREMIUM

3. Enum ResponseCode
   - SUCCESS
   - TOO_MANY_REQUESTS
   - INTERNAL_ERROR

3a. RequestHistory (for tracking)
   - long timestamp
   - int count

4. IService (Interface)
   - Response* processRequest(Request* req);

5. RateLimiterService: public IService (Proxy Pattern)
   - IService* innerService
   - map<UserTier, RateLimiterStrategy*> strategies
   - Response* processRequest(Request* req);  // Delegates to strategy, then innerService

6. RateLimiterStrategy (Abstract)
   - virtual bool isAllowed(string userId, long currentTime) = 0;
   - virtual map<string, RequestHistory> userHistory  // Per-user state

7. FixedWindowRateLimiterStrategy: public RateLimiterStrategy
   - int windowSize (seconds)
   - int maxRequests
   - map<string, pair<long, int>> userWindow  // userId -> (windowStart, count)
   - bool isAllowed(string userId, long currentTime);

8. TokenBucketRateLimiterStrategy: public RateLimiterStrategy
   - int bucketSize
   - int refillRate (tokens/second)
   - map<string, pair<int, long>> userBucket  // userId -> (tokens, lastRefillTime)
   - bool isAllowed(string userId, long currentTime);

9. FloatingWindowRateLimiterStrategy: public RateLimiterStrategy
   - int windowSize (seconds)
   - int maxRequests
   - map<string, queue<long>> userRequests  // userId -> timestamps
   - bool isAllowed(string userId, long currentTime);

10. RateLimiterServiceFactory
   - static RateLimiterStrategy* createStrategy(string type, UserTier tier)

11. Service: public IService
    - Response* processRequest(Request* req)  // Actual business logic

12. Request
    - string url
    - User* user

13. Response
    - ResponseCode code
    - string body
    - long retryAfterSeconds  // For TOO_MANY_REQUESTS

*/

#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <string>

using namespace std;

enum class UserTier {
    FREE,
    PREMIUM
};

enum class ResponseCode {
    SUCCESS,
    TOO_MANY_REQUESTS,
    INTERNAL_ERROR
};

struct Response {
    ResponseCode code;
    string body;
    long retryAfterSeconds{0};
};

struct User {
    string userId;
    string userName;
    UserTier tier;
};

struct Request {
    string url;
    User* user;
};

struct RequestHistory {
    long timestamp{0};
    int count{0};
};

class IService {
public:
    virtual ~IService() = default;
    virtual Response processRequest(Request* req) = 0;
};

class RateLimiterStrategy {
public:
    virtual ~RateLimiterStrategy() = default;
    virtual bool isAllowed(const string& userId, long currentTime) = 0;
    virtual long getRetryAfterSeconds(const string& userId, long currentTime) {
        return 1;
    }
};

static long currentTimeSeconds() {
    return chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();
}

class FixedWindowRateLimiterStrategy : public RateLimiterStrategy {
public:
    FixedWindowRateLimiterStrategy(int windowSizeSeconds, int maxRequests)
        : windowSize(windowSizeSeconds), maxRequests(maxRequests) {}

    bool isAllowed(const string& userId, long currentTime) override {
        auto& entry = userWindow[userId];
        if (currentTime - entry.first >= windowSize) {
            entry.first = currentTime;
            entry.second = 0;
        }

        if (entry.second < maxRequests) {
            entry.second++;
            return true;
        }

        return false;
    }

    long getRetryAfterSeconds(const string& userId, long currentTime) override {
        auto it = userWindow.find(userId);
        if (it == userWindow.end()) {
            return 1;
        }

        long elapsed = currentTime - it->second.first;
        long remaining = windowSize - elapsed;
        return remaining > 0 ? remaining : 1;
    }

private:
    int windowSize;
    int maxRequests;
    map<string, pair<long, int>> userWindow;
};

class TokenBucketRateLimiterStrategy : public RateLimiterStrategy {
public:
    TokenBucketRateLimiterStrategy(int bucketSize, int refillRatePerSecond)
        : bucketSize(bucketSize), refillRate(refillRatePerSecond) {}

    bool isAllowed(const string& userId, long currentTime) override {
        auto& entry = userBucket[userId];
        int tokens = entry.first;
        long lastRefill = entry.second;

        if (lastRefill == 0) {
            tokens = bucketSize;
            lastRefill = currentTime;
        }

        int refillTokens = static_cast<int>((currentTime - lastRefill) * refillRate);
        tokens = min(bucketSize, tokens + refillTokens);
        entry.second = currentTime;
        entry.first = tokens;

        if (tokens > 0) {
            entry.first = tokens - 1;
            return true;
        }

        return false;
    }

    long getRetryAfterSeconds(const string& userId, long currentTime) override {
        auto it = userBucket.find(userId);
        if (it == userBucket.end()) {
            return 1;
        }

        int tokens = it->second.first;
        if (tokens > 0) {
            return 0;
        }

        return 1;
    }

private:
    int bucketSize;
    int refillRate;
    map<string, pair<int, long>> userBucket;
};

class FloatingWindowRateLimiterStrategy : public RateLimiterStrategy {
public:
    FloatingWindowRateLimiterStrategy(int windowSizeSeconds, int maxRequests)
        : windowSize(windowSizeSeconds), maxRequests(maxRequests) {}

    bool isAllowed(const string& userId, long currentTime) override {
        auto& timestamps = userRequests[userId];
        while (!timestamps.empty() && (currentTime - timestamps.front() >= windowSize)) {
            timestamps.pop();
        }

        if (static_cast<int>(timestamps.size()) < maxRequests) {
            timestamps.push(currentTime);
            return true;
        }

        return false;
    }

    long getRetryAfterSeconds(const string& userId, long currentTime) override {
        auto it = userRequests.find(userId);
        if (it == userRequests.end() || it->second.empty()) {
            return 1;
        }

        long oldest = it->second.front();
        long remaining = windowSize - (currentTime - oldest);
        return remaining > 0 ? remaining : 1;
    }

private:
    int windowSize;
    int maxRequests;
    map<string, queue<long>> userRequests;
};

class RateLimiterService : public IService {
public:
    RateLimiterService(shared_ptr<IService> innerService,
                       map<UserTier, shared_ptr<RateLimiterStrategy>> strategies)
        : innerService(move(innerService)), strategies(move(strategies)) {}

    Response processRequest(Request* req) override {
        if (!req || !req->user) {
            return {ResponseCode::INTERNAL_ERROR, "Invalid request", 0};
        }

        auto it = strategies.find(req->user->tier);
        if (it == strategies.end() || !it->second) {
            return {ResponseCode::INTERNAL_ERROR, "No rate limiter configured for user tier", 0};
        }

        long now = currentTimeSeconds();
        if (it->second->isAllowed(req->user->userId, now)) {
            return innerService->processRequest(req);
        }

        return {ResponseCode::TOO_MANY_REQUESTS,
                "Rate limit exceeded",
                it->second->getRetryAfterSeconds(req->user->userId, now)};
    }

private:
    shared_ptr<IService> innerService;
    map<UserTier, shared_ptr<RateLimiterStrategy>> strategies;
};

class Service : public IService {
public:
    Response processRequest(Request* req) override {
        if (!req || !req->user) {
            return {ResponseCode::INTERNAL_ERROR, "Invalid request", 0};
        }
        return {ResponseCode::SUCCESS, "Processed " + req->url, 0};
    }
};

class RateLimiterServiceFactory {
public:
    static shared_ptr<RateLimiterStrategy> createStrategy(const string& type, UserTier tier) {
        if (type == "fixed") {
            if (tier == UserTier::PREMIUM) {
                return make_shared<FixedWindowRateLimiterStrategy>(10, 20);
            }
            return make_shared<FixedWindowRateLimiterStrategy>(10, 5);
        }

        if (type == "token") {
            if (tier == UserTier::PREMIUM) {
                return make_shared<TokenBucketRateLimiterStrategy>(10, 5);
            }
            return make_shared<TokenBucketRateLimiterStrategy>(5, 1);
        }

        if (type == "floating") {
            if (tier == UserTier::PREMIUM) {
                return make_shared<FloatingWindowRateLimiterStrategy>(10, 20);
            }
            return make_shared<FloatingWindowRateLimiterStrategy>(10, 5);
        }

        return nullptr;
    }
};

static string toString(ResponseCode code) {
    switch (code) {
        case ResponseCode::SUCCESS:
            return "SUCCESS";
        case ResponseCode::TOO_MANY_REQUESTS:
            return "TOO_MANY_REQUESTS";
        case ResponseCode::INTERNAL_ERROR:
            return "INTERNAL_ERROR";
    }
    return "UNKNOWN";
}

int main() {
    auto businessService = make_shared<Service>();
    map<UserTier, shared_ptr<RateLimiterStrategy>> strategies;
    strategies[UserTier::FREE] = RateLimiterServiceFactory::createStrategy("fixed", UserTier::FREE);
    strategies[UserTier::PREMIUM] = RateLimiterServiceFactory::createStrategy("token", UserTier::PREMIUM);

    RateLimiterService limiter(businessService, move(strategies));

    User freeUser{"user-free", "Free User", UserTier::FREE};
    User premiumUser{"user-premium", "Premium User", UserTier::PREMIUM};

    Request freeRequest{"/api/free", &freeUser};
    Request premiumRequest{"/api/premium", &premiumUser};

    cout << "Free user requests:\n";
    for (int i = 0; i < 8; ++i) {
        Response response = limiter.processRequest(&freeRequest);
        cout << i + 1 << ". " << toString(response.code) << " - " << response.body;
        if (response.retryAfterSeconds > 0) {
            cout << " (retry after " << response.retryAfterSeconds << "s)";
        }
        cout << '\n';
    }

    cout << "\nPremium user requests:\n";
    for (int i = 0; i < 8; ++i) {
        Response response = limiter.processRequest(&premiumRequest);
        cout << i + 1 << ". " << toString(response.code) << " - " << response.body;
        if (response.retryAfterSeconds > 0) {
            cout << " (retry after " << response.retryAfterSeconds << "s)";
        }
        cout << '\n';
    }

    return 0;
}
