#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <unordered_map>
#include <thread>
#include <chrono>

using namespace std;

/*************************************************
 * ENUMS
 *************************************************/
enum class NotificationChannel {
    EMAIL,
    SMS,
    PUSH
};

enum class NotificationStatus {
    PENDING,
    SENT,
    FAILED
};

/*************************************************
 * NOTIFICATION ENTITY
 *************************************************/
class Notification {
public:
    string id;
    string userId;
    NotificationChannel channel;
    string templateId;
    map<string, string> payload;
    NotificationStatus status;

    Notification(
        string id,
        string userId,
        NotificationChannel channel,
        string templateId,
        map<string, string> payload
    ) : id(id),
        userId(userId),
        channel(channel),
        templateId(templateId),
        payload(payload),
        status(NotificationStatus::PENDING) {}
};

/*************************************************
 * USER PREFERENCES
 *************************************************/
class UserPreference {
public:
    bool emailEnabled = true;
    bool smsEnabled = true;
    bool pushEnabled = true;
};

class UserPreferenceService {
private:
    unordered_map<string, UserPreference> preferences;

public:
    void setPreference(const string& userId,
                       const UserPreference& pref) {
        preferences[userId] = pref;
    }

    bool isChannelAllowed(
        const string& userId,
        NotificationChannel channel
    ) {
        if (preferences.find(userId) == preferences.end())
            return true;

        auto pref = preferences[userId];

        switch (channel) {
            case NotificationChannel::EMAIL:
                return pref.emailEnabled;
            case NotificationChannel::SMS:
                return pref.smsEnabled;
            case NotificationChannel::PUSH:
                return pref.pushEnabled;
        }
        return false;
    }
};

/*************************************************
 * TEMPLATE ENGINE
 *************************************************/
class TemplateRepository {
private:
    unordered_map<string, string> templates;

public:
    TemplateRepository() {
        templates["ORDER_CONFIRM"] =
            "Hello {{name}}, your order {{orderId}} "
            "has been confirmed.";

        templates["PAYMENT_FAILED"] =
            "Hi {{name}}, payment failed for "
            "order {{orderId}}.";
    }

    string getTemplate(const string& templateId) {
        return templates[templateId];
    }
};

class TemplateEngine {
public:
    string render(
        string templ,
        const map<string, string>& payload
    ) {
        for (auto& kv : payload) {
            string key = "{{" + kv.first + "}}";

            size_t pos = templ.find(key);
            while (pos != string::npos) {
                templ.replace(
                    pos,
                    key.length(),
                    kv.second
                );
                pos = templ.find(key);
            }
        }
        return templ;
    }
};

/*************************************************
 * STRATEGY PATTERN
 *************************************************/
class INotificationSender {
public:
    virtual bool send(
        const string& userId,
        const string& message
    ) = 0;

    virtual ~INotificationSender() = default;
};

class EmailSender : public INotificationSender {
public:
    bool send(
        const string& userId,
        const string& message
    ) override {
        cout << "[EMAIL] To: "
             << userId
             << "\nMessage: "
             << message
             << "\n\n";
        return true;
    }
};

class SmsSender : public INotificationSender {
public:
    bool send(
        const string& userId,
        const string& message
    ) override {
        cout << "[SMS] To: "
             << userId
             << "\nMessage: "
             << message
             << "\n\n";

        return true;
    }
};

class PushSender : public INotificationSender {
public:
    bool send(
        const string& userId,
        const string& message
    ) override {
        cout << "[PUSH] To: "
             << userId
             << "\nMessage: "
             << message
             << "\n\n";

        return true;
    }
};

/*************************************************
 * DECORATOR PATTERN (RETRY)
 *************************************************/
class RetrySender : public INotificationSender {
private:
    unique_ptr<INotificationSender> sender;
    int maxRetries;

public:
    RetrySender(
        unique_ptr<INotificationSender> sender,
        int retries = 3
    ) : sender(move(sender)),
        maxRetries(retries) {}

    bool send(
        const string& userId,
        const string& message
    ) override {

        for (int i = 1; i <= maxRetries; i++) {
            cout << "Attempt "
                 << i
                 << "...\n";

            if (sender->send(userId, message)) {
                return true;
            }

            this_thread::sleep_for(
                chrono::milliseconds(500)
            );
        }

        return false;
    }
};

/*************************************************
 * FACTORY PATTERN
 *************************************************/
class NotificationSenderFactory {
public:
    static unique_ptr<INotificationSender>
    createSender(NotificationChannel channel) {

        unique_ptr<INotificationSender> sender;

        switch (channel) {
            case NotificationChannel::EMAIL:
                sender =
                    make_unique<EmailSender>();
                break;

            case NotificationChannel::SMS:
                sender =
                    make_unique<SmsSender>();
                break;

            case NotificationChannel::PUSH:
                sender =
                    make_unique<PushSender>();
                break;
        }

        return make_unique<RetrySender>(
            move(sender)
        );
    }
};

/*************************************************
 * NOTIFICATION SERVICE
 *************************************************/
class NotificationService {
private:
    TemplateRepository templateRepo;
    TemplateEngine templateEngine;
    UserPreferenceService& prefService;

public:
    NotificationService(
        UserPreferenceService& prefService
    ) : prefService(prefService) {}

    void send(Notification& notification) {

        // 1. Check user preference
        if (!prefService.isChannelAllowed(
                notification.userId,
                notification.channel)) {

            cout << "Notification blocked "
                 << "by user preference.\n";
            notification.status =
                NotificationStatus::FAILED;
            return;
        }

        // 2. Render template
        string templ =
            templateRepo.getTemplate(
                notification.templateId
            );

        string message =
            templateEngine.render(
                templ,
                notification.payload
            );

        // 3. Create sender
        auto sender =
            NotificationSenderFactory
                ::createSender(
                    notification.channel
                );

        // 4. Send
        bool success =
            sender->send(
                notification.userId,
                message
            );

        notification.status =
            success
                ? NotificationStatus::SENT
                : NotificationStatus::FAILED;
    }
};

/*************************************************
 * MAIN
 *************************************************/
int main() {

    UserPreferenceService prefService;

    // Configure preferences
    UserPreference pref;
    pref.emailEnabled = true;
    pref.smsEnabled = false;
    pref.pushEnabled = true;

    prefService.setPreference(
        "user123",
        pref
    );

    NotificationService service(
        prefService
    );

    // Example notification
    Notification notif(
        "notif001",
        "user123",
        NotificationChannel::EMAIL,
        "ORDER_CONFIRM",
        {
            {"name", "Sayandeep"},
            {"orderId", "ORD-98765"}
        }
    );

    service.send(notif);

    cout << "Final Status: "
         << (notif.status ==
             NotificationStatus::SENT
                 ? "SENT"
                 : "FAILED")
         << endl;

    return 0;
}