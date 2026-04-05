#include "../../all.h"
using namespace std;

class Beverage {
    protected:
        string description = "Unknown Beverage";
    public:
        virtual string getDescription() {
            return description;
        }
        virtual double cost() = 0;
};

class CondimentDecorator : public Beverage {
    protected:
        Beverage* beverage;
    public:
        virtual string getDescription() = 0;
};

class Espresso : public Beverage {
    public:
        Espresso() {
            description = "Espresso";
        }
        double cost() {
            return 1.99;
        }
};

class HouseBlend : public Beverage {
    public:
        HouseBlend() {
            description = "House Blend Coffee";
        }
        double cost() {
            return 0.89;
        }
};

class Mocha : public CondimentDecorator {
    public:
        Mocha(Beverage* beverage) {
            this->beverage = beverage;
        }
        string getDescription() {
            return beverage->getDescription() + ", Mocha";
        }
        double cost() {
            return 0.20 + beverage->cost();
        }
};

class Whip : public CondimentDecorator {
    public:
        Whip(Beverage* beverage) {
            this->beverage = beverage;
        }
        string getDescription() {
            return beverage->getDescription() + ", Whip";
        }
        double cost() {
            return 0.10 + beverage->cost();
        }
};

int main() {
    Beverage* beverage = new Espresso();
    cout << beverage->getDescription() << " $" << beverage->cost() << endl;

    Beverage* beverage2 = new HouseBlend();
    beverage2 = new Mocha(beverage2);
    beverage2 = new Mocha(beverage2);
    beverage2 = new Whip(beverage2);
    cout << beverage2->getDescription() << " $" << beverage2->cost() << endl;

    return 0;
}