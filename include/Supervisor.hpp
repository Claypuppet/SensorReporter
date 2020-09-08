//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_REPORTHANDLER_HPP_
#define SENSOR_REPORTER_REPORTHANDLER_HPP_

/**
 * Supervisor will oversee the full report. Mainly used for outputting all possible values (for example, put results on
 * a screen or control an LED).
 */
class Supervisor {
 public:
  Supervisor() = default;
  virtual ~Supervisor() = default;

  /**
   * Activate or deactivate the supervisor
   * @return
   */
  virtual bool activate() { return true; }

  /**
   * handle the full report
   * @param report
   */
  virtual void handle_report(const worker_map_t& workers, const handler_map_t& handlers) = 0;

};

#endif //SENSOR_REPORTER_REPORTHANDLER_HPP_
