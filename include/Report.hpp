//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_INCLUDE_REPORT_HPP_
#define SENSOR_REPORTER_INCLUDE_REPORT_HPP_

#include <map>

/**
 * The produced work of a worker
 */
struct WorkReport {
  /// Enum can be replaced with custom implementation
  typedef enum {
    e_inactive = -1,
    e_idle = 0,
    e_fresh_data,
    e_error
  } StatusCode;

  uint8_t state = e_inactive;
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

typedef std::map<uint8_t, WorkReport> worker_report_map_t;

/**
 * The results of the handler handling work reports
 */
struct HandlerReport {
  /// Enum can be replaced with custom implementation
  typedef enum {
    e_inactive = -1,
    e_ok = 0,
    e_error
  } StatusCode;

  int8_t state = StatusCode::e_inactive;
};

typedef std::map<uint8_t, HandlerReport> handler_reports_map_t;

/**
 * The report object keeps track of the activity of the workers and handlers. It provides the work reports to the
 * handlers, which will then fill in the handler report. The supervisor will get the full report to handle the rest.
 */
class Report {
 public:
  Report(worker_report_map_t& measurements, handler_reports_map_t& handlers)
      : worker_reports(measurements), handler_reports(handlers) {}

  virtual ~Report() = default;

  const worker_report_map_t& worker_reports;
  const handler_reports_map_t& handler_reports;
};

#endif //SENSOR_REPORTER_INCLUDE_REPORT_HPP_
