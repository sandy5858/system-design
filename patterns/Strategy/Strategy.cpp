#include "../../all.h"
using namespace std;
class FlyBehavior {
    public:
        virtual void fly() = 0;
};

class QuackBehavior {
    public:
        virtual void quack() = 0;
};

class FlyWithWings : public FlyBehavior {
    public:
        void fly() {
            cout << "I'm flying with wings!" << endl;
        }
};

class Squeak : public QuackBehavior {
    public:
        void quack() {
            cout << "Squeak!" << endl;
        }
};

class FlyNoWay : public FlyBehavior {
    public:
        void fly() {
            cout << "I can't fly!" << endl;
        }
};

class MuteQuack : public QuackBehavior {
    public:
        void quack() {
            cout << "Cannot quack!" << endl;
        }
};

class Duck {
    protected:
        FlyBehavior* flyBehavior;
        QuackBehavior* quackBehavior;
    public:
        void performQuack(){
            quackBehavior->quack();
        }
        void performFly(){
            flyBehavior->fly();
        }
        virtual void display() = 0;
};

class MallardDuck : public Duck {
    public:
        MallardDuck(){
            flyBehavior = new FlyWithWings();
            quackBehavior = new Squeak();
        }
        void display() {
            cout << "I'm a real Mallard duck!" << endl;
        }
};

class ModelDuck : public Duck {
    public:
        ModelDuck(){
            flyBehavior = new FlyNoWay();
            quackBehavior = new MuteQuack();
        }
        void display() {
            cout << "I'm a model duck!" << endl;
        }
};

int main() {
    Duck* mallard = new MallardDuck();
    mallard->performFly();
    mallard->performQuack();
    mallard->display();

    Duck* model = new ModelDuck();
    model->performFly();
    model->performQuack();
    model->display();

    return 0;
}