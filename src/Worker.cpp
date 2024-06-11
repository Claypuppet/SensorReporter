//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include "Worker.hpp"

BaseWorker::BaseWorker(uint32_t break_duration)
    : Activatable(), break_duration(break_duration), last_produce(0), status(Status::e_worker_idle), async_result_status(Status::e_worker_idle) {
}

int8_t BaseWorker::get_status() const {
  return status;
}

uint32_t BaseWorker::get_last_produce() const {
  return last_produce;
}

bool BaseWorker::is_fresh() const {
  return get_active_state() == e_state_active && status == e_worker_data_read;
}

int8_t BaseWorker::produce_async_data() {
  return e_worker_idle;
}

bool BaseWorker::work(const worker_map_t& workers) {
  if (get_active_state() == e_state_activating_failed) {
    // Still activating, will try to activate again
    set_active(true);
  }
  if (active()) {
    if (task_running()) {
      // Task running async, skip working
      return false;
    } else if (status == e_worker_processing) {
      // Task completed async, prepare data to be used in system
      status = async_result_status;
      async_result_status = e_worker_idle;
      finish_produced_data();
    } else {
      // Normal work process
      if ((millis() - last_produce > break_duration || last_produce == 0)) {
        status = is_process_worker() ? produce_data(workers) : produce_data();
      }
      else {
        status = e_worker_idle;
      }
    }
    if (is_fresh()) {
      // Work has been produced
      last_produce = millis();
      return true;
    }
  }
  return false;
}

int8_t BaseWorker::start_task(const char* task_name, uint32_t memory, uint8_t priority, uint8_t core) {
  if (xAsyncWorkerHandle != nullptr) {
    return e_worker_processing; // Already working
  }
  xTaskCreatePinnedToCore(BaseWorker::run_task, task_name, memory, this, priority, &xAsyncWorkerHandle, core);
  return e_worker_processing;
}

void BaseWorker::kill_task() {
  if (xAsyncWorkerHandle != nullptr) {
    vTaskDelete(xAsyncWorkerHandle);
    xAsyncWorkerHandle = nullptr;
  }
}

bool BaseWorker::task_running() const {
  return xAsyncWorkerHandle != nullptr;
}

void BaseWorker::run_task(void* instance) {
  auto worker = (BaseWorker*) instance;
  worker->async_result_status = worker->produce_async_data();
  worker->xAsyncWorkerHandle = nullptr;
  vTaskDelete(nullptr);
}

void BaseWorker::finish_produced_data() {
}

bool WorkerMap::any_updates() const {
  return std::any_of(
      begin(),
      end(),
      [](const std::pair<uint8_t, BaseWorker*>& pair) {
        return pair.second->get_status() != BaseWorker::e_worker_idle;
      }
  );
}
