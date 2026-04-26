// ===============================================================
// ATM LLD in C++ using REAL State Pattern
//
// Design Patterns Used:
// 1. State Pattern      -> ATM states
// 2. Strategy Pattern   -> Transactions
// 3. Factory Pattern    -> Transaction creation
// 4. Singleton Pattern  -> ATMSystem
//
// ===============================================================

#include <iostream>
#include <memory>
#include <unordered_map>
using namespace std;

class ATM;
class ATMState;
class Account;


// ===============================================================
// ACCOUNT
// ===============================================================
class Account {
private:
    string accNo;
    int pin;
    double balance;

public:
    Account(string a, int p, double b)
        : accNo(a), pin(p), balance(b) {}

    bool validatePin(int enteredPin) {
        return pin == enteredPin;
    }

    double getBalance() { return balance; }

    bool withdraw(double amt) {
        if (amt > balance) return false;
        balance -= amt;
        return true;
    }

    void deposit(double amt) {
        balance += amt;
    }
};


// ===============================================================
// CARD
// ===============================================================
class Card {
private:
    string cardNo;
    Account* account;

public:
    Card(string c, Account* a)
        : cardNo(c), account(a) {}

    string getCardNo() { return cardNo; }

    Account* getAccount() {
        return account;
    }
};


// ===============================================================
// DATABASE
// ===============================================================
class BankDatabase {
private:
    unordered_map<string, Card*> cards;

public:
    void addCard(Card* c) {
        cards[c->getCardNo()] = c;
    }

    Card* getCard(string cardNo) {
        if (cards.count(cardNo))
            return cards[cardNo];
        return nullptr;
    }
};


// ===============================================================
// STATE INTERFACE
// ===============================================================
class ATMState {
public:
    virtual void insertCard(ATM*, string) = 0;
    virtual void enterPin(ATM*, int) = 0;
    virtual void withdraw(ATM*, double) = 0;
    virtual void ejectCard(ATM*) = 0;
    virtual void showBalance(ATM*) = 0;

    virtual ~ATMState() = default;
};


// Forward declare states
class IdleState;
class HasCardState;
class AuthenticatedState;


// ===============================================================
// ATM MACHINE
// ===============================================================
class ATM {
private:
    ATMState* currentState;
    BankDatabase* db;

    Card* currentCard = nullptr;
    Account* currentAccount = nullptr;

public:
    ATM(BankDatabase* database);

    void setState(ATMState* state) {
        currentState = state;
    }

    ATMState* getState() {
        return currentState;
    }

    BankDatabase* getDB() {
        return db;
    }

    void setCard(Card* c) {
        currentCard = c;
    }

    Card* getCard() {
        return currentCard;
    }

    void setAccount(Account* a) {
        currentAccount = a;
    }

    Account* getAccount() {
        return currentAccount;
    }

    void clearSession() {
        currentCard = nullptr;
        currentAccount = nullptr;
    }

    // Delegate behavior to state
    void insertCard(string cardNo) {
        currentState->insertCard(this, cardNo);
    }

    void enterPin(int pin) {
        currentState->enterPin(this, pin);
    }

    void withdraw(double amt) {
        currentState->withdraw(this, amt);
    }

    void showBalance() {
        currentState->showBalance(this);
    }

    void ejectCard() {
        currentState->ejectCard(this);
    }
};


// ===============================================================
// IDLE STATE
// ===============================================================
class IdleState : public ATMState {
public:
    void insertCard(ATM* atm, string cardNo) override;
    void enterPin(ATM*, int) override;
    void withdraw(ATM*, double) override;
    void ejectCard(ATM*) override;
    void showBalance(ATM*) override;
};


// ===============================================================
// HAS CARD STATE
// ===============================================================
class HasCardState : public ATMState {
public:
    void insertCard(ATM*, string) override;
    void enterPin(ATM* atm, int pin) override;
    void withdraw(ATM*, double) override;
    void ejectCard(ATM* atm) override;
    void showBalance(ATM*) override;
};


// ===============================================================
// AUTHENTICATED STATE
// ===============================================================
class AuthenticatedState : public ATMState {
public:
    void insertCard(ATM*, string) override;
    void enterPin(ATM*, int) override;
    void withdraw(ATM* atm, double amt) override;
    void ejectCard(ATM* atm) override;
    void showBalance(ATM*) override;
};


// ===============================================================
// STATE IMPLEMENTATIONS
// ===============================================================
void IdleState::insertCard(ATM* atm, string cardNo) {
    Card* card =
        atm->getDB()->getCard(cardNo);

    if (!card) {
        cout << "Invalid Card\n";
        return;
    }

    atm->setCard(card);

    cout << "Card Inserted\n";

    atm->setState(new HasCardState());
}

void IdleState::enterPin(ATM*, int) {
    cout << "Insert card first\n";
}

void IdleState::withdraw(ATM*, double) {
    cout << "Insert card first\n";
}

void IdleState::ejectCard(ATM*) {
    cout << "No Card Present\n";
}

void IdleState::showBalance(ATM*) {
    cout << "Insert card first\n";
}

void HasCardState::insertCard(ATM*, string) {
    cout << "Card already inserted\n";
}

void HasCardState::enterPin(ATM* atm, int pin) {
    Account* acc =
        atm->getCard()->getAccount();

    if (acc->validatePin(pin)) {
        atm->setAccount(acc);

        cout << "PIN Verified\n";

        atm->setState(
            new AuthenticatedState()
        );
    }
    else {
        cout << "Wrong PIN\n";
    }
}

void HasCardState::withdraw(ATM*, double) {
    cout << "Enter PIN first\n";
}

void HasCardState::showBalance(ATM*) {
    cout << "Enter PIN first\n";
}

void HasCardState::ejectCard(ATM* atm) {
    cout << "Card Ejected\n";
    atm->clearSession();
    atm->setState(new IdleState());
}

void AuthenticatedState::insertCard(ATM*, string) {
    cout << "Transaction in progress\n";
}

void AuthenticatedState::enterPin(ATM*, int) {
    cout << "Already authenticated\n";
}

void AuthenticatedState::withdraw(ATM* atm, double amt) {
    if (atm->getAccount()->withdraw(amt))
        cout << "Please collect cash Rs."
             << amt << endl;
    else
        cout << "Insufficient Balance\n";
}

void AuthenticatedState::showBalance(ATM* atm) {
    cout << "Available balance: Rs." 
         << atm->getAccount()->getBalance() << endl;
}

void AuthenticatedState::ejectCard(ATM* atm) {
    cout << "Card Ejected\n";
    atm->clearSession();
    atm->setState(new IdleState());
}


// ===============================================================
// ATM CONSTRUCTOR
// ===============================================================
ATM::ATM(BankDatabase* database) {
    db = database;
    currentState = new IdleState();
}


// ===============================================================
// SINGLETON SYSTEM
// ===============================================================
class ATMSystem {
private:
    static ATMSystem* instance;
    BankDatabase db;

    ATMSystem() {}

public:
    static ATMSystem* getInstance() {
        if (!instance)
            instance = new ATMSystem();
        return instance;
    }

    BankDatabase* getDB() {
        return &db;
    }
};

ATMSystem* ATMSystem::instance = nullptr;


// ===============================================================
// MAIN
// ===============================================================
int main() {

    ATMSystem* sys =
        ATMSystem::getInstance();

    BankDatabase* db =
        sys->getDB();

    Account acc1("SB1001", 1234, 50000);
    Card card1("ATM9999", &acc1);

    db->addCard(&card1);

    ATM atm(db);

    atm.insertCard("ATM9999");
    atm.enterPin(1234);
    atm.showBalance();
    atm.withdraw(5000);
    atm.showBalance();
    atm.ejectCard();

    return 0;
}