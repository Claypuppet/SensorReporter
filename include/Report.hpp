//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_INCLUDE_REPORT_HPP_
#define SENSOR_REPORTER_INCLUDE_REPORT_HPP_

#include "Measurement.hpp"
#include <map>

/**
 * The report object keeps track of the current reporting being done. It provides the measurements to the reporter,
 * which will then set the reporter status and results in this object. The report handler will get the full object to
 * handle the rest
 */
class Report {
 public:
  Report(std::map<uint8_t, Measurement>& measurements) : measurements(measurements) {}
  virtual ~Report() = default;

  const measurement_list_t& measurements;
};

#endif //SENSOR_REPORTER_INCLUDE_REPORT_HPP_
