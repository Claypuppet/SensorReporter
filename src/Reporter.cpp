//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include "Reporter.hpp"

Reporter::Reporter(uint8_t reporter_id) : reporter_id(reporter_id) {

}

bool Reporter::initialize() {
  return true;
}

bool Reporter::activate() {
  return true;
}

bool Reporter::deactivate() {
  return true;
}

bool Reporter::pre_report() {
  return true;
}

bool Reporter::report_measurements(const measurement_list_t& measurements) {
  return true;
}

bool Reporter::post_report() {
  return true;
}

void Reporter::perform_report(Report& report) {
  pre_report();
  report_measurements(report.measurements);
  post_report();
}

uint8_t Reporter::get_reporter_id() {
  return reporter_id;
}
