//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_REPORTHANDLER_HPP_
#define SENSOR_REPORTER_REPORTHANDLER_HPP_

/**
 * TODO
 */
class ReportHandler {
 public:
  ReportHandler() = default;
  virtual ~ReportHandler() = default;

  virtual void handle_report(const Report& report) = 0;

};

#endif //SENSOR_REPORTER_REPORTHANDLER_HPP_
