//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include <Aggregator.hpp>

Aggregator::Aggregator() : report(measurements, handler_status) {

}

void Aggregator::register_worker(BaseWorker& worker) {
  auto receiver_id = worker.get_worker_id();
  if(workers.find(receiver_id) == workers.end()) {
    // Create new worker and measurement
    workers[receiver_id] = &worker;
    measurements[receiver_id] = WorkReport();
  } else {
    // Receiver with this id already exists...
    // TODO: add error logging
  }
}

void Aggregator::register_handler(Handler& handler) {
  auto handler_id = handler.get_handler_id();
  if(handlers.find(handler_id) == handlers.end()) {
    // Create new handler
    handlers[handler_id] = &handler;
    handler_status[handler_id] = HandlerReport();
  } else {
    // Observer with this id already exists...
    // TODO: add error logging
  }
}

void Aggregator::register_supervisor(Supervisor& supervisor) {
  supervisors.push_back(&supervisor);
}

void Aggregator::initialize_all() {
  for(const auto& o : handlers) {
    o.second->perform_initialize();
  }
  for(const auto& r : workers) {
    r.second->perform_initialize();
  }
}

void Aggregator::run() {
  bool any_new = false;
  // get worker_reports from the data workers
  for(const auto& w : workers) {
    auto& worker = w.second;
    auto& worker_status = measurements[w.first];
    if(worker && worker->work(worker_status)) {
      any_new = true;
    }
  }
  // If no new worker_reports, skip the cycle
  if(!any_new) {
    return;
  }
  // Report the worker_reports
  for(const auto& r : handlers) {
    auto handler = r.second;
    if(handler && handler->is_active()) {
      handler->try_handle_work(measurements, handler_status.at(handler->get_handler_id()));
    }
  }
  // Handle the final report
  for(const auto& report_handler : supervisors) {
    if(report_handler) {
      report_handler->handle_report(report);
    }
  }
}
