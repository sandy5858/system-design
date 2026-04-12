#include "../../all.h"
using namespace std;

class State {
    public:
        virtual void insertQuarter() = 0;
        virtual void ejectQuarter() = 0;
        virtual void turnCrank() = 0;
        virtual void dispense() = 0;
};

class GumballMachine {
    private:
        State* soldOutState;
        State* noQuarterState;
        State* hasQuarterState;
        State* soldState;

        State* state;
        int count;

    public:
        GumballMachine(int numberGumballs);
        void insertQuarter() {
            state->insertQuarter();
        }
        void ejectQuarter() {
            state->ejectQuarter();
        }
        void turnCrank() {
            state->turnCrank();
            state->dispense();
        }
        void setState(State* state) {
            this->state = state;
        }
        State* getNoQuarterState() {
            return noQuarterState;
        }
        State* getHasQuarterState() {
            return hasQuarterState;
        }
        State* getSoldState() {
            return soldState;
        }
        State* getSoldOutState() {
            return soldOutState;
        }
        int getCount() {
            return count;
        }
        void releaseBall() {
            cout << "A gumball comes rolling out the slot..." << endl;
            if (count != 0) {
                count = count - 1;
            }
        }
};

class NoQuarterState : public State {
    private:
        GumballMachine* gumballMachine;
    public:
        NoQuarterState(GumballMachine* gumballMachine) {
            this->gumballMachine = gumballMachine;
        }
        void insertQuarter() {
            cout << "You inserted a quarter." << endl;
            gumballMachine->setState(gumballMachine->getHasQuarterState());
        }
        void ejectQuarter() {
            cout << "You haven't inserted a quarter." << endl;
        }
        void turnCrank() {
            cout << "You turned, but there's no quarter." << endl;
        }
        void dispense() {
            cout << "You need to pay first." << endl;
        }
};

class HasQuarterState : public State {
    private:
        GumballMachine* gumballMachine;
    public:
        HasQuarterState(GumballMachine* gumballMachine) {
            this->gumballMachine = gumballMachine;
        }
        void insertQuarter() {
            cout << "You can't insert another quarter." << endl;
        }
        void ejectQuarter() {
            cout << "Quarter returned." << endl;
            gumballMachine->setState(gumballMachine->getNoQuarterState());
        }
        void turnCrank() {
            cout << "You turned..." << endl;
            gumballMachine->setState(gumballMachine->getSoldState());
        }
        void dispense() {
            cout << "No gumball dispensed." << endl;
        }
};

class SoldState : public State {
    private:
        GumballMachine* gumballMachine;
    public:
        SoldState(GumballMachine* gumballMachine) {
            this->gumballMachine = gumballMachine;
        }
        void insertQuarter() {
            cout << "Please wait, we're already giving you a gumball." << endl;
        }
        void ejectQuarter() {
            cout << "Sorry, you already turned the crank." << endl;
        }
        void turnCrank() {
            cout << "Turning twice doesn't get you another gumball!" << endl;
        }
        void dispense() {
            gumballMachine->releaseBall();
            if (gumballMachine->getCount() > 0) {
                gumballMachine->setState(gumballMachine->getNoQuarterState());
            } else {
                cout << "Oops, out of gumballs!" << endl;
                gumballMachine->setState(gumballMachine->getSoldOutState());
            }
        }
};

class SoldOutState : public State {
    private:
        GumballMachine* gumballMachine;
    public:
        SoldOutState(GumballMachine* gumballMachine) {
            this->gumballMachine = gumballMachine;
        }
        void insertQuarter() {
            cout << "You can't insert a quarter, the machine is sold out." << endl;
        }
        void ejectQuarter() {
            cout << "You can't eject, you haven't inserted a quarter yet." << endl;
        }
        void turnCrank() {
            cout << "You turned, but there are no gumballs." << endl;
        }
        void dispense() {
            cout << "No gumball dispensed." << endl;
        }
};

GumballMachine::GumballMachine(int numberGumballs) {
    soldOutState = new SoldOutState(this);
    noQuarterState = new NoQuarterState(this);
    hasQuarterState = new HasQuarterState(this);
    soldState = new SoldState(this);

    this->count = numberGumballs;
    if (numberGumballs > 0) {
        state = noQuarterState;
    } else {
        state = soldOutState;
    }
}

int main() {
    GumballMachine* gumballMachine = new GumballMachine(5);

    gumballMachine->insertQuarter();
    gumballMachine->turnCrank();

    gumballMachine->insertQuarter();
    gumballMachine->ejectQuarter();
    gumballMachine->turnCrank();

    gumballMachine->insertQuarter();
    gumballMachine->turnCrank();
    gumballMachine->insertQuarter();
    gumballMachine->turnCrank();
    gumballMachine->insertQuarter();
    gumballMachine->turnCrank();
    gumballMachine->insertQuarter();
    gumballMachine->turnCrank();

    return 0;
}