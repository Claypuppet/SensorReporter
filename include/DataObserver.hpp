//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_REPORTER_HPP_
#define SENSOR_REPORTER_REPORTER_HPP_

#include "Report.hpp"
#include "Activatable.hpp"

/**
 * The data observer can report the measurements to some output.
 */
class DataObserver : public Activatable {
 public:
  /**
   * Construct a data observer
   * @param handler_id: unique id of the data handler
   * @param active: start activated (the activate will be called on initialize)
   */
  explicit DataObserver(uint8_t handler_id, bool active = true);
  virtual ~DataObserver() = default;

  /**
   * Gets the data handler id
   * @return data handler id
   */
  virtual uint8_t get_observer_id() final;

  /**
   * Call the data handler sequence to report measurements
   * @param measurements: measurements to report
   * @param status: reference to the status to fill of this data handler
   */
  virtual void perform_report(measurement_map_t& measurements, ReporterStatus& status) final;

 protected:
  /**
   * Step 1 of the report process.  Prepare data / data handler
   * This step is optional - Returns OK if not overridden
   * @return status code,  0 for OK (will continue).  Use ReporterStatus::StatusCode or custom (keep 0 OK)
   */
  virtual int8_t pre_report();

  /**
   * Step 2 of the report process.  Report the data
   * @param measurements: All the measurements from the data receivers
   * @return status code,  0 for OK (will continue).  Use ReporterStatus::StatusCode or custom (keep 0 OK)
   */
  virtual int8_t report_measurements(const measurement_map_t& measurements) = 0;

  /**
   * Step 3 of the report process.  Cleanup current reporting
   * This step is optional - Returns OK if not overridden
   * @return status code,  0 for OK  Use ReporterStatus::StatusCode or custom (keep 0 OK)
   */
  virtual int8_t post_report();

 private:
  uint8_t handler_id;
};

#endif //SENSOR_REPORTER_REPORTER_HPP_
