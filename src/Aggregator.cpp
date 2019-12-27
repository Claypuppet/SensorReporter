//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include <Aggregator.hpp>

Aggregator::Aggregator() : report(measurements, handler_status) {

}

void Aggregator::register_receiver(BaseDataReceiver& receiver) {
  auto receiver_id = receiver.get_receiver_id();
  if(receivers.find(receiver_id) == receivers.end()) {
    // Create new receiver and measurement
    receivers[receiver_id] = &receiver;
    measurements[receiver_id] = Measurement();
  } else {
    // Receiver with this id already exists...
    // TODO: add error logging
  }
}

void Aggregator::register_data_observer(DataObserver& handler) {
  auto handler_id = handler.get_observer_id();
  if(observers.find(handler_id) == observers.end()) {
    // Create new handler
    observers[handler_id] = &handler;
    handler_status[handler_id] = ReporterStatus();
  } else {
    // Observer with this id already exists...
    // TODO: add error logging
  }
}

void Aggregator::register_report_handler(ReportHandler& handler) {
  report_handlers.push_back(&handler);
}

void Aggregator::initialize_all() {
  for(const auto& o : observers) {
    o.second->perform_initialize();
  }
  for(const auto& r : receivers) {
    r.second->perform_initialize();
  }
}

void Aggregator::run() {
  bool any_new = false;
  // get measurements from the data receivers
  for(const auto& r : receivers) {
    auto& retriever = r.second;
    if(retriever && retriever->retrieve_data()) {
      any_new = true;
      measurements[r.first].data = retriever->get_last();
      measurements[r.first].fresh = true;
    }
  }
  // If no new measurements, skip the cycle
  if(!any_new) {
    return;
  }
  // Report the measurements
  for(const auto& r : observers) {
    auto handler = r.second;
    if(handler && handler->is_active()) {
      handler->perform_report(measurements, handler_status.at(handler->get_observer_id()));
    }
  }
  // Handle the final report
  for(const auto& report_handler : report_handlers) {
    if(report_handler) {
      report_handler->handle_report(report);
    }
  }
  // Mark all the measurements as not-fresh
  for(auto& measurement : measurements) {
    measurement.second.fresh = false;
  }
}
