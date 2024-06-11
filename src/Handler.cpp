//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include "Handler.hpp"

Handler::Handler() : Activatable(), status(e_handler_idle) {
}

int8_t Handler::get_status() const {
  return status;
}

void Handler::try_handle_work(const WorkerMap& workers) {
  if(get_active_state() == e_state_activating_failed) {
    // Still activating, will try to activate again
    set_active(true);
  }
  if(active()) {
    if (!task_running()) {
      if (status == e_handler_processing) {
        // Task completed async, set status based on result
        status = async_result_status;
        async_result_status = e_handler_idle;
      } else {
        // handle data normally
        status = handle_produced_work(workers);
      }
    }
  }
}

int8_t Handler::handle_async() {
  return e_handler_idle;
}

int8_t Handler::start_task(const char* task_name, uint32_t memory, uint8_t priority, uint8_t core) {
  if (xAsyncHandlerHandle != nullptr) {
    return e_handler_processing; // already handling
  }
  xTaskCreatePinnedToCore(Handler::run_task, task_name, memory, this, priority, &xAsyncHandlerHandle, core);
  return e_handler_processing;
}

void Handler::kill_task() {
  if (task_running()) {
    vTaskDelete(xAsyncHandlerHandle);
    xAsyncHandlerHandle = nullptr;
  }
}

bool Handler::task_running() const {
  return xAsyncHandlerHandle != nullptr;
}

void Handler::run_task(void* instance) {
  auto handler = (Handler*) instance;
  handler->async_result_status = handler->handle_async();
  handler->xAsyncHandlerHandle = nullptr;
  vTaskDelete(nullptr);
}
