//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_REPORTER_HPP_
#define SENSOR_REPORTER_REPORTER_HPP_

#include <stdint.h>
#include "Report.hpp"

/**
 * The reporter reports the measurements to some output.
 */
class Reporter {
 public:

  /**
   * Construct a reporter
   * @param reporter_id: unique id of the reporter
   * @param active: start activated (the activate will be called on initialize)
   */
  explicit Reporter(uint8_t reporter_id, bool active = true);
  virtual ~Reporter() = default;

  /**
   * Gets the reporter id
   * @return reporter id
   */
  virtual uint8_t get_reporter_id() final;

  /**
   * Initializes the reporter if it is active but not initialized.
   * If successfully initialized, sets initialized to true
   */
  virtual void perform_initialize() final;

  /**
   * Call the reporter sequence to report measurements
   * @param measurements: measurements to report
   * @param status: reference to the status to fill of this reporter
   */
  virtual void perform_report(measurement_map_t& measurements, ReporterStatus& status) final;

  /**
   * Activates / deactivates the reporter. If the activate / deactivate fails, status wont change
   * Requires the reporter to be initialized
   * @param active: true to activate, false to deactivate
   */
  virtual void set_active(bool active) final;

 protected:
  /**
   * Initialize the reporter, this will only be called once.
   * Override this to set pin modes, serial begin or other one time settings
   * @return true if initialize was success. if false, reporter will not be initialized (and wont be used)
   */
  virtual bool initialize();

  /**
   * Perform an activate function, will be called when set_active(true)
   * Override this when the reporter will be activated/deactivated in times. The deactivate can clean up.
   * Example, use of wifi or bluetooth connection
   * @return true if activate was success. if false, reporter will not be activated (and wont be used)
   */
  virtual bool activate();

  /**
   * Perform an deactivate function, will be called when set_active(false)
   * Override this to cleanup the activation... shut down connections or something
   */
  virtual void deactivate();

  /**
   * Step 1 of the report process.  Prepare data / reporter
   * This step is optional - Returns OK if not overridden
   * @return status code,  0 for OK (will continue).  Use ReporterStatus::StatusCode or custom (keep 0 OK)
   */
  virtual int8_t pre_report();

  /**
   * Step 2 of the report process.  Report the data
   * @param measurements: All the measurements from the data retrievers
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
  uint8_t reporter_id;
  bool initialized;
  bool active;
};

#endif //SENSOR_REPORTER_REPORTER_HPP_
