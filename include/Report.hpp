//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_INCLUDE_REPORT_HPP_
#define SENSOR_REPORTER_INCLUDE_REPORT_HPP_

#include <map>

/**
 * A single measurement, keeps track if its fresh data and has method to cast the containing data
 */
struct Measurement {
  bool fresh = false;
  bool active = false;
  void* data = nullptr;

  /**
   * Get the data as type T
   * @tparam T: type of the data
   * @return the data
   */
  template<typename T>
  const T& get() const {
    return *static_cast<T*>(data);
  }
};

typedef std::map<uint8_t, Measurement> measurement_map_t;

/**
 * Status of a single data handler
 */
struct ReporterStatus {
  typedef enum {
    e_inactive = -1,
    e_ok = 0,
    e_error
  } StatusCode;

  int8_t pre = StatusCode::e_inactive;
  int8_t report = StatusCode::e_inactive;
  int8_t post = StatusCode::e_inactive;
};

typedef std::map<uint8_t, ReporterStatus> handler_status_map_t;

/**
 * The report object keeps track of the current reporting being done. It provides the measurements to the data handler,
 * which will then set the data handler status and results in this object. The report handler will get the full object to
 * handle the rest
 */
class Report {
 public:
  Report(measurement_map_t& measurements, handler_status_map_t& handlers)
      : measurements(measurements), handlers(handlers) {}

  virtual ~Report() = default;

  const measurement_map_t& measurements;
  const handler_status_map_t& handlers;
};

#endif //SENSOR_REPORTER_INCLUDE_REPORT_HPP_
