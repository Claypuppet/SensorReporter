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
  virtual void try_handle_work(const worker_map_t& workers) final;

 protected:
  /**
   * Handle the data produced by workers
   * @param worker_reports: All the data from the workers
   * @return status code (HandlerStatus::StatusCode or any custom)
   */
  virtual int8_t handle_produced_work(const worker_map_t& workers) = 0;


 private:
  int8_t status;
};

typedef std::map<uint8_t, Handler*> handler_map_t;


#endif //SENSOR_REPORTER_REPORTER_HPP_
