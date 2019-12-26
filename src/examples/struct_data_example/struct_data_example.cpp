#include <Arduino.h>

#include "Aggregator.hpp"

enum SensorTypes {
  e_my_sensor = 0
};

enum ReporterTypes {
  e_my_led_reporter = 0,
  e_my_serial_reporter
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
class MySensor : public DataRetriever<MySensorData> {
 public:
  MySensor() : DataRetriever<MySensorData>(e_my_sensor, MySensorData{0, "hokey pokey"}, 1000) {
  }

 protected:
  /**
   * When measured, increment the data
   * @return true
   */
  bool measure() override {
    ++data.measurement;
    data.set_some_text(data.measurement % 5 ? "hokey pokey" : "ee macarena");
    return true;
  }
};

/**
 * A reporter that outputs the measurements in the form of a LED
 */
class LedReporter : public Reporter {
 public:
  LedReporter() : Reporter(e_my_led_reporter) {}

 protected:
  bool initialize() override {
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    return true;
  }

  int8_t report_measurements(const measurement_map_t& measurements) override {
    auto my_sensor_measurement = measurements.at(e_my_sensor);
    if(my_sensor_measurement.get<MySensorData>().measurement % 2) {
      digitalWrite(BUILTIN_LED, HIGH);
    } else {
      digitalWrite(BUILTIN_LED, LOW);
    }
    return ReporterStatus::e_ok;
  }
};

/**
 * A reporter that outputs the measurement through serial
 */
class SerialReporter : public Reporter {
 public:
  SerialReporter() : Reporter(e_my_serial_reporter) {}

 protected:
  bool initialize() override {
    Serial.begin(115200);
    return true;
  }

  int8_t report_measurements(const measurement_map_t& measurements) override {
    // Retrieve data as reference to avoid calling copy constructor
    auto& my_sensor_data = measurements.at(e_my_sensor).get<MySensorData>();

    Serial.printf("Reporting data from my sensor: %d! (%s)\n", my_sensor_data.measurement, my_sensor_data.some_text);
    Serial.flush();
    return ReporterStatus::e_ok;
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
