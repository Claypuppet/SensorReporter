//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include <Aggregator.hpp>

Aggregator::Aggregator() = default;

void Aggregator::register_worker(uint8_t worker_id, BaseWorker& worker) {
  if(workers.find(worker_id) == workers.end()) {
    // Create new worker and measurement
    workers[worker_id] = &worker;
    worker.initialize();
  } else {
    // Receiver with this id already exists...
    // TODO: add error logging
  }
}

void Aggregator::register_handler(uint8_t handler_id, Handler& handler) {
  if(handlers.find(handler_id) == handlers.end()) {
    // Create new handler
    handlers[handler_id] = &handler;
    handler.initialize();
  } else {
    // Observer with this id already exists...
    // TODO: add error logging
  }
}

void Aggregator::register_supervisor(Supervisor& supervisor) {
  supervisors.push_back(&supervisor);
  supervisor.initialize();
}

void Aggregator::run() {
  bool any_new = false;
  // get worker_reports from the data workers
  for(const auto& w : workers) {
    auto& worker = w.second;
    if(worker && worker->work()) {
      any_new = true;
    }
  }
  if(any_new) {
    // Report the worker_reports
    for(const auto& r : handlers) {
      auto handler = r.second;
      if(handler) {
        handler->try_handle_work(workers);
      }
    }
  }
  // Handle the final report
  for(const auto& report_handler : supervisors) {
    if(report_handler) {
      report_handler->handle_report(workers, handlers);
    }
  }
}

void Aggregator::set_worker_active(uint8_t worker_id, bool active) {
  workers.at(worker_id)->set_active(active);
}

void Aggregator::set_handler_active(uint8_t handler_id, bool active) {
  handlers.at(handler_id)->set_active(active);
}
