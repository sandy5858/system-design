#include "../../all.h"
using namespace std;

class ChocolateBoiler {
    private:
        bool empty;
        bool boiled;
        ChocolateBoiler() : empty(true), boiled(false) {}
    public:
        static ChocolateBoiler* getInstance() {
            static ChocolateBoiler instance;
            return &instance;
        }
        void fill() {
            if (isEmpty()) {
                empty = false;
                boiled = false;
                cout << "Filling the boiler with a chocolate mixture." << endl;
            }
        }
        void drain() {
            if (!isEmpty() && isBoiled()) {
                empty = true;
                cout << "Draining the boiled chocolate." << endl;
            }
        }
        void boil() {
            if (!isEmpty() && !isBoiled()) {
                boiled = true;
                cout << "Boiling the chocolate mixture." << endl;
            }
        }
        bool isEmpty() {
            return empty;
        }
        bool isBoiled() {
            return boiled;
        }
};

int main() {
    ChocolateBoiler* boiler = ChocolateBoiler::getInstance();
    boiler->fill();
    boiler->boil();
    boiler->drain();

    return 0;
}