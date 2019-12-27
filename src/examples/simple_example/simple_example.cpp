#include <Arduino.h>

#include "Aggregator.hpp"

enum SensorTypes {
  e_my_sensor = 0
};

enum ReporterTypes {
  e_my_led_handler = 0,
  e_my_serial_handler
};

/**
 * Some sensor that counts upwards every second
 */
class MySensor : public DataReceiver<int> {
 public:
  MySensor() : DataReceiver<int>(e_my_sensor, 0, true, 1000) {
  }

  /**
   * When measured, increment the data
   * @return true
   */
  bool receive_data() override {
    ++data;
    return true;
  }
};

/**
 * A data handler that outputs the measurements in the form of a LED
 */
class LedReporter : public DataObserver {
 public:
  LedReporter() : DataObserver(e_my_led_handler) {}

  bool initialize() override {
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    return true;
  }

  int8_t report_measurements(const measurement_map_t& measurements) override {
    auto my_sensor_measurement = measurements.at(e_my_sensor);
    if(my_sensor_measurement.get<int>() % 2) {
      digitalWrite(BUILTIN_LED, HIGH);
    } else {
      digitalWrite(BUILTIN_LED, LOW);
    }
    return ReporterStatus::e_ok;
  }
};

/**
 * A data handler that outputs the measurement through serial
 */
class SerialReporter : public DataObserver {
 public:
  SerialReporter() : DataObserver(e_my_serial_handler) {}

  bool initialize() override {
    Serial.begin(115200);
    return true;
  }

  int8_t report_measurements(const measurement_map_t& measurements) override {
    auto my_sensor_data = measurements.at(e_my_sensor).get<int>();

    Serial.printf("Reporting data from my sensor: %d!\n", my_sensor_data);
    Serial.flush();
    return ReporterStatus::e_ok;
  }
};

Aggregator a;
MySensor sensor;
LedReporter handler_l;
SerialReporter handler_s;

void setup() {
  a.register_receiver(sensor);
  a.register_data_observer(handler_l);
  a.register_data_observer(handler_s);

  a.initialize_all();
}

void loop() {
  a.run();
}
