/*

Objects
1. CabBookingSystem
- vector<Driver*> drivers;
- vector<Rider*> riders;
- void requestTrip(Rider* rider, Location* source, Location* destination)
- TripSchedulingController* controller
- NotificationService* notificationService

2. TripSchedulingController
- PricingStrategy* pricingStrategy
- RiderSchedulingStrategy* riderSchedulingStrategy
- Trip* scheduleTrip(Rider* rider, Location* source, Location* destination)
- void handleTripAcceptance(Trip* trip, Driver* driver)
- void handleTripCancellation(Trip* trip, string reason)

3. Trip
- int tripId
- TripState state   //enum(Active, Completed, Cancelled)
- Driver* driver
- Rider* rider
- Location source
- Location destination
- int price
- Payment* payment
- long requestedTime
- long acceptedTime
- long completedTime
- void setPrice(int price)
- int getPrice()
- void cancelTrip(string reason)
- void setTripState(TripState state)
- void completeTrip()

4. Rider
- int riderId
- int riderRating
- string name
- vector<Trip*> trips
- Location currentLocation
- PaymentMethod* paymentMethod
- Location getCurrentLocation()
- void addTrip(Trip* trip)

5. Driver
- int driverId
- int driverRating
- string name
- License* drivingLicense
- Location currentLocation
- DriverStatus status   //enum(Available, OnTrip, Offline)
- Vehicle* vehicle
- vector<Trip*> trips
- void setStatus(DriverStatus status)
- DriverStatus getStatus()
- bool isAvailable()
- void addTrip(Trip* trip)

6. Vehicle
- string vehicleId
- string vehicleType   //Sedan, SUV, Premium
- int capacity
- string licensePlate

7. Location
- double latitude
- double longitude
- double calculateDistance(Location* other)
- bool equals(Location* other)

8. Payment (abstract)
- int paymentId
- PaymentStatus status   //enum(Pending, Completed, Failed, Refunded)
- int amount
- long transactionTime
- virtual bool processPayment() = 0
- virtual bool refund() = 0
-> CreditCardPayment
-> UPIPayment
-> WalletPayment

9. PaymentMethod
- string methodId
- string methodType

10. License
- string licenseNumber
- string licenseType
- long expiryDate
- bool isValid()

11. NotificationService (abstract)
- virtual void notifyRider(Rider* rider, string message) = 0
- virtual void notifyDriver(Driver* driver, string message) = 0
-> EmailNotification
-> SMSNotification
-> PushNotification

12. PricingStrategy (abstract)
- virtual int calculatePrice(Location* source, Location* destination) = 0
- virtual int calculatePrice(Location* source, Location* destination, int surgeFactor) = 0
-> FlatPricingStrategy
-> DistanceBasedPricingStrategy
-> TimeSurgeBasedPricingStrategy

13. RiderSchedulingStrategy (abstract)
- virtual Driver* selectDriver(vector<Driver*> availableDrivers, Location* pickupLocation) = 0
-> NearestDriverStrategy
-> HighestRatedDriverStrategy
-> RatingAndDistanceStrategy

*/

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <memory>
#include <algorithm>

using namespace std;

// ==================== ENUMS ====================
enum class TripState { ACTIVE, COMPLETED, CANCELLED };
enum class DriverStatus { AVAILABLE, ON_TRIP, OFFLINE };
enum class PaymentStatus { PENDING, COMPLETED, FAILED, REFUNDED };

// ==================== FORWARD DECLARATIONS ====================
class Driver;
class Rider;
class Trip;
class Payment;
class Location;
class License;
class Vehicle;
class PaymentMethod;
class NotificationService;
class PricingStrategy;
class RiderSchedulingStrategy;

// ==================== LOCATION ====================
class Location {
private:
    double latitude;
    double longitude;

public:
    Location(double lat, double lon) : latitude(lat), longitude(lon) {}

    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }

    double calculateDistance(Location* other) const {
        if (!other) return 0.0;
        const double R = 6371; // Earth radius in km
        double lat1 = latitude * M_PI / 180.0;
        double lat2 = other->latitude * M_PI / 180.0;
        double dLat = (other->latitude - latitude) * M_PI / 180.0;
        double dLon = (other->longitude - longitude) * M_PI / 180.0;

        double a = sin(dLat / 2) * sin(dLat / 2) +
                   cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
        double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        return R * c;
    }

    bool equals(Location* other) const {
        if (!other) return false;
        return latitude == other->latitude && longitude == other->longitude;
    }
};

// ==================== LICENSE ====================
class License {
private:
    string licenseNumber;
    string licenseType;
    long expiryDate;

public:
    License(string number, string type, long expiry)
        : licenseNumber(number), licenseType(type), expiryDate(expiry) {}

    bool isValid() const {
        return time(nullptr) < expiryDate;
    }

    string getLicenseNumber() const { return licenseNumber; }
    string getLicenseType() const { return licenseType; }
};

// ==================== VEHICLE ====================
class Vehicle {
private:
    string vehicleId;
    string vehicleType;
    int capacity;
    string licensePlate;

public:
    Vehicle(string id, string type, int cap, string plate)
        : vehicleId(id), vehicleType(type), capacity(cap), licensePlate(plate) {}

    string getVehicleId() const { return vehicleId; }
    string getVehicleType() const { return vehicleType; }
    int getCapacity() const { return capacity; }
    string getLicensePlate() const { return licensePlate; }
};

// ==================== PAYMENT METHOD ====================
class PaymentMethod {
private:
    string methodId;
    string methodType;

public:
    PaymentMethod(string id, string type) : methodId(id), methodType(type) {}

    string getMethodId() const { return methodId; }
    string getMethodType() const { return methodType; }
};

// ==================== PAYMENT (ABSTRACT) ====================
class Payment {
protected:
    int paymentId;
    PaymentStatus status;
    int amount;
    long transactionTime;

public:
    Payment(int id, int amt)
        : paymentId(id), status(PaymentStatus::PENDING), amount(amt), transactionTime(0) {}

    virtual bool processPayment() = 0;
    virtual bool refund() = 0;

    int getPaymentId() const { return paymentId; }
    PaymentStatus getStatus() const { return status; }
    int getAmount() const { return amount; }
    void setStatus(PaymentStatus s) { status = s; }
};

// ==================== PAYMENT IMPLEMENTATIONS ====================
class CreditCardPayment : public Payment {
private:
    string cardNumber;

public:
    CreditCardPayment(int id, int amt, string card)
        : Payment(id, amt), cardNumber(card) {}

    bool processPayment() override {
        cout << "Processing credit card payment of $" << amount << endl;
        status = PaymentStatus::COMPLETED;
        transactionTime = time(nullptr);
        return true;
    }

    bool refund() override {
        cout << "Refunding credit card: $" << amount << endl;
        status = PaymentStatus::REFUNDED;
        return true;
    }
};

class UPIPayment : public Payment {
private:
    string upiId;

public:
    UPIPayment(int id, int amt, string upi)
        : Payment(id, amt), upiId(upi) {}

    bool processPayment() override {
        cout << "Processing UPI payment of $" << amount << endl;
        status = PaymentStatus::COMPLETED;
        transactionTime = time(nullptr);
        return true;
    }

    bool refund() override {
        cout << "Refunding UPI: $" << amount << endl;
        status = PaymentStatus::REFUNDED;
        return true;
    }
};

class WalletPayment : public Payment {
private:
    int walletBalance;

public:
    WalletPayment(int id, int amt, int balance)
        : Payment(id, amt), walletBalance(balance) {}

    bool processPayment() override {
        if (walletBalance >= amount) {
            cout << "Processing wallet payment of $" << amount << endl;
            status = PaymentStatus::COMPLETED;
            transactionTime = time(nullptr);
            walletBalance -= amount;
            return true;
        }
        status = PaymentStatus::FAILED;
        return false;
    }

    bool refund() override {
        cout << "Refunding wallet: $" << amount << endl;
        walletBalance += amount;
        status = PaymentStatus::REFUNDED;
        return true;
    }
};

// ==================== NOTIFICATION SERVICE (ABSTRACT) ====================
class NotificationService {
public:
    virtual void notifyRider(Rider* rider, string message) = 0;
    virtual void notifyDriver(Driver* driver, string message) = 0;
};

// ==================== NOTIFICATION IMPLEMENTATIONS ====================
class EmailNotification : public NotificationService {
public:
    void notifyRider(Rider* rider, string message) override {
        cout << "[EMAIL] Notifying rider: " << message << endl;
    }

    void notifyDriver(Driver* driver, string message) override {
        cout << "[EMAIL] Notifying driver: " << message << endl;
    }
};

class SMSNotification : public NotificationService {
public:
    void notifyRider(Rider* rider, string message) override {
        cout << "[SMS] Notifying rider: " << message << endl;
    }

    void notifyDriver(Driver* driver, string message) override {
        cout << "[SMS] Notifying driver: " << message << endl;
    }
};

class PushNotification : public NotificationService {
public:
    void notifyRider(Rider* rider, string message) override {
        cout << "[PUSH] Notifying rider: " << message << endl;
    }

    void notifyDriver(Driver* driver, string message) override {
        cout << "[PUSH] Notifying driver: " << message << endl;
    }
};

// ==================== PRICING STRATEGY (ABSTRACT) ====================
class PricingStrategy {
public:
    virtual int calculatePrice(Location* source, Location* destination) = 0;
    virtual int calculatePrice(Location* source, Location* destination, int surgeFactor) = 0;
};

// ==================== PRICING STRATEGY IMPLEMENTATIONS ====================
class FlatPricingStrategy : public PricingStrategy {
private:
    int basePrice;

public:
    FlatPricingStrategy(int price) : basePrice(price) {}

    int calculatePrice(Location* source, Location* destination) override {
        return basePrice;
    }

    int calculatePrice(Location* source, Location* destination, int surgeFactor) override {
        return (basePrice * surgeFactor) / 100;
    }
};

class DistanceBasedPricingStrategy : public PricingStrategy {
private:
    int pricePerKm;

public:
    DistanceBasedPricingStrategy(int price) : pricePerKm(price) {}

    int calculatePrice(Location* source, Location* destination) override {
        double distance = source->calculateDistance(destination);
        return static_cast<int>(distance * pricePerKm);
    }

    int calculatePrice(Location* source, Location* destination, int surgeFactor) override {
        double distance = source->calculateDistance(destination);
        int basePrice = static_cast<int>(distance * pricePerKm);
        return (basePrice * surgeFactor) / 100;
    }
};

class TimeSurgeBasedPricingStrategy : public PricingStrategy {
private:
    int basePrice;

public:
    TimeSurgeBasedPricingStrategy(int price) : basePrice(price) {}

    int calculatePrice(Location* source, Location* destination) override {
        return basePrice;
    }

    int calculatePrice(Location* source, Location* destination, int surgeFactor) override {
        return (basePrice * surgeFactor) / 100;
    }
};

// ==================== RIDER SCHEDULING STRATEGY (ABSTRACT) ====================
class RiderSchedulingStrategy {
public:
    virtual Driver* selectDriver(vector<Driver*> availableDrivers, Location* pickupLocation) = 0;
};

// ==================== RIDER SCHEDULING STRATEGY IMPLEMENTATIONS ====================
class NearestDriverStrategy : public RiderSchedulingStrategy {
public:
    Driver* selectDriver(vector<Driver*> availableDrivers, Location* pickupLocation) override;
};

class HighestRatedDriverStrategy : public RiderSchedulingStrategy {
public:
    Driver* selectDriver(vector<Driver*> availableDrivers, Location* pickupLocation) override;
};

class RatingAndDistanceStrategy : public RiderSchedulingStrategy {
public:
    Driver* selectDriver(vector<Driver*> availableDrivers, Location* pickupLocation) override;
};

// ==================== RIDER ====================
class Rider {
private:
    int riderId;
    int riderRating;
    string name;
    vector<Trip*> trips;
    Location* currentLocation;
    PaymentMethod* paymentMethod;

public:
    Rider(int id, string n, int rating, Location* loc, PaymentMethod* method)
        : riderId(id), name(n), riderRating(rating), currentLocation(loc), paymentMethod(method) {}

    int getRiderId() const { return riderId; }
    string getName() const { return name; }
    int getRiderRating() const { return riderRating; }
    Location* getCurrentLocation() const { return currentLocation; }
    PaymentMethod* getPaymentMethod() const { return paymentMethod; }
    vector<Trip*> getTrips() const { return trips; }

    void addTrip(Trip* trip) {
        trips.push_back(trip);
    }

    void setCurrentLocation(Location* loc) {
        currentLocation = loc;
    }
};

// ==================== DRIVER ====================
class Driver {
private:
    int driverId;
    int driverRating;
    string name;
    License* drivingLicense;
    Location* currentLocation;
    DriverStatus status;
    Vehicle* vehicle;
    vector<Trip*> trips;

public:
    Driver(int id, string n, int rating, License* license, Location* loc, Vehicle* v)
        : driverId(id), name(n), driverRating(rating), drivingLicense(license),
          currentLocation(loc), status(DriverStatus::OFFLINE), vehicle(v) {}

    int getDriverId() const { return driverId; }
    string getName() const { return name; }
    int getDriverRating() const { return driverRating; }
    License* getDrivingLicense() const { return drivingLicense; }
    Location* getCurrentLocation() const { return currentLocation; }
    DriverStatus getStatus() const { return status; }
    Vehicle* getVehicle() const { return vehicle; }
    vector<Trip*> getTrips() const { return trips; }

    void setStatus(DriverStatus s) { status = s; }
    bool isAvailable() const { return status == DriverStatus::AVAILABLE; }

    void addTrip(Trip* trip) {
        trips.push_back(trip);
    }

    void setCurrentLocation(Location* loc) {
        currentLocation = loc;
    }
};

// ==================== TRIP ====================
class Trip {
private:
    int tripId;
    TripState state;
    Driver* driver;
    Rider* rider;
    Location* source;
    Location* destination;
    int price;
    Payment* payment;
    long requestedTime;
    long acceptedTime;
    long completedTime;

public:
    Trip(int id, Rider* r, Location* src, Location* dest)
        : tripId(id), state(TripState::ACTIVE), driver(nullptr), rider(r),
          source(src), destination(dest), price(0), payment(nullptr),
          requestedTime(time(nullptr)), acceptedTime(0), completedTime(0) {}

    int getTripId() const { return tripId; }
    TripState getState() const { return state; }
    Driver* getDriver() const { return driver; }
    Rider* getRider() const { return rider; }
    Location* getSource() const { return source; }
    Location* getDestination() const { return destination; }
    int getPrice() const { return price; }
    Payment* getPayment() const { return payment; }

    void setPrice(int p) { price = p; }
    void setDriver(Driver* d) { driver = d; }
    void setPayment(Payment* p) { payment = p; }
    void setTripState(TripState s) { state = s; }

    void acceptTrip() {
        if (driver) {
            driver->setStatus(DriverStatus::ON_TRIP);
            acceptedTime = time(nullptr);
            cout << "Trip " << tripId << " accepted by driver " << driver->getName() << endl;
        }
    }

    void completeTrip() {
        state = TripState::COMPLETED;
        completedTime = time(nullptr);
        if (driver) {
            driver->setStatus(DriverStatus::AVAILABLE);
        }
        cout << "Trip " << tripId << " completed!" << endl;
    }

    void cancelTrip(string reason) {
        state = TripState::CANCELLED;
        if (driver) {
            driver->setStatus(DriverStatus::AVAILABLE);
        }
        cout << "Trip " << tripId << " cancelled. Reason: " << reason << endl;
    }
};

// ==================== TRIP SCHEDULING CONTROLLER ====================
class TripSchedulingController {
private:
    PricingStrategy* pricingStrategy;
    RiderSchedulingStrategy* riderSchedulingStrategy;
    int tripCounter;
    vector<Trip*> allTrips;

public:
    TripSchedulingController(PricingStrategy* pricing, RiderSchedulingStrategy* scheduling)
        : pricingStrategy(pricing), riderSchedulingStrategy(scheduling), tripCounter(0) {}

    Trip* scheduleTrip(Rider* rider, Location* source, Location* destination, vector<Driver*>& availableDrivers) {
        tripCounter++;
        Trip* trip = new Trip(tripCounter, rider, source, destination);

        // Calculate price
        int price = pricingStrategy->calculatePrice(source, destination);
        trip->setPrice(price);

        // Select driver
        Driver* selectedDriver = riderSchedulingStrategy->selectDriver(availableDrivers, source);
        if (selectedDriver) {
            trip->setDriver(selectedDriver);
            trip->acceptTrip();
            selectedDriver->addTrip(trip);
        } else {
            cout << "No available drivers found!" << endl;
        }

        rider->addTrip(trip);
        allTrips.push_back(trip);
        return trip;
    }

    void handleTripAcceptance(Trip* trip, Driver* driver) {
        if (trip && driver) {
            trip->setDriver(driver);
            trip->acceptTrip();
            driver->addTrip(trip);
            cout << "Trip acceptance handled" << endl;
        }
    }

    void handleTripCancellation(Trip* trip, string reason) {
        if (trip) {
            trip->cancelTrip(reason);
        }
    }
};

// ==================== CAB BOOKING SYSTEM ====================
class CabBookingSystem {
private:
    vector<Driver*> drivers;
    vector<Rider*> riders;
    TripSchedulingController* controller;
    NotificationService* notificationService;

public:
    CabBookingSystem(TripSchedulingController* ctrl, NotificationService* notif)
        : controller(ctrl), notificationService(notif) {}

    void addDriver(Driver* driver) {
        drivers.push_back(driver);
        driver->setStatus(DriverStatus::AVAILABLE);
        cout << "Driver " << driver->getName() << " added to system" << endl;
    }

    void addRider(Rider* rider) {
        riders.push_back(rider);
        cout << "Rider " << rider->getName() << " added to system" << endl;
    }

    Trip* requestTrip(Rider* rider, Location* source, Location* destination) {
        cout << "\n=== Trip Request ===" << endl;
        cout << "Rider: " << rider->getName() << endl;

        // Find available drivers
        vector<Driver*> availableDrivers;
        for (auto driver : drivers) {
            if (driver->isAvailable() && driver->getDrivingLicense()->isValid()) {
                availableDrivers.push_back(driver);
            }
        }

        if (availableDrivers.empty()) {
            cout << "No available drivers!" << endl;
            notificationService->notifyRider(rider, "No drivers available");
            return nullptr;
        }

        // Schedule trip
        Trip* trip = controller->scheduleTrip(rider, source, destination, availableDrivers);

        if (trip && trip->getDriver()) {
            notificationService->notifyDriver(trip->getDriver(), "New trip assigned");
            notificationService->notifyRider(rider, "Driver assigned: " + trip->getDriver()->getName());
        }

        return trip;
    }

    vector<Driver*> getDrivers() const { return drivers; }
    vector<Rider*> getRiders() const { return riders; }
};

// ==================== RIDER SCHEDULING IMPLEMENTATIONS ====================
Driver* NearestDriverStrategy::selectDriver(vector<Driver*> availableDrivers, Location* pickupLocation) {
    if (availableDrivers.empty()) return nullptr;
    Driver* nearest = availableDrivers[0];
    double minDistance = nearest->getCurrentLocation()->calculateDistance(pickupLocation);

    for (auto driver : availableDrivers) {
        double distance = driver->getCurrentLocation()->calculateDistance(pickupLocation);
        if (distance < minDistance) {
            minDistance = distance;
            nearest = driver;
        }
    }
    cout << "Selected nearest driver: " << nearest->getName() << " (Distance: " << minDistance << " km)" << endl;
    return nearest;
}

Driver* HighestRatedDriverStrategy::selectDriver(vector<Driver*> availableDrivers, Location* pickupLocation) {
    if (availableDrivers.empty()) return nullptr;
    Driver* highest = availableDrivers[0];

    for (auto driver : availableDrivers) {
        if (driver->getDriverRating() > highest->getDriverRating()) {
            highest = driver;
        }
    }
    cout << "Selected highest rated driver: " << highest->getName() << " (Rating: " << highest->getDriverRating() << ")" << endl;
    return highest;
}

Driver* RatingAndDistanceStrategy::selectDriver(vector<Driver*> availableDrivers, Location* pickupLocation) {
    if (availableDrivers.empty()) return nullptr;
    Driver* best = availableDrivers[0];
    double bestScore = best->getDriverRating() / (1 + best->getCurrentLocation()->calculateDistance(pickupLocation));

    for (auto driver : availableDrivers) {
        double score = driver->getDriverRating() / (1 + driver->getCurrentLocation()->calculateDistance(pickupLocation));
        if (score > bestScore) {
            bestScore = score;
            best = driver;
        }
    }
    cout << "Selected best driver (rating+distance): " << best->getName() << endl;
    return best;
}

// ==================== DEMO ====================
int main() {
    cout << "===== CAB BOOKING SYSTEM =====" << endl << endl;

    // Create locations
    Location* homeLocation = new Location(40.7128, -74.0060);      // New York
    Location* workLocation = new Location(40.7580, -73.9855);      // Midtown
    Location* driver1Location = new Location(40.7489, -73.9680);   // Near Times Square
    Location* driver2Location = new Location(40.6892, -74.0445);   // Brooklyn

    // Create vehicles
    Vehicle* vehicle1 = new Vehicle("V001", "Sedan", 4, "ABC123");
    Vehicle* vehicle2 = new Vehicle("V002", "SUV", 6, "XYZ789");

    // Create licenses
    License* license1 = new License("DL001", "B", time(nullptr) + 365 * 24 * 3600); // Valid
    License* license2 = new License("DL002", "B", time(nullptr) + 365 * 24 * 3600);

    // Create drivers
    Driver* driver1 = new Driver(1, "John", 4, license1, driver1Location, vehicle1);
    Driver* driver2 = new Driver(2, "Sarah", 5, license2, driver2Location, vehicle2);

    // Create payment methods
    PaymentMethod* pm1 = new PaymentMethod("PM001", "CreditCard");
    PaymentMethod* pm2 = new PaymentMethod("PM002", "UPI");

    // Create riders
    Rider* rider1 = new Rider(101, "Alice", 4, homeLocation, pm1);
    Rider* rider2 = new Rider(102, "Bob", 3, homeLocation, pm2);

    // Create strategies
    PricingStrategy* pricingStrategy = new DistanceBasedPricingStrategy(10); // $10 per km
    RiderSchedulingStrategy* schedulingStrategy = new NearestDriverStrategy();

    // Create controller
    TripSchedulingController* controller = new TripSchedulingController(pricingStrategy, schedulingStrategy);

    // Create notification service
    NotificationService* notificationService = new PushNotification();

    // Create cab booking system
    CabBookingSystem* cabSystem = new CabBookingSystem(controller, notificationService);

    // Add drivers and riders
    cabSystem->addDriver(driver1);
    cabSystem->addDriver(driver2);
    cabSystem->addRider(rider1);
    cabSystem->addRider(rider2);

    cout << endl;

    // Request trips
    Trip* trip1 = cabSystem->requestTrip(rider1, homeLocation, workLocation);
    cout << "Trip Price: $" << trip1->getPrice() << endl;

    cout << endl;

    Trip* trip2 = cabSystem->requestTrip(rider2, homeLocation, workLocation);
    cout << "Trip Price: $" << trip2->getPrice() << endl;

    cout << endl;

    // Process payment
    if (trip1) {
        Payment* payment1 = new CreditCardPayment(1001, trip1->getPrice(), "1234-5678-9012-3456");
        trip1->setPayment(payment1);
        payment1->processPayment();

        // Complete trip
        trip1->completeTrip();
    }

    delete cabSystem;
    return 0;
}

