//
// Created by Jelle Bouwhuis on 12/24/19.
//

#include "Handler.hpp"

Handler::Handler() : Activatable(), status(e_handler_idle) {
}

void Handler::try_handle_work(const worker_map_t& workers) {
  if(active_state == e_state_activating_failed) {
    // Still activating, will try to activate again
    if (activate(true)) {
      active_state = e_state_active;
    }
  }
  if(active()) {
    status = handle_produced_work(workers);
  }
}
