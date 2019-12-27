//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include <vector>
#include "Report.hpp"
#include "Handler.hpp"
#include "Supervisor.hpp"
#include "Worker.hpp"

#ifndef SENSOR_REPORTER_AGGREGATOR_HPP_
#define SENSOR_REPORTER_AGGREGATOR_HPP_

/**
 * Some class that collects the data from the workers and passes it to the data handlers.
 * Can register
 * - workers (that retrieve data/worker_reports)
 * - handlers (handle the work that the workers produced)
 * - supervisors (that handle the full report)
 */
class Aggregator {
 public:
  Aggregator();
  virtual ~Aggregator() = default;

  /**
   * Add a new worker to the aggregator
   * @param worker
   */
  void register_worker(BaseWorker& worker);

  /**
   * Add a new data handler to the aggregator
   * @param data handler
   */
  void register_handler(Handler& handler);

  /**
   * Add a new report supervisor to the aggregator
   * @param supervisor
   */
  void register_supervisor(Supervisor& supervisor);

  /**
   * initialize all data handlers
   */
  void initialize_all();

  /**
   * Run the aggregator
   * Three steps:
   * 1. workers produce work
   *   - If no fresh work is produced, the next steps are skipped
   * 2. handlers handle produced work
   * 3. supervisor oversees final report
   */
  void run();

 private:
  std::map<uint8_t, BaseWorker*> workers;
  std::map<uint8_t, Handler*> handlers;
  std::vector<Supervisor*> supervisors;

  worker_report_map_t measurements;
  handler_reports_map_t handler_status;

  Report report;
};

#endif //SENSOR_REPORTER_AGGREGATOR_HPP_
