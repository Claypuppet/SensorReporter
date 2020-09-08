//
// Created by Jelle Bouwhuis on 12/27/19.
//

#include <Activatable.hpp>

Activatable::Activatable(): active_state(e_state_inactive) {

}

void Activatable::set_active(bool _activate) {
  if(active_state == e_state_inactive && _activate) {
    active_state = activate(false) ? e_state_active : e_state_activating_failed;
  } else if(active_state != e_state_inactive && !_activate) {
    deactivate();
    active_state = e_state_inactive;
  }
}

bool Activatable::activate(bool retry) {
  return true;
}

void Activatable::deactivate() {}
