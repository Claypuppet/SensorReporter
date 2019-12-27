//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_SENSOR_HPP_
#define SENSOR_REPORTER_SENSOR_HPP_

#include <Arduino.h>
#include "Activatable.hpp"

/**
 * Base class for the worker
 * To use this, extend the Worker class
 */
class BaseWorker : public Activatable {
 public:
  explicit BaseWorker(bool active) : Activatable(active) {};
  virtual ~BaseWorker() = default;

  /**
   * Get the worker id
   * @return worker id
   */
  virtual uint8_t get_worker_id() const = 0;

  /**
   * Called by the aggregator to get new data. Will fill in the work report depending on its state and produced work
   * @return true if new data was produced.
   */
  virtual bool work(WorkReport& status) = 0;
};

/**
 * A worker will do things to get fresh data in the system. This can be retrieving sensor data, keeping track of
 * interrupts or reading serial data.
 * @tparam T: Type of the data it produces (can be int, string, or some struct type)
 */
template<typename T>
class Worker : public BaseWorker {
 public:
  /**
   *
   * Construct a worker_id
   * @param worker_id : unique id of the worker
   * @param initial_val : initial value of the data it produces
   * @param active : if it should be active from start (default true)
   * @param break_duration : time in millis how long the delay should be between produced work.
   */
  Worker(uint8_t worker_id, T initial_val, bool active = true, uint32_t break_duration = 1000)
      : BaseWorker(active),
        worker_id(worker_id),
        data(initial_val),
        break_duration(break_duration),
        last_break(0) {
  }

  virtual ~Worker() = default;

  /**
   * The main function to implement in sub classes, store produced work in `data` property
   * @return true if the work was success (new data available)
   */
  virtual bool produce_data() = 0;

  /**
   * Called by the aggregator to get new data. Will fill in the work report depending on its state and produced work
   * @return true if new data was produced.
   */
  bool work(WorkReport& status) final {
    if(is_initialized() && is_active()) {
      if((millis() - last_break > break_duration || last_break == 0) && produce_data()) {
        // Work has been produced
        last_break = millis();
        status.state = WorkReport::e_fresh_data;
        status.data = (void*) &data;
        return true;
      }
      // No fresh work produced
      status.state = WorkReport::e_idle;
    }
    else {
      // Worker is not active
      status.state = WorkReport::e_inactive;
    }
    return false;
  }

  /**
   * Get the worker id
   * @return worker id
   */
  uint8_t get_worker_id() const final {
    return worker_id;
  }

  /**
   * Get the current data from the worker
   * @return current data
   */
  const T& get_data() const {
    return data;
  }

 protected:
  uint8_t worker_id;
  T data;

 private:
  uint32_t break_duration;
  uint32_t last_break;
};

#endif //SENSOR_REPORTER_SENSOR_HPP_
