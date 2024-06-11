//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_REPORTER_HPP_
#define SENSOR_REPORTER_REPORTER_HPP_

#include "Activatable.hpp"
#include "Worker.hpp"
#include <Arduino.h>
#include <map>

class Aggregator;

/**
 * The handler can report the produced work to some desired output or handle it internally.
 */
class Handler : public Activatable {
 public:

  /**
   * Enum can be replaced with custom implementation
   * async running and idle states are used by the handler itself (at -2 and -1 respectively)
   * Data handled = 0, considered success
   * Anything above 0 can be used for custom errors
   */
  typedef enum Status {
    e_handler_processing = -2, // For async
    e_handler_idle = -1,
    e_handler_data_handled = 0,
    e_handler_error,
  } Status;

  /**
   * Construct a handler
   */
  explicit Handler();
  virtual ~Handler() = default;

  /**
   * get current status
   * @return  status code (HandlerStatus::StatusCode or any custom)
   */
  int8_t get_status() const;

 protected:
  /**
   * Handle the data produced by workers
   * @param workers: All the data from the workers
   * @return status code (HandlerStatus::StatusCode or any custom)
   */
  virtual int8_t handle_produced_work(const WorkerMap& workers) = 0;

  /**
   * Handle data async, prepare your data internally in the handler
   * @return status code (HandlerStatus::StatusCode or any custom)
   */
  virtual int8_t handle_async();

  int8_t status;
  int8_t async_result_status;

 protected:
  /**
   * Start task running async to perform the data handling (prepare data beforehand in handler instance)
   * @param task_name
   * @param core
   * @param priority
   * @return
   */
  int8_t start_task(const char* task_name, uint32_t memory=1024, uint8_t priority=5, uint8_t core=0);

  /**
   * Kill a running task
   */
  void kill_task();

  virtual bool task_running() const;

 private:

  /**
   * Call the data handler sequence to report worker_reports
   * @param work_reports: worker reports to handle
   * @param status: reference to the status to fill of this data handler
   */
  virtual void try_handle_work(const WorkerMap& workers) final;

  static void run_task(void* instance);
  TaskHandle_t xAsyncHandlerHandle;

  friend Aggregator;
};


class HandlerMap : public std::map<uint8_t, Handler*> {
 public:
  template<typename T, typename std::enable_if<std::is_base_of<Handler, T>::value>::type* = nullptr>
  T* handler(uint8_t idx) const {
    return (T*) at(idx);
  }

  bool any_updates() const {
    return std::any_of(
        begin(),
        end(),
        [](const std::pair<uint8_t, Handler*>& pair){return pair.second->get_status() != Handler::e_handler_idle;}
    );
  }
};

typedef HandlerMap handler_map_t;


#endif //SENSOR_REPORTER_REPORTER_HPP_
