//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include "Reporter.hpp"

Reporter::Reporter(uint8_t reporter_id, bool active) : reporter_id(reporter_id), initialized(false), active(active) {

}

void Reporter::perform_initialize() {
  if(!initialized && initialize()) {
    initialized = true;
    if(active) {
      activate();
    }
  }
}

bool Reporter::initialize() {
  return true;
}

bool Reporter::activate() {
  return true;
}

void Reporter::deactivate() {
}

int8_t Reporter::pre_report() {
  return ReporterStatus::StatusCode::e_ok;
}

int8_t Reporter::post_report() {
  return ReporterStatus::StatusCode::e_ok;
}

void Reporter::perform_report(measurement_map_t& measurements, ReporterStatus& status) {
  if(!active || !initialized) {
    status.pre = ReporterStatus::e_unused;
    status.report = ReporterStatus::e_unused;
    status.post = ReporterStatus::e_unused;
    return;
  }
  status.pre = pre_report();
  if(status.pre == ReporterStatus::e_ok) {
    status.report = report_measurements(measurements);
    if(status.report == ReporterStatus::e_ok) {
      status.post = post_report();
    }
  }
}

uint8_t Reporter::get_reporter_id() {
  return reporter_id;
}

void Reporter::set_active(bool _active) {
  if(!initialized || active == _active) {
    return;
  }
  if(_active && activate()) {
    active = true;
  } else if(!_active) {
    deactivate();
    active = false;
  }
}
