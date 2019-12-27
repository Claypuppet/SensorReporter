//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include "DataObserver.hpp"

DataObserver::DataObserver(uint8_t handler_id, bool active) : Activatable(active), handler_id(handler_id) {
}


int8_t DataObserver::pre_report() {
  return ReporterStatus::StatusCode::e_ok;
}

int8_t DataObserver::post_report() {
  return ReporterStatus::StatusCode::e_ok;
}

void DataObserver::perform_report(measurement_map_t& measurements, ReporterStatus& status) {
  if(!is_active() || !is_initialized()) {
    status.pre = ReporterStatus::e_inactive;
    status.report = ReporterStatus::e_inactive;
    status.post = ReporterStatus::e_inactive;
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

uint8_t DataObserver::get_observer_id() {
  return handler_id;
}