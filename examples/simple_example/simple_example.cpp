#include <Arduino.h>

#include <Aggregator.hpp>

enum SensorTypes {
  e_my_sensor = 0,
  e_echo
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
  MySensor() : Worker<MySensorData>(MySensorData{0, "hokey pokey"}, 1000) {
  }

 protected:
  /**
   * When measured, increment the data
   */
  int8_t produce_data() override {
    ++data.measurement;
    data.set_some_text(data.measurement % 5 ? "hokey pokey" : "ee macarena");
    return e_worker_data_read;
  }
};

/**
 * Some sensor that counts upwards every second
 */
class Echo : public ProcessWorker<int> {
 public:
  Echo() : ProcessWorker<int>(false) {
  }

 protected:
  /**
   * copy measurement from sensor
   */
  int8_t produce_data(const worker_map_t& workers) override {
    const auto& worker = workers.worker<MySensor>(e_my_sensor);
    if (worker->is_fresh()) {
      data = worker->get_data().measurement;
      return e_worker_data_read;
    }
    return e_worker_idle;
  }
};

/**
 * A data handler that outputs the worker_reports in the form of a LED
 */
class LedReporter : public Handler {
 public:
  LedReporter() : Handler() {}

 protected:
  bool activate(bool retry) override {
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, LOW);
    return true;
  }

  int8_t handle_produced_work(const WorkerMap& workers) override {
    auto my_sensor_measurement = workers.worker<MySensor>(e_my_sensor);
    if(my_sensor_measurement && my_sensor_measurement->is_fresh()) {
      if(my_sensor_measurement->get_data().measurement % 2) {
        digitalWrite(BUILTIN_LED, HIGH);
      } else {
        digitalWrite(BUILTIN_LED, LOW);
      }
      return e_handler_data_handled;
    }
    return e_handler_idle;
  }
};

/**
 * A data handler that outputs the measurement through serial
 */
class SerialReporter : public Handler {
 public:
  SerialReporter() : Handler() {}

 protected:
  bool activate(bool retry) override {
    Serial.begin(115200);
    return true;
  }

  int8_t handle_produced_work(const WorkerMap & workers) override {
    const auto& my_sensor_measurement = workers.worker<MySensor>(e_my_sensor);
    const auto& my_echo = workers.worker<Echo>(e_echo);
    if(my_sensor_measurement && my_sensor_measurement->is_fresh()) {
      // Retrieve data as reference to avoid calling copy constructor
      const auto& data = my_sensor_measurement->get_data();
      Serial.printf("Reporting data from my sensor: %d! (%s)\n", data.measurement, data.some_text);
      Serial.printf("(My echo says: %d)\n", my_echo->get_data());
      Serial.flush();
      return e_handler_data_handled;
    }
    return e_handler_idle;
  }
};

Aggregator aggregator;
MySensor sensor;
Echo echo;
LedReporter handler_l;
SerialReporter handler_s;

void setup() {
  // register workers/handlers
  aggregator.register_worker(e_echo, echo);
  aggregator.register_worker(e_my_sensor, sensor);
  aggregator.register_handler(e_my_led_handler, handler_l);
  aggregator.register_handler(e_my_serial_handler, handler_s);

  // Activate workers/handlers
  aggregator.set_worker_active(e_echo, true);
  aggregator.set_worker_active(e_my_sensor, true);
  aggregator.set_handler_active(e_my_led_handler, true);
  aggregator.set_handler_active(e_my_serial_handler, true);
}

void loop() {
  aggregator.run();
}
