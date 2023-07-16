//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_REPORTER_HPP_
#define SENSOR_REPORTER_REPORTER_HPP_

#include "Activatable.hpp"
#include "Worker.hpp"
#include <Arduino.h>
#include <map>

/**
 * The handler can report the produced work to some desired output or handle it internally.
 */
class Handler : public Activatable {
 public:

  /// Enum can be replaced with custom implementation, or use custom enum starting at `= 1`
  typedef enum Status {
    e_handler_idle = -1,
    e_handler_data_handled,
    e_handler_error,
  } Status;

  /**
   * Construct a handler
   * @param handler_id: unique id of the handler
   */
  explicit Handler();
  virtual ~Handler() = default;

  /**
   * Call the data handler sequence to report worker_reports
   * @param work_reports: worker reports to handle
   * @param status: reference to the status to fill of this data handler
   */
  virtual void try_handle_work(const WorkerMap& workers) final;

  /**
   * get current status
   * @return  status code (HandlerStatus::StatusCode or any custom)
   */
  int8_t get_status() const;

 protected:
  /**
   * Handle the data produced by workers
   * @param worker_reports: All the data from the workers
   * @return status code (HandlerStatus::StatusCode or any custom)
   */
  virtual int8_t handle_produced_work(const WorkerMap& workers) = 0;


 protected:
  int8_t status;
};

class HandlerMap : public std::map<uint8_t, Handler*> {
 public:
  template<typename T, typename std::enable_if<std::is_base_of<Handler, T>::value>::type* = nullptr>
  T* handler(uint8_t idx) const {
    return (T*) at(idx);
  }
};

typedef HandlerMap handler_map_t;


#endif //SENSOR_REPORTER_REPORTER_HPP_
