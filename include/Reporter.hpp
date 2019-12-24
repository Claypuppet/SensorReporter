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
  Reporter() = default;
  virtual ~Reporter() = default;

  virtual uint32_t get_reporter_id() const = 0;

  virtual bool initialize();
  virtual bool activate();
  virtual bool deactivate();

  virtual void perform_report(Report& report) final;

  virtual bool pre_report();
  virtual bool report_measurements(const measurement_list_t& measurements);
  virtual bool post_report();
};

#endif //SENSOR_REPORTER_REPORTER_HPP_
