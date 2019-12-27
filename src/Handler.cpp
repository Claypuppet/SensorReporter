//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include "Handler.hpp"

Handler::Handler(uint8_t handler_id, bool active) : Activatable(active), handler_id(handler_id) {
}

void Handler::try_handle_work(worker_report_map_t& work_reports, HandlerReport& status) {
  if(!is_active() || !is_initialized()) {
    status.state = HandlerReport::e_inactive;
    return;
  }
  status.state = handle_produced_work(work_reports);
}

uint8_t Handler::get_handler_id() {
  return handler_id;
}