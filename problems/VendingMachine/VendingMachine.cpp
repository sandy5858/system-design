/* Vending Machine LLD in C++ 

Design Patterns Used:
1. State Pattern      -> Vending Machine states
2. Singleton Pattern  -> VendingMachineSystem

Objects
1. VendingMachine
- VendingMachineState* currentState
- vector<Slot> slots;

2. VendingMachineState
- void selectSlot(int slotId)
- void scan&Pay()
- void dispense(Product* product)
- void cancel()
-> IdleState
-> SlotSelectedState
-> PaidState
-> SoldState

3. Slot
- int slotId
- Product* product
- int quantity

4. Product
- int productId
- int price
*/

// ============================================================================
// VENDING MACHINE LLD in C++
//
// Base Design Used:
// 1. State Pattern      -> Machine behavior changes by current state
// 2. Singleton Pattern  -> VendingMachineSystem
//
// States:
// IdleState
// SlotSelectedState
// PaidState
// SoldState
//
// ============================================================================

#include <iostream>
#include <vector>
#include <memory>
using namespace std;

class VendingMachine;
class VendingMachineState;
class IdleState;
class SlotSelectedState;
class PaidState;
class SoldState;


// ============================================================================
// PRODUCT
// ============================================================================
class Product {
public:
    int productId;
    string name;
    int price;

    Product(int id, string n, int p)
        : productId(id), name(n), price(p) {}
};


// ============================================================================
// SLOT
// One physical compartment in vending machine
// ============================================================================
class Slot {
public:
    int slotId;
    Product* product;
    int quantity;

    Slot(int id, Product* p, int qty)
        : slotId(id), product(p), quantity(qty) {}

    bool isAvailable() {
        return quantity > 0;
    }

    void reduceQuantity() {
        if (quantity > 0)
            quantity--;
    }
};


// ============================================================================
// STATE INTERFACE
// ============================================================================
class VendingMachineState {
public:
    virtual void selectSlot(VendingMachine*, int slotId) = 0;
    virtual void scanAndPay(VendingMachine*) = 0;
    virtual void dispense(VendingMachine*) = 0;
    virtual void cancel(VendingMachine*) = 0;

    virtual ~VendingMachineState() = default;
};


// ============================================================================
// VENDING MACHINE
// Context Class for State Pattern
// ============================================================================
class VendingMachine {
private:
    VendingMachineState* currentState;

public:
    vector<Slot> slots;

    Slot* selectedSlot = nullptr;

    VendingMachine();

    void setState(VendingMachineState* state) {
        currentState = state;
    }

    VendingMachineState* getState() {
        return currentState;
    }

    Slot* getSlotById(int id) {
        for (auto& slot : slots) {
            if (slot.slotId == id)
                return &slot;
        }
        return nullptr;
    }

    // Delegate behavior to current state
    void selectSlot(int slotId) {
        currentState->selectSlot(this, slotId);
    }

    void scanAndPay() {
        currentState->scanAndPay(this);
    }

    void dispense() {
        currentState->dispense(this);
    }

    void cancel() {
        currentState->cancel(this);
    }
};


// ============================================================================
// SOLD STATE
// Transaction complete
// ============================================================================
class SoldState : public VendingMachineState {
public:
    void selectSlot(VendingMachine* vm, int slotId) override;
    void scanAndPay(VendingMachine* vm) override;
    void dispense(VendingMachine* vm) override;
    void cancel(VendingMachine* vm) override;
    ~SoldState() override {}
    SoldState() = default;
};


// ============================================================================
// PAID STATE
// Ready to dispense product
// ============================================================================
class PaidState : public VendingMachineState {
public:
    void selectSlot(VendingMachine* vm, int slotId) override;
    void scanAndPay(VendingMachine* vm) override;
    void dispense(VendingMachine* vm) override;
    void cancel(VendingMachine* vm) override;
};


// ============================================================================
// SLOT SELECTED STATE
// Waiting for payment
// ============================================================================
class SlotSelectedState : public VendingMachineState {
public:
    void selectSlot(VendingMachine* vm, int slotId) override;
    void scanAndPay(VendingMachine* vm) override;
    void dispense(VendingMachine* vm) override;
    void cancel(VendingMachine* vm) override;
};


// ============================================================================
// IDLE STATE
// Waiting for user to select slot
// ============================================================================
class IdleState : public VendingMachineState {
public:
    void selectSlot(VendingMachine* vm, int slotId) override;
    void scanAndPay(VendingMachine* vm) override;
    void dispense(VendingMachine* vm) override;
    void cancel(VendingMachine* vm) override;
};


// ============================================================================
// STATE IMPLEMENTATIONS
// ============================================================================
void SoldState::selectSlot(VendingMachine*, int) {
    cout << "Please wait...\n";
}

void SoldState::scanAndPay(VendingMachine*) {
    cout << "Please wait...\n";
}

void SoldState::dispense(VendingMachine*) {
    cout << "Already dispensed\n";
}

void SoldState::cancel(VendingMachine*) {
    cout << "Resetting Machine\n";
}


void PaidState::selectSlot(VendingMachine*, int) {
    cout << "Already paid\n";
}

void PaidState::scanAndPay(VendingMachine*) {
    cout << "Payment already done\n";
}

void PaidState::dispense(VendingMachine* vm) {
    vm->selectedSlot->reduceQuantity();

    cout << "Dispensing Product: "
         << vm->selectedSlot->product->name
         << endl;

    vm->setState(new SoldState());
}

void PaidState::cancel(VendingMachine*) {
    cout << "Cannot cancel after payment\n";
}


void SlotSelectedState::selectSlot(VendingMachine*, int) {
    cout << "Slot already selected\n";
}

void SlotSelectedState::scanAndPay(VendingMachine* vm) {
    cout << "Payment Successful via QR Scan\n";
    vm->setState(new PaidState());
}

void SlotSelectedState::dispense(VendingMachine*) {
    cout << "Please complete payment first\n";
}

void SlotSelectedState::cancel(VendingMachine* vm) {
    cout << "Selection Cancelled\n";
    vm->selectedSlot = nullptr;
    vm->setState(new IdleState());
}


void IdleState::selectSlot(VendingMachine* vm, int slotId) {
    Slot* slot = vm->getSlotById(slotId);

    if (!slot) {
        cout << "Invalid Slot\n";
        return;
    }

    if (!slot->isAvailable()) {
        cout << "Out of Stock\n";
        return;
    }

    vm->selectedSlot = slot;

    cout << "Selected Slot: " << slotId
         << " Product: " << slot->product->name
         << " Price: Rs." << slot->product->price
         << endl;

    vm->setState(new SlotSelectedState());
}

void IdleState::scanAndPay(VendingMachine*) {
    cout << "Select slot first\n";
}

void IdleState::dispense(VendingMachine*) {
    cout << "Payment pending\n";
}

void IdleState::cancel(VendingMachine*) {
    cout << "Nothing to cancel\n";
}


// ============================================================================
// MACHINE CONSTRUCTOR
// ============================================================================
VendingMachine::VendingMachine() {
    currentState = new IdleState();
}


// ============================================================================
// SINGLETON SYSTEM
// One machine system manager
// ============================================================================
class VendingMachineSystem {
private:
    static VendingMachineSystem* instance;

    VendingMachine machine;

    VendingMachineSystem() {}

public:
    static VendingMachineSystem* getInstance() {

        if (!instance)
            instance = new VendingMachineSystem();

        return instance;
    }

    VendingMachine* getMachine() {
        return &machine;
    }
};

VendingMachineSystem*
VendingMachineSystem::instance = nullptr;


// ============================================================================
// MAIN
// ============================================================================
int main() {

    Product coke(1, "Coke", 40);
    Product chips(2, "Chips", 30);
    Product juice(3, "Juice", 50);

    VendingMachineSystem* system =
        VendingMachineSystem::getInstance();

    VendingMachine* vm =
        system->getMachine();

    vm->slots.push_back(Slot(101, &coke, 5));
    vm->slots.push_back(Slot(102, &chips, 3));
    vm->slots.push_back(Slot(103, &juice, 2));

    vm->selectSlot(101);
    vm->scanAndPay();
    vm->dispense();

    return 0;
}