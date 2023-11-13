//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_SENSOR_HPP_
#define SENSOR_REPORTER_SENSOR_HPP_

#include <Arduino.h>
#include <map>
#include "Activatable.hpp"

class Aggregator;
class WorkerMap;

typedef WorkerMap worker_map_t;

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

  explicit BaseWorker(uint32_t break_duration = 0)
      : Activatable(), break_duration(break_duration), last_produce(0), status(Status::e_worker_idle) {};
  virtual ~BaseWorker() = default;

  /**
   * Get status
   * @return
   */
  int8_t get_status() const {
    return status;
  }

  /**
   * Get status
   * @return
   */
  uint32_t get_last_produce() const {
    return last_produce;
  }

  /**
   * Checks if fresh data is read
   * @return
   */
  bool is_fresh() const { return get_active_state() == e_state_active && status == e_worker_data_read; }

 protected:
  /**
   * The main function to implement in sub classes, store produced work in `data` property
   * @return status code (BaseWorker::Status or any custom)
   */
  virtual int8_t produce_data() = 0;

  /**
   * The main function to implement in sub classes, store produced work in `data` property
   * @return status code (BaseWorker::Status or any custom)
   */
  virtual int8_t produce_data(const worker_map_t& workers) = 0;
  virtual bool is_process_worker() const = 0;

 private:

  /**
   * Called by the aggregator to get new data. Will fill in the work report depending on its state and produced work
   * @return true if new data was produced.
   */
  virtual bool work(const worker_map_t& workers) final {
    if (get_active_state() == e_state_activating_failed) {
      // Still activating, will try to activate again
      if (activate(true)) {
        active_state = e_state_active;
      }
    }
    if (active()) {
      if ((millis() - last_produce > break_duration || last_produce == 0)) {
        status = is_process_worker() ? produce_data(workers) : produce_data();
        if (is_fresh()) {
          // Work has been produced
          last_produce = millis();
          return true;
        }
      }
      else {
        status = e_worker_idle;
      }
    }
    return false;
  }

  uint32_t break_duration;
  uint32_t last_produce;
  int8_t status;

  friend Aggregator;
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
   * Construct a worker, calls default constructor for data
   * @param worker_id : unique id of the worker
   * @param break_duration : time in millis how long the delay should be between produced work, default 0.
   */
  explicit Worker(uint32_t break_duration = 0)
      : BaseWorker(break_duration), data() {
  }

  /**
   * Construct a worker with initial data T
   * @param worker_id : unique id of the worker
   * @param initial_val : initial value of the data it produces
   * @param break_duration : time in millis how long the delay should be between produced work, default 0.
   */
  explicit Worker(T initial_val, uint32_t break_duration = 0)
      : BaseWorker(break_duration), data(initial_val) {
  }

  virtual ~Worker() = default;

  /**
   * Get the current data from the worker
   * @return current data
   */
  const T& get_data() const {
    return data;
  }

 protected:
  bool is_process_worker() const override {
    return false;
  }

  T data;

 private:
  virtual int8_t produce_data(const worker_map_t& workers) {
    // Process worker will be called with produce_data(workers)
    return e_worker_idle;
  };
};

/**
 * A worker will do things to get fresh data in the system. This can be retrieving sensor data, keeping track of
 * interrupts or reading serial data.
 * @tparam T: Type of the data it produces (can be int, string, or some struct type)
 */
template<typename T>
class ProcessWorker : public Worker<T> {
 public:
  /**
   *
   * Construct a worker, calls default constructor for data
   * @param worker_id : unique id of the worker
   * @param break_duration : time in millis how long the delay should be between produced work, default 0.
   */
  explicit ProcessWorker(uint32_t break_duration = 0)
      : Worker<T>(break_duration) {
  }

  /**
   * Construct a worker with initial data T
   * @param worker_id : unique id of the worker
   * @param initial_val : initial value of the data it produces
   * @param break_duration : time in millis how long the delay should be between produced work, default 0.
   */
  explicit ProcessWorker(T initial_val, uint32_t break_duration = 0)
      : Worker<T>(initial_val, break_duration) {
  }

  virtual ~ProcessWorker() = default;

 protected:
  bool is_process_worker() const override {
    return true;
  }

  virtual int8_t produce_data(const WorkerMap& workers) = 0;

 private:
  virtual int8_t produce_data() final {
    // Process worker will be called with produce_data(workers)
    return BaseWorker::e_worker_idle;
  };

  friend Aggregator;
};

class WorkerMap : public std::map<uint8_t, BaseWorker*> {
 public:

  template<typename T, typename std::enable_if<std::is_base_of<BaseWorker, T>::value>::type* = nullptr>
  T* worker(uint8_t idx) const {
    return (T*) at(idx);
  }

  bool any_updates() const {
    return std::any_of(
        begin(),
        end(),
        [](const std::pair<uint8_t, BaseWorker*>& pair) {
          return pair.second->get_status() != BaseWorker::e_worker_idle;
        }
    );
  }
};

#endif //SENSOR_REPORTER_SENSOR_HPP_
