//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_REPORTHANDLER_HPP_
#define SENSOR_REPORTER_REPORTHANDLER_HPP_

#include <Handler.hpp>
#include <Worker.hpp>

/**
 * Supervisor will oversee the full report. Mainly used for outputting all possible values (for example, put results on
 * a screen or control an LED).
 */
class Supervisor {
 public:
  Supervisor() = default;
  virtual ~Supervisor() = default;

  /**
   * Initializes this (called once, when registered in the aggregator)
   */
  virtual void initialize() {}

  /**
   * handle the full report
   * @param report
   */
  virtual void handle_report(const WorkerMap& workers, const HandlerMap& handlers) = 0;

};

#endif //SENSOR_REPORTER_REPORTHANDLER_HPP_
