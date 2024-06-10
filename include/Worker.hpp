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

template<typename T>
class Worker;


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
    e_worker_data_working, // For async
    e_worker_error,
  } Status;

  explicit BaseWorker(uint32_t break_duration = 0);
  virtual ~BaseWorker() = default;

  explicit BaseWorker(BaseWorker& copy) = delete;


  /**
   * Get status
   * @return
   */
  int8_t get_status() const;

  /**
   * Get status
   * @return
   */
  uint32_t get_last_produce() const;

  /**
   * Checks if fresh data is read
   * @return
   */
  bool is_fresh() const;

 protected:
  /**
   * The main function to implement in sub classes, store produced work in `data` property
   * Used by normal workers
   * @return status code (BaseWorker::Status or any custom)
   */
  virtual int8_t produce_data() = 0;

  /**
   * The main function to implement in sub classes, store produced work in `data` property
   * Used by process workers
   * @return status code (BaseWorker::Status or any custom)
   */
  virtual int8_t produce_data(const worker_map_t& workers) = 0;

  /**
   * Function that is run async in a separate task, do NOT access worker.data from this
   * prepare data inside the instance, to be copied afterwards in the main loop by implementing `finish_produced_data`
   * @return status code (BaseWorker::Status or any custom)
   */
  virtual int8_t produce_async_data();

  /**
   * Implement this function to finalize async results to the data to be used in the rest of the system.
   * This function is called by the main thread after the task to produce async data was finished.
   */
  virtual void finish_produced_data();

  virtual bool is_process_worker() const = 0;

  int8_t start_task(const char* task_name, uint8_t core=0, uint8_t priority=5);
  void kill_task();

  virtual bool task_running() const;

 private:

  /**
   * Called by the aggregator to get new data. Will fill in the work report depending on its state and produced work
   * @return true if new data was produced.
   */
  bool work(const worker_map_t& workers);

  uint32_t break_duration;
  uint32_t last_produce;
  int8_t status;
  int8_t async_result_status;

 private:

  static void run_task(void* instance);

  TaskHandle_t xAsyncWorkerHandle;

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

  virtual ~Worker() {
  };

  /**
   * Get the current data from the worker
   * @return current data
   */
  const T& get_data() {
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

  bool any_updates() const;
};

#endif //SENSOR_REPORTER_SENSOR_HPP_
