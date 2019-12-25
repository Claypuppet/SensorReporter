//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_REPORTER_HPP_
#define SENSOR_REPORTER_REPORTER_HPP_

#include <stdint.h>
#include "Report.hpp"

/**
 * The reporter reports the measurements.
 * TODO: stuff
 */
class Reporter {
 public:
  explicit Reporter(uint8_t reporter_id);
  virtual ~Reporter() = default;

  virtual uint8_t get_reporter_id() final;

  virtual bool initialize();
  virtual bool activate();
  virtual bool deactivate();

  virtual void perform_report(Report& report) final;

  virtual bool pre_report();
  virtual bool report_measurements(const measurement_list_t& measurements);
  virtual bool post_report();

 private:
  uint8_t reporter_id;
};

#endif //SENSOR_REPORTER_REPORTER_HPP_
