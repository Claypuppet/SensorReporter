//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include <map>
#include <vector>
#include "Report.hpp"
#include "DataObserver.hpp"
#include "ReportHandler.hpp"
#include "DataReceiver.hpp"

#ifndef SENSOR_REPORTER_AGGREGATOR_HPP_
#define SENSOR_REPORTER_AGGREGATOR_HPP_

/**
 * Some class that collects the data from the receivers and passes it to the data observers.
 * Can register
 * - receivers (that retrieve data/measurements)
 * - data observers (report the measurements)
 * - report observers (that handle the report results)
 */
class Aggregator {
 public:
  Aggregator();
  virtual ~Aggregator() = default;

  /**
   * Add a new receiver / sensor to the aggregator
   * @param receiver
   */
  void register_receiver(BaseDataReceiver& receiver);

  /**
   * Add a new data handler to the aggregator
   * @param data handler
   */
  void register_data_observer(DataObserver& handler);

  /**
   * Add a new report handler to the aggregator
   * @param handler
   */
  void register_report_handler(ReportHandler& handler);

  /**
   * initialize all data observers
   */
  void initialize_all();

  /**
   * Run the aggregator
   */
  void run();

 private:
  std::map<uint8_t, BaseDataReceiver*> receivers;
  std::map<uint8_t, DataObserver*> observers;
  std::vector<ReportHandler*> report_handlers;

  measurement_map_t measurements;
  handler_status_map_t handler_status;

  Report report;
};

#endif //SENSOR_REPORTER_AGGREGATOR_HPP_
