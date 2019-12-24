//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include <map>
#include <vector>
#include "Report.hpp"
#include "Measurement.hpp"
#include "Reporter.hpp"
#include "ReportHandler.hpp"
#include "DataRetriever.hpp"

#ifndef SENSOR_REPORTER_AGGREGATOR_HPP_
#define SENSOR_REPORTER_AGGREGATOR_HPP_

/**
 * Some class that collects the data from the retrievers and passes it to the reporters.
 * Can register
 * - retrievers (that retrieve data)
 * - reporters (that report data)
 * - handlers (that handle the report results)
 */
class Aggregator {
 public:
  Aggregator();
  virtual ~Aggregator() = default;

  /**
   * Add a new retriever / sensor to the aggregator
   * @param retriever
   */
  void register_retriever(BaseDataRetriever& retriever);

  /**
   * Add a new reporter to the aggregator
   * @param reporter
   */
  void register_reporter(Reporter& reporter);

  /**
   * Add a new report handler to the aggregator
   * @param handler
   */
  void register_report_handler(ReportHandler& handler);

  /**
   * initialize all reporters
   */
  void initialize_all();

  /**
   * Run the aggregator
   */
  void run();

 private:
  std::map<uint8_t, BaseDataRetriever*> retrievers;
  std::map<uint8_t, Reporter*> reporters;
  std::vector<ReportHandler*> report_handlers;

  measurement_list_t measurements;

  Report report;
};

#endif //SENSOR_REPORTER_AGGREGATOR_HPP_
