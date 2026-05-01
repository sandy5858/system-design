/* Elevator LLD in C++

Design Patterns Used:
1. State Pattern      -> Elevator states
2. Strategy Pattern   -> Movement strategies

Objects
1. Request
- int floor;
- Direction direction;
- RequestType type;

2. Elevator
- int id;
- int currentFloor;
- Direction direction;
- ElevatorState* state;
- set<int> upStops;
- set<int, greater<int>> downStops;
- void setState(ElevatorState* newState);
- void addRequest(Request req);
- void step();
- void openDoor();
- void closeDoor();
- int getCurrentFloor();
- Direction getDirection();
- int getId();
- set<int>& getUpStops();
- set<int, greater<int>>& getDownStops();
- void setDirection(Direction d);

3. ElevatorState (abstract)
- virtual void handle(Elevator* elevator) = 0;
-> IdleState
-> MovingUpState
-> MovingDownState

4. SchedulingStrategy (abstract)
- virtual Elevator* selectElevator(vector<Elevator*>& elevators, Request req) = 0;
-> NearestElevatorStrategy

5. ElevatorController
- vector<Elevator*> elevators;
- SchedulingStrategy* strategy;
- void submitRequest(Request req);
- void stepAll();

6. ElevatorSystem
- ElevatorController* controller;
- void requestElevator(int floor, Direction dir);
- void step();
*/

#include "../../all.h"
using namespace std;

// ================= ENUMS =================

enum Direction {
    UP,
    DOWN,
    IDLE
};

enum RequestType {
    INTERNAL,
    EXTERNAL
};

// ================= REQUEST =================

class Request {
public:
    int floor;
    Direction direction;
    RequestType type;

    Request(int f, Direction d, RequestType t)
        : floor(f), direction(d), type(t) {}
};

// Forward declaration
class Elevator;

// ================= STATE PATTERN =================

class ElevatorState {
public:
    virtual void handle(Elevator* elevator) = 0;
    virtual ~ElevatorState() {}
};

// ================= ELEVATOR =================

class Elevator {
private:
    int id;
    int currentFloor;
    Direction direction;
    ElevatorState* state;

    set<int> upStops;
    set<int, greater<int>> downStops;

public:
    Elevator(int id);

    void setState(ElevatorState* newState) {
        state = newState;
    }

    void addRequest(Request req);

    void step(); // simulate one unit movement

    void openDoor() {
        cout << "Elevator " << id << " opening door at floor " << currentFloor << endl;
    }

    void closeDoor() {
        cout << "Elevator " << id << " closing door\n";
    }

    int getCurrentFloor() { return currentFloor; }
    Direction getDirection() { return direction; }
    int getId() { return id; }

    set<int>& getUpStops() { return upStops; }
    set<int, greater<int>>& getDownStops() { return downStops; }

    void setDirection(Direction d) { direction = d; }
};

// ================= STATES =================

class IdleState : public ElevatorState {
public:
    void handle(Elevator* elevator) override;
};

class MovingUpState : public ElevatorState {
public:
    void handle(Elevator* elevator) override;
};

class MovingDownState : public ElevatorState {
public:
    void handle(Elevator* elevator) override;
};

// ================= STATE IMPLEMENTATION =================

void IdleState::handle(Elevator* elevator) {
    if (!elevator->getUpStops().empty()) {
        elevator->setDirection(UP);
        elevator->setState(new MovingUpState());
    } else if (!elevator->getDownStops().empty()) {
        elevator->setDirection(DOWN);
        elevator->setState(new MovingDownState());
    }
}

void MovingUpState::handle(Elevator* elevator) {
    auto& stops = elevator->getUpStops();
    if (stops.empty()) {
        elevator->setState(new IdleState());
        elevator->setDirection(IDLE);
        return;
    }

    int next = *stops.begin();

    if (elevator->getCurrentFloor() == next) {
        elevator->openDoor();
        stops.erase(next);
        elevator->closeDoor();
    } else {
        elevator->setDirection(UP);
        cout << "Elevator " << elevator->getId()
             << " moving UP to floor "
             << elevator->getCurrentFloor() + 1 << endl;
    }
}

void MovingDownState::handle(Elevator* elevator) {
    auto& stops = elevator->getDownStops();
    if (stops.empty()) {
        elevator->setState(new IdleState());
        elevator->setDirection(IDLE);
        return;
    }

    int next = *stops.begin();

    if (elevator->getCurrentFloor() == next) {
        elevator->openDoor();
        stops.erase(next);
        elevator->closeDoor();
    } else {
        elevator->setDirection(DOWN);
        cout << "Elevator " << elevator->getId()
             << " moving DOWN to floor "
             << elevator->getCurrentFloor() - 1 << endl;
    }
}

// ================= FIX: PROPER FLOOR UPDATE =================

void Elevator::step() {
    if (state) state->handle(this);

    // Safe movement update
    if (direction == UP) currentFloor++;
    else if (direction == DOWN) currentFloor--;
}

// ================= ELEVATOR IMPLEMENTATION =================

Elevator::Elevator(int id) {
    this->id = id;
    currentFloor = 0;
    direction = IDLE;
    state = new IdleState();
}

void Elevator::addRequest(Request req) {
    if (req.floor > currentFloor) {
        upStops.insert(req.floor);
    } else if (req.floor < currentFloor) {
        downStops.insert(req.floor);
    } else {
        openDoor();
    }
}

// ================= STRATEGY PATTERN =================

class SchedulingStrategy {
public:
    virtual Elevator* selectElevator(vector<Elevator*>& elevators, Request req) = 0;
    virtual ~SchedulingStrategy() {}
};

class NearestElevatorStrategy : public SchedulingStrategy {
public:
    Elevator* selectElevator(vector<Elevator*>& elevators, Request req) override {
        Elevator* best = nullptr;
        int minDist = INT_MAX;

        for (auto e : elevators) {
            int dist = abs(e->getCurrentFloor() - req.floor);
            if (dist < minDist) {
                minDist = dist;
                best = e;
            }
        }
        return best;
    }
};

// ================= CONTROLLER =================

class ElevatorController {
private:
    vector<Elevator*> elevators;
    SchedulingStrategy* strategy;

public:
    ElevatorController(vector<Elevator*> elevs, SchedulingStrategy* strat)
        : elevators(elevs), strategy(strat) {}

    void submitRequest(Request req) {
        Elevator* e = strategy->selectElevator(elevators, req);
        cout << "Assigning Elevator " << e->getId()
             << " to floor " << req.floor << endl;
        e->addRequest(req);
    }

    void stepAll() {
        for (auto e : elevators) {
            e->step();
        }
    }
};

// ================= SYSTEM =================

class ElevatorSystem {
private:
    ElevatorController* controller;

public:
    ElevatorSystem(ElevatorController* ctrl) : controller(ctrl) {}

    void requestElevator(int floor, Direction dir) {
        Request req(floor, dir, EXTERNAL);
        controller->submitRequest(req);
    }

    void step() {
        controller->stepAll();
    }
};

// ================= MAIN =================

int main() {
    Elevator* e1 = new Elevator(1);
    Elevator* e2 = new Elevator(2);

    vector<Elevator*> elevators = {e1, e2};

    SchedulingStrategy* strategy = new NearestElevatorStrategy();
    ElevatorController* controller = new ElevatorController(elevators, strategy);

    ElevatorSystem system(controller);

    // Simulate requests
    system.requestElevator(5, UP);
    system.requestElevator(2, DOWN);
    system.requestElevator(8, DOWN);

    cout << "\n--- Simulation Start ---\n";

    for (int i = 0; i < 15; i++) {
        system.step();
    }

    return 0;
}