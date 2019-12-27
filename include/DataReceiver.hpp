//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_SENSOR_HPP_
#define SENSOR_REPORTER_SENSOR_HPP_

#include <Arduino.h>
#include "Activatable.hpp"

/**
 * Base class for the data receiver
 * Each receiver needs an unique ID, best defined with an enum or similar
 * To use this, extend the DataReceiver class
 */
class BaseDataReceiver : public Activatable {
 public:
  explicit BaseDataReceiver(bool active) : Activatable(active) {};
  virtual ~BaseDataReceiver() = default;

  /**
   * Get the retriever id
   * @return retriever id
   */
  virtual uint8_t get_receiver_id() const = 0;

  /**
   * Get pointer to last data measured
   * @return void pointer to data last measured
   */
  virtual void* get_last() = 0;

  /**
   * Retrieve new data, this is called by the aggregator and handled in the DataReceiver class
   * @return true if new data has been recorded
   */
  virtual bool try_receive() = 0;
};

/**
 * The DataReceiver should be extended by sensors and such, so they can be registered by the aggregator.
 * @tparam T: Type of the data object (can be int, string, or some struct type)
 */
template<typename T>
class DataReceiver : public BaseDataReceiver {
 public:
  /**
   * Construct a data retriever
   * @param check_delay : it will wait x milliseconds after successful measurement before getting a new one
   */
  DataReceiver(uint8_t receiver_id, T initial_val, bool active = true, uint32_t check_delay = 1000)
      : BaseDataReceiver(active),
        receiver_id(receiver_id),
        data(initial_val),
        check_delay(check_delay),
        last_check_time(0) {
  }

  virtual ~DataReceiver() = default;

  /**
   * The main function of the class, will do the measurement and store it in `data`
   * @return true if the measurement was success (new data available)
   */
  virtual bool receive_data() = 0;

  /**
   * Called by the aggregator to retrieve new data. If new data is read, it will be stored in `last_data` to
   * be referenced to by the aggregator measurement data.
   * @return true if new data was read.
   */
  bool try_receive() final {
    if(is_initialized()
        && is_active()
        && (millis() - last_check_time > 1000 || last_check_time == 0)
        && receive_data()) {

      last_check_time = millis();
      return true;
    }
    return false;
  }

  /**
   * Get the retriever id
   * @return retriever id
   */
  uint8_t get_receiver_id() const final {
    return receiver_id;
  }

  /**
   * Get the current data object from the retriever
   * @return current data
   */
  const T& get() const {
    return data;
  }

  /**
   * Get a pointer to the last data object of the retriever
   * @return void pointer to the last data
   */
  void* get_last() final {
    return reinterpret_cast<void*>(&data);
  }

 protected:
  uint8_t receiver_id;
  T data;

 private:
  uint32_t check_delay;
  uint32_t last_check_time;
};

#endif //SENSOR_REPORTER_SENSOR_HPP_
