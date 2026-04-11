#include "../../all.h"
using namespace std;

class Command {
    public:
        virtual void execute() = 0;
};

class Light {
    public:
        void on() {
            cout << "The light is on." << endl;
        }
        void off() {
            cout << "The light is off." << endl;
        }
};

class LightOnCommand : public Command {
    private:
        Light* light;
    public:
        LightOnCommand(Light* light) {
            this->light = light;
        }
        void execute() {
            light->on();
        }
};

class LightOffCommand : public Command {
    private:
        Light* light;
    public:
        LightOffCommand(Light* light) {
            this->light = light;
        }
        void execute() {
            light->off();
        }
};

class CeilingFan {
    public:
        void on() {
            cout << "The ceiling fan is on." << endl;
        }
        void off() {
            cout << "The ceiling fan is off." << endl;
        }
};

class CeilingFanOnCommand : public Command {
    private:
        CeilingFan* ceilingFan;
    public:
        CeilingFanOnCommand(CeilingFan* ceilingFan) {
            this->ceilingFan = ceilingFan;
        }
        void execute() {
            ceilingFan->on();
        }
};

class CeilingFanOffCommand : public Command {
    private:
        CeilingFan* ceilingFan;
    public:
        CeilingFanOffCommand(CeilingFan* ceilingFan) {
            this->ceilingFan = ceilingFan;
        }
        void execute() {
            ceilingFan->off();
        }
};

class RemoteControl {
    private:
        Command* onCommands[7];
        Command* offCommands[7];
    public:
        RemoteControl() {
            for (int i = 0; i < 7; i++) {
                onCommands[i] = nullptr;
                offCommands[i] = nullptr;
            }
        }
        void setCommand(int slot, Command* onCommand, Command* offCommand) {
            onCommands[slot] = onCommand;
            offCommands[slot] = offCommand;
        }
        void onButtonWasPushed(int slot) {
            if (onCommands[slot] != nullptr) {
                onCommands[slot]->execute();
            }
        }
        void offButtonWasPushed(int slot) {
            if (offCommands[slot] != nullptr) {
                offCommands[slot]->execute();
            }
        }
};

int main() {
    RemoteControl* remote = new RemoteControl();
    Light* livingRoomLight = new Light();
    CeilingFan* ceilingFan = new CeilingFan();
    
    Command* livingRoomLightOn = new LightOnCommand(livingRoomLight);
    Command* livingRoomLightOff = new LightOffCommand(livingRoomLight);
    Command* ceilingFanOn = new CeilingFanOnCommand(ceilingFan);
    Command* ceilingFanOff = new CeilingFanOffCommand(ceilingFan);

    remote->setCommand(0, livingRoomLightOn, livingRoomLightOff);
    remote->setCommand(1, ceilingFanOn, ceilingFanOff);
    
    remote->onButtonWasPushed(0);
    remote->offButtonWasPushed(0);
    remote->onButtonWasPushed(1);
    remote->offButtonWasPushed(1);

    return 0;
}