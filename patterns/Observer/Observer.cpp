#include "../../all.h"
using namespace std;

class Observer {
    public:
        virtual void update() = 0;
};

class Subject {
    public:
        virtual void registerObserver(Observer* observer) = 0;
        virtual void removeObserver(Observer* observer) = 0;
        virtual void notifyObservers() = 0;
};

class WeatherData : public Subject {
    private:
        vector<Observer*> observers;
        float temperature;
        float humidity;
        float pressure;
    public:
        void registerObserver(Observer* observer) {
            observers.push_back(observer);
        }
        void removeObserver(Observer* observer) {
            observers.erase(remove(observers.begin(), observers.end(), observer), observers.end());
        }
        void notifyObservers() {
            for (Observer* observer : observers) {
                observer->update();
            }
        }
        void measurementsChanged() {
            notifyObservers();
        }
        void setMeasurements(float temp, float humidity, float pressure) {
            this->temperature = temp;
            this->humidity = humidity;
            this->pressure = pressure;
            measurementsChanged();
        }
        float getTemperature() {
            return temperature;
        }
        float getHumidity() {
            return humidity;
        }
        float getPressure() {
            return pressure;
        }
};

class DisplayElement {
    public:
        virtual void display() = 0;
};

class CurrentConditionsDisplay : public Observer, public DisplayElement {
    private:
        float temperature;
        float humidity;
        WeatherData* weatherData;
    public:
        CurrentConditionsDisplay(WeatherData* weatherData) {
            this->weatherData = weatherData;
            weatherData->registerObserver(this);
        }
        void update() {
            temperature = weatherData->getTemperature();
            humidity = weatherData->getHumidity();
            display();
        }
        void display() {
            cout << "Current conditions: " << temperature << "F degrees and " << humidity << "% humidity" << endl;
        }
};

class StatisticsDisplay : public Observer, public DisplayElement {
    private:
        float maxTemp;
        float minTemp;
        float tempSum;
        int numReadings;
        WeatherData* weatherData;
    public:
        StatisticsDisplay(WeatherData* weatherData) {
            this->weatherData = weatherData;
            weatherData->registerObserver(this);
            maxTemp = 0.0f;
            minTemp = 200.0f;
            tempSum = 0.0f;
            numReadings = 0;
        }
        void update() {
            float temp = weatherData->getTemperature();
            tempSum += temp;
            numReadings++;

            if (temp > maxTemp) {
                maxTemp = temp;
            }

            if (temp < minTemp) {
                minTemp = temp;
            }

            display();
        }
        void display() {
            cout << "Avg/Max/Min temperature = " << (tempSum / numReadings) << "/" << maxTemp << "/" << minTemp << endl;
        }
};

int main() {
    WeatherData* weatherData = new WeatherData();
    CurrentConditionsDisplay* currentDisplay = new CurrentConditionsDisplay(weatherData);
    StatisticsDisplay* statisticsDisplay = new StatisticsDisplay(weatherData);

    weatherData->setMeasurements(80, 65, 30.4f);
    weatherData->setMeasurements(82, 70, 29.2f);
    weatherData->setMeasurements(78, 90, 29.2f);

    return 0;
}