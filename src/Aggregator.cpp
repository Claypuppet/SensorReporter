//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include <Aggregator.hpp>

Aggregator::Aggregator() : report(measurements) {

}

void Aggregator::register_retriever(BaseDataRetriever& retriever) {
  auto retriever_id = retriever.get_retriever_id();
  if(retrievers.find(retriever_id) == retrievers.end()) {
    // Create new retriever and measurement
    retrievers[retriever_id] = &retriever;
    measurements[retriever_id] = Measurement();
  } else {
    // Retriever with this id already exists...
    // TODO: add error logging
  }
}

void Aggregator::register_reporter(Reporter& reporter) {
  auto reporter_id = reporter.get_reporter_id();
  if(reporters.find(reporter_id) == reporters.end()) {
    // Create new reporter
    reporters[reporter_id] = &reporter;
  } else {
    // Reporter with this id already exists...
    // TODO: add error logging
  }
}

void Aggregator::register_report_handler(ReportHandler& handler) {
  report_handlers.push_back(&handler);
}

void Aggregator::initialize_all() {
  for(auto r : reporters) {
    r.second->initialize();
  }
}

void Aggregator::run() {
  bool any_new = false;
  // get measurements from the data retrievers
  for(const auto r : retrievers) {
    auto retriever = r.second;
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
  for(const auto r : reporters) {
    auto reporter = r.second;
    if(reporter) {
      reporter->perform_report(report);
    }
  }
  // TODO: Handle the final report
//  for(auto r : report_handlers) {
//  }
  // Mark all the measurements as not-fresh
  for(auto& measurement : measurements) {
    measurement.second.fresh = false;
  }
}
