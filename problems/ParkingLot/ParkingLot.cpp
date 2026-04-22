#include <iostream>
#include <vector>
#include <memory>
#include <ctime>
using namespace std;

enum class VehicleType { CAR, BIKE, TRUCK };


// =====================================================
// ABSTRACT BASE CLASS + POLYMORPHISM
// Used for runtime flexibility of different vehicles
// =====================================================
class Vehicle {
protected:
    string plate;

public:
    Vehicle(string p) : plate(p) {}

    virtual VehicleType getType() = 0;   // Runtime polymorphism

    string getPlate() { return plate; }

    virtual ~Vehicle() = default;
};


// =====================================================
// INHERITANCE
// Specialized vehicle classes
// =====================================================
class Car : public Vehicle {
public:
    Car(string p) : Vehicle(p) {}

    VehicleType getType() override {
        return VehicleType::CAR;
    }
};

class Bike : public Vehicle {
public:
    Bike(string p) : Vehicle(p) {}

    VehicleType getType() override {
        return VehicleType::BIKE;
    }
};

class Truck : public Vehicle {
public:
    Truck(string p) : Vehicle(p) {}

    VehicleType getType() override {
        return VehicleType::TRUCK;
    }
};


// =====================================================
// FACTORY PATTERN
// Encapsulates object creation logic
// Client does not use new Car(), new Bike()
// =====================================================
class VehicleFactory {
public:
    static unique_ptr<Vehicle> createVehicle(
        VehicleType type,
        string plate
    ) {
        if (type == VehicleType::CAR)
            return make_unique<Car>(plate);

        if (type == VehicleType::BIKE)
            return make_unique<Bike>(plate);

        return make_unique<Truck>(plate);
    }
};


// =====================================================
// STRATEGY PATTERN
// Defines interchangeable pricing algorithms
// =====================================================
class PricingStrategy {
public:
    virtual double calculateFee(int hours) = 0;

    virtual ~PricingStrategy() = default;
};


// Concrete Strategy #1
class HourlyPricing : public PricingStrategy {
public:
    double calculateFee(int hours) override {
        return hours * 20;
    }
};


// Concrete Strategy #2
class PremiumPricing : public PricingStrategy {
public:
    double calculateFee(int hours) override {
        return hours * 40;
    }
};


// =====================================================
// OBSERVER-LIKE PATTERN
// Display board reacts to parking state changes
// (Simplified observer implementation)
// =====================================================
class DisplayBoard {
public:
    void update(int freeSpots) {
        cout << "Available Spots: "
             << freeSpots << endl;
    }
};


class ParkingSpot {
private:
    int id;
    bool occupied = false;

public:
    ParkingSpot(int i) : id(i) {}

    int getId() { return id; }

    bool isFree() {
        return !occupied;
    }

    void park() {
        occupied = true;
    }

    void free() {
        occupied = false;
    }
};


// =====================================================
// DATA OBJECT
// Encapsulates parking session info
// =====================================================
class Ticket {
public:
    int id;
    int spotId;
    time_t entryTime;
    string vehicleNo;

    Ticket(int i, int s, string v) {
        id = i;
        spotId = s;
        entryTime = time(nullptr);
        vehicleNo = v;
    }
};


// =====================================================
// SINGLETON PATTERN
// Only one ParkingLot instance exists
// Global access point via getInstance()
// =====================================================
class ParkingLot {
private:
    static ParkingLot* instance;

    // COMPOSITION:
    // ParkingLot owns these objects
    vector<ParkingSpot> spots;
    DisplayBoard board;

    // STRATEGY via dependency
    unique_ptr<PricingStrategy> strategy;

    int ticketId = 1;

    // Private constructor => Singleton
    ParkingLot() {
        strategy = make_unique<HourlyPricing>();

        for (int i = 1; i <= 5; i++) {
            spots.push_back(ParkingSpot(i));
        }
    }

public:
    static ParkingLot* getInstance() {
        if (instance == nullptr) {
            instance = new ParkingLot();
        }
        return instance;
    }

    Ticket parkVehicle(string vehicleNo) {

        for (auto& spot : spots) {

            if (spot.isFree()) {

                spot.park();

                // Observer-like notification
                board.update(getFreeSpots());

                return Ticket(ticketId++, spot.getId(), vehicleNo);
            }
        }

        throw runtime_error("No Spots Available");
    }

    void unparkVehicle(Ticket& ticket) {

        // Find the spot by id
        for (auto& spot : spots) {
            if (spot.getId() == ticket.spotId) {
                spot.free();
                break;
            }
        }

        time_t now = time(nullptr);
        int hours = (now - ticket.entryTime) / 3600;
        if (hours == 0) hours = 1;  // Minimum 1 hour

        // Strategy Pattern usage
        cout << "Fee: Rs."
             << strategy->calculateFee(hours)
             << endl;

        // Observer-like update
        board.update(getFreeSpots());
    }

    int getFreeSpots() {
        int cnt = 0;

        for (auto& s : spots) {
            if (s.isFree())
                cnt++;
        }

        return cnt;
    }
};

ParkingLot* ParkingLot::instance = nullptr;


// =====================================================
// CLIENT CODE
// =====================================================
int main() {

    // FACTORY PATTERN used here
    auto car =
        VehicleFactory::createVehicle(
            VehicleType::CAR,
            "WB06A1234"
        );

    // SINGLETON used here
    ParkingLot* lot =
        ParkingLot::getInstance();

    try {
        Ticket t =
            lot->parkVehicle(car->getPlate());

        // Simulate some time passing
        // In real code, use sleep or something, but for demo:
        // sleep(2); but to avoid, just proceed

        lot->unparkVehicle(t);

        // Test parking another vehicle
        auto bike = VehicleFactory::createVehicle(VehicleType::BIKE, "BIKE123");
        Ticket t2 = lot->parkVehicle(bike->getPlate());
        lot->unparkVehicle(t2);

    } catch (const runtime_error& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}