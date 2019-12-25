#include <Arduino.h>

#include "Aggregator.hpp"

enum SensorTypes {
  e_my_sensor = 0
};

enum ReporterTypes {
  e_my_led_reporter = 0,
  e_my_serial_reporter
};

/**
 * Some sensor that counts upwards every second
 */
class MySensor : public DataRetriever<int> {
 public:
  MySensor() : DataRetriever<int>(e_my_sensor, 0, 1000) {
  }

  /**
   * When measured, increment the data
   * @return true
   */
  bool measure() override {
    ++data;
    return true;
  }
};

/**
 * A reporter that outputs the measurements in the form of a LED
 */
class LedReporter : public Reporter {
 public:
  LedReporter() : Reporter(e_my_led_reporter) {}

  bool initialize() override {
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    return true;
  }

  bool report_measurements(const measurement_list_t& measurements) override {
    auto my_sensor_measurement = measurements.at(e_my_sensor);
    if(my_sensor_measurement.get<int>() % 2) {
      digitalWrite(BUILTIN_LED, HIGH);
    } else {
      digitalWrite(BUILTIN_LED, LOW);
    }
    return true;
  }
};


/**
 * A reporter that outputs the measurement through serial
 */
class SerialReporter : public Reporter {
 public:
  SerialReporter() : Reporter(e_my_serial_reporter) {}

  bool initialize() override {
    Serial.begin(115200);
    return true;
  }

  bool report_measurements(const measurement_list_t& measurements) override {
    int my_sensor_data = measurements.at(e_my_sensor).get<int>();
    Serial.printf("Reporting data from my sensor: %d!\n", my_sensor_data);
    Serial.flush();
    return true;
  }
};

Aggregator a;
MySensor sensor;
LedReporter reporter_l;
SerialReporter reporter_s;

void setup() {
  a.register_retriever(sensor);
  a.register_reporter(reporter_l);
  a.register_reporter(reporter_s);

  a.initialize_all();
}

void loop() {
  a.run();
}
