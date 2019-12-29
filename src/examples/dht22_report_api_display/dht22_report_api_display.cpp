#include <Arduino.h>
#include <DHT.h>

#include "Aggregator.hpp"

#define DHT_PIN 19

enum SensorTypes {
  e_my_sensor = 0,
  e_threshold_reader,
};

enum ReporterTypes {
  e_my_led_handler = 0,
  e_my_serial_handler
};

struct DHTData {
  float temperature;
  float humidity;
};

/**
 * Some sensor that counts upwards every second
 */
class MyDHTSensor : public DHT, public Worker<DHTData> {
 public:
  MyDHTSensor() : DHT(), Worker<DHTData>(e_my_sensor, DHTData{0, 0}, 2000) {}

 protected:
  bool activate() override {
    setup(DHT_PIN, DHT_MODEL_t::DHT22);
    return true;
  }

 protected:
  /**
   * Read DHT data
   */
  int8_t produce_data() override {
    if(getStatus() == DHT_ERROR_t::ERROR_NONE) {
      data.temperature = getTemperature();
      data.humidity = getHumidity();
      Serial.println("trying to get dht data");
      if(data.temperature != NAN && data.humidity != NAN) {
        return WorkerStatus::e_worker_data_read;
      }
    }
    return WorkerStatus::e_worker_error;
  }
};

/**
 * Some sensor that counts upwards every second
 */
class MyThresholdSetter : public Worker<int> {
 public:
  MyThresholdSetter() : Worker<int>(e_threshold_reader, 0, 0) {}

 protected:
  bool activate() override {
    Serial.begin(115200);
    while(Serial.available() > 0) {
      Serial.read();
    }
    return true;
  }

 protected:
  /**
   * Read serial to set threshold.  Send command with t<number> (like `t22` to set 22)
   */
  int8_t produce_data() override {
    if(Serial.available() > 0) {
      auto string = Serial.readString();
      data = string.toInt();
      Serial.printf("Read this: %s\n", string.c_str());
      if(data == 0 && string.indexOf('0') < 0) {
        return WorkerStatus::e_worker_error;
      }
      return WorkerStatus::e_worker_data_read;
    }
    return WorkerStatus::e_worker_idle;
  }
};

/**
 * A data handler that outputs the worker_reports in the form of a LED
 */
class LedReporter : public Handler {
 public:
  LedReporter() : Handler(e_my_led_handler), threshold(25) {}

 protected:
  bool activate() override {
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    return true;
  }

  int8_t handle_produced_work(const worker_status_t& worker_stats) override {
    auto threshold_receiver = worker_stats.at(e_threshold_reader);
    if(threshold_receiver.status == WorkerStatus::e_worker_data_read) {
      threshold = threshold_receiver.get<int>();
    }
    auto my_sensor_measurement = worker_stats.at(e_my_sensor);
    if(my_sensor_measurement.status == WorkerStatus::e_worker_data_read) {
      if(my_sensor_measurement.get<DHTData>().temperature > threshold) {
        digitalWrite(BUILTIN_LED, HIGH);
      } else {
        digitalWrite(BUILTIN_LED, LOW);
      }
      return HandlerStatus::e_handler_data_handled;
    }
    return HandlerStatus::e_handler_idle;
  }

  int threshold;
};

/**
 * A data handler that outputs the measurement through serial
 */
class SerialReporter : public Handler {
 public:
  SerialReporter() : Handler(e_my_serial_handler) {}

 protected:
  bool activate() override {
    Serial.begin(115200);
    return true;
  }

  int8_t handle_produced_work(const worker_status_t& worker_stats) override {
    auto my_sensor_measurement = worker_stats.at(e_my_sensor);
    if(my_sensor_measurement.status == WorkerStatus::e_worker_data_read) {
      // Retrieve data as reference to avoid calling copy constructor
      const auto& data = my_sensor_measurement.get<DHTData>();
      Serial.printf("Reporting data from my sensor: (temp: %f) (hum: %f)\n", data.temperature, data.humidity);
      Serial.flush();
      return HandlerStatus::e_handler_data_handled;
    }
    return HandlerStatus::e_handler_idle;
  }
};

/**
 * A data handler that outputs the measurement through serial
 */
class SerialSupervisor : public Supervisor {
 public:
  SerialSupervisor() : Supervisor() {}

 protected:
  bool activate() override {
    Serial.begin(115200);
    return true;
  }

 public:
  void handle_report(const Report& report) override {
    auto& worker_stats = report.get_worker_stats();
    auto& handler_stats = report.get_handler_stats();
    auto& threshold_setter = worker_stats.at(e_threshold_reader);
    if(threshold_setter.is_fresh()) {
      Serial.printf(
          "threshold now: %f\n", threshold_setter.get<float>());
    }
  }
};

Aggregator a;
MyDHTSensor sensor;
MyThresholdSetter serial_reader;
LedReporter handler_l;
SerialReporter handler_s;
SerialSupervisor supervisor;

void setup() {
  a.register_worker(sensor);
  a.register_worker(serial_reader);
  a.register_handler(handler_l);
  a.register_handler(handler_s);
  a.register_supervisor(supervisor);
}

void loop() {
  a.run();
}
