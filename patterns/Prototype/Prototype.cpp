#include "../../all.h"
using namespace std;

class Prototype {
    public:
        virtual Prototype* clone() = 0;
        virtual string getData() = 0;
};

class ConcretePrototype : public Prototype {
    private:
        string data;
    public:
        ConcretePrototype(string data) {
            this->data = data;
        }
        Prototype* clone() {
            return new ConcretePrototype(data);
        }
        string getData() {
            return data;
        }
};

int main() {
    Prototype* prototype = new ConcretePrototype("Hello, world!");
    Prototype* clone = prototype->clone();

    cout << "Original: " << prototype->getData() << endl;
    cout << "Clone: " << clone->getData() << endl;

    return 0;
}