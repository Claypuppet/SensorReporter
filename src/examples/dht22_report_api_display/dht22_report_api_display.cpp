#include <Arduino.h>
#include <DHP.hpp>

#include "Aggregator.hpp"

#define DHT_PIN 19

Aggregator aggregator;


enum SensorTypes {
  e_my_sensor = 0,
  e_serial_reader,
};

enum ReporterTypes {
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
  MyDHTSensor() : DHT(), Worker<DHTData>(DHTData{0, 0}, 2000) {}

 protected:
  bool activate(bool retry) override {
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
      if(data.temperature != NAN && data.humidity != NAN) {
        return Worker::e_worker_data_read;
      }
    }
    return Worker::e_worker_error;
  }
};

struct SerialCommand {
  enum Type {
    e_invalid = '.',
    e_threshold = 't',
    e_serial_output = 's'
  };

  Type type;
  int val;
};

/**
 * Some sensor that counts upwards every second
 */
class SerialCommandReader : public Worker<SerialCommand> {
 public:

  SerialCommandReader() : Worker<SerialCommand>({SerialCommand::e_invalid, 0}, 0) {}

 protected:
  /**
   * Read serial to set threshold.
   *    Send new threshold with t<number> (like `t22` to set 22)
   *    Send serial output enable with s<0|1> (like `s0` to disable output)
   */
  int8_t produce_data() override {
    if(Serial.available() > 0) {
      char command = Serial.read();
      if(Serial.available() <= 0) return Worker::e_worker_idle;
      auto string = Serial.readString();
      switch(command){
        case SerialCommand::e_threshold:
          data.type = SerialCommand::e_threshold;
          data.val = string.toInt();
          break;
        case SerialCommand::e_serial_output:
          data.type = SerialCommand::e_serial_output;
          data.val = string.toInt() != 0;
          break;
        default:
          return Worker::e_worker_idle;
      }
      if(data.val == 0 && string.indexOf('0') < 0) {
        return Worker::e_worker_error;
      }
      return Worker::e_worker_data_read;
    }
    return Worker::e_worker_idle;
  }
};

/**
 * A data handler that outputs the measurement through serial
 */
class SerialReporter : public Handler {
 public:
  SerialReporter() : Handler() {}

 protected:
  int8_t handle_produced_work(const worker_map_t& workers) override {
    auto dht_measurement = workers.worker<MyDHTSensor>(e_my_sensor);
    if(dht_measurement->is_fresh()) {
      // Retrieve data as reference to avoid calling copy constructor
      const auto& data = dht_measurement->get_data();
      Serial.printf("Reporting data from my sensor: (temp: %f) (hum: %f)\n", data.temperature, data.humidity);
      Serial.flush();
      return e_handler_data_handled;
    }
    return e_handler_idle;
  }
};

/**
 * A data handler that outputs the measurement through serial
 */
class SerialSupervisor : public Supervisor {
 public:
  SerialSupervisor() : Supervisor() {}

  void initialize() override {
    Serial.begin(115200);
  }

  void handle_report(const worker_map_t& workers, const handler_map_t& handlers) override {
    const auto& serial_reader = workers.worker<SerialCommandReader>(e_serial_reader);
    if(serial_reader->is_fresh()) {
      auto& command = serial_reader->get_data();
      switch(command.type){
        case SerialCommand::e_threshold:
          Serial.printf("threshold now: %d\n", command.val);
          break;
        case SerialCommand::e_serial_output:
          aggregator.set_handler_active(e_my_serial_handler, command.val);
          Serial.printf("output now: %s\n", command.val ? "enabled" : "disabled");
          break;
        case SerialCommand::e_invalid:
          break;
      }
    }
    auto& output_handler = handlers.at((e_my_serial_handler));
    if(output_handler->get_status() == Handler::e_handler_data_handled) {
      Serial.printf("output successfully handled the data\n");
    }
  }
};

MyDHTSensor sensor;
SerialCommandReader serial_reader;
SerialReporter handler_s;
SerialSupervisor supervisor;

void setup() {
  Serial.begin(115200);

  aggregator.register_worker(e_my_sensor, sensor);
  aggregator.register_worker(e_serial_reader, serial_reader);
  aggregator.register_handler(e_my_serial_handler, handler_s);
  aggregator.register_supervisor(supervisor);
}

void loop() {
  aggregator.run();
}
