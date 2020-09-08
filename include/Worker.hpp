//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_SENSOR_HPP_
#define SENSOR_REPORTER_SENSOR_HPP_

#include <Arduino.h>
#include <map>
#include "Activatable.hpp"

/**
 * Base class for the worker
 * To use this, extend the Worker class
 */
class BaseWorker : public Activatable {
 public:

  /// Enum can be replaced with custom implementation, or use custom enum starting at `= 1`
  typedef enum Status {
    e_worker_idle = -1,
    e_worker_data_read,
    e_worker_error,
  } Status;

  int8_t status = Status::e_worker_idle;

  bool is_fresh() const { return active_state == e_state_active && status == e_worker_data_read; }

  explicit BaseWorker() : Activatable() {};
  virtual ~BaseWorker() = default;

  /**
   * Called by the aggregator to get new data. Will fill in the work report depending on its state and produced work
   * @return true if new data was produced.
   */
  virtual bool work() = 0;
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
   * @param break_duration : time in millis how long the delay should be between produced work.
   */
  explicit Worker(T initial_val, uint32_t break_duration = 1000)
      : BaseWorker(), data(initial_val), break_duration(break_duration), last_break(0) {
  }

  virtual ~Worker() = default;

  /**
   * The main function to implement in sub classes, store produced work in `data` property
   * @return true if the work was success (new data available)
   */
  virtual int8_t produce_data() = 0;

  /**
   * Called by the aggregator to get new data. Will fill in the work report depending on its state and produced work
   * @return true if new data was produced.
   */
  bool work() final {
    if(active_state == e_state_activating_failed) {
      // Still activating, will try to activate again
      if(activate(true)) {
        active_state = e_state_active;
      } else {
        return false;
      }
    }
    if(active()) {
      if((millis() - last_break > break_duration || last_break == 0)) {
        status = produce_data();
        if(is_fresh()) {
          // Work has been produced
          last_break = millis();
          return true;
        }
      }
      status = e_worker_idle;
    }
    return false;
  }

  /**
   * Get the current data from the worker
   * @return current data
   */
  const T& get_data() const {
    return data;
  }

 protected:
  T data;

 private:
  uint32_t break_duration;
  uint32_t last_break;
};

typedef std::map<uint8_t, BaseWorker*> worker_map_t;

#endif //SENSOR_REPORTER_SENSOR_HPP_
