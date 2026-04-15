#include "../../all.h"
using namespace std;

class EmployeeDao {
    public:
        virtual void create() = 0;
};

class EmployeeDaoImpl : public EmployeeDao {
    public:
        void create() {
            cout << "Creating employee in database." << endl;
        }
};

class EmployeeDaoProxy : public EmployeeDao {
    private:
        EmployeeDao* employeeDao;
    public:
        EmployeeDaoProxy(EmployeeDao* employeeDao) {
            this->employeeDao = employeeDao;
        }
        void create() {
            cout << "Logging: Creating employee." << endl;
            employeeDao->create();
            cout << "Logging: Employee created." << endl;
        }
};

int main() {
    EmployeeDao* employeeDao = new EmployeeDaoImpl();
    EmployeeDao* proxy = new EmployeeDaoProxy(employeeDao);

    proxy->create();

    return 0;
}