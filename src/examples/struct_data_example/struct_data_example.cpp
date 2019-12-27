#include <Arduino.h>

#include "Aggregator.hpp"

enum SensorTypes {
  e_my_sensor = 0
};

enum ReporterTypes {
  e_my_led_handler = 0,
  e_my_serial_handler
};

struct MySensorData {
  int measurement;
  char some_text[50];

  void set_some_text(const char* text) {
    if(strlen(text) < 50) {
      strcpy(some_text, text);
    }
  }
};

/**
 * Some sensor that counts upwards every second
 */
class MySensor : public Worker<MySensorData> {
 public:
  MySensor() : Worker<MySensorData>(e_my_sensor, MySensorData{0, "hokey pokey"}, true, 1000) {
  }

 protected:
  /**
   * When measured, increment the data
   * @return true
   */
  bool produce_data() override {
    ++data.measurement;
    data.set_some_text(data.measurement % 5 ? "hokey pokey" : "ee macarena");
    return true;
  }
};

/**
 * A data handler that outputs the worker_reports in the form of a LED
 */
class LedReporter : public Handler {
 public:
  LedReporter() : Handler(e_my_led_handler) {}

 protected:
  bool initialize() override {
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    return true;
  }

  int8_t handle_produced_work(const worker_report_map_t& measurements) override {
    auto my_sensor_measurement = measurements.at(e_my_sensor);
    if(my_sensor_measurement.state == WorkReport::StatusCode::e_fresh_data) {
      if(my_sensor_measurement.get<MySensorData>().measurement % 2) {
        digitalWrite(BUILTIN_LED, HIGH);
      } else {
        digitalWrite(BUILTIN_LED, LOW);
      }
    }
    return HandlerReport::e_ok;
  }
};

/**
 * A data handler that outputs the measurement through serial
 */
class SerialReporter : public Handler {
 public:
  SerialReporter() : Handler(e_my_serial_handler) {}

 protected:
  bool initialize() override {
    Serial.begin(115200);
    return true;
  }

  int8_t handle_produced_work(const worker_report_map_t& measurements) override {
    auto my_sensor_measurement = measurements.at(e_my_sensor);
    if(my_sensor_measurement.state == WorkReport::StatusCode::e_fresh_data) {
      // Retrieve data as reference to avoid calling copy constructor
      const auto& data = my_sensor_measurement.get<MySensorData>();
      Serial.printf("Reporting data from my sensor: %d! (%s)\n", data.measurement, data.some_text);
      Serial.flush();
    }
    return HandlerReport::e_ok;
  }
};

Aggregator a;
MySensor sensor;
LedReporter handler_l;
SerialReporter handler_s;

void setup() {
  a.register_worker(sensor);
  a.register_handler(handler_l);
  a.register_handler(handler_s);

  a.initialize_all();
}

void loop() {
  a.run();
}
