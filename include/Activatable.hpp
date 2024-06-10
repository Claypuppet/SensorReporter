//
// Created by Jelle Bouwhuis on 12/27/19.
//

#ifndef SENSOR_HANDLER_INCLUDE_ACTIVATABLE_HPP_
#define SENSOR_HANDLER_INCLUDE_ACTIVATABLE_HPP_

/**
 * Some abstract class used by the receiver and observer
 */
class Activatable {
 public:
  typedef enum State {
    e_state_inactive,
    e_state_active,
    e_state_activating_failed,
  } State;

  Activatable();

  /**
   * Returns true if worker/handler is active
   * @return
   */
  bool active() const;

  /**
   * Returns current state of the worker/handler
   * @return
   */
  State get_active_state() const;

  /**
   * Activates / deactivates the data worker/handler. If the activate / deactivate fails, status wont change
   * Requires the data handler to be initialized
   * @param active: true to activate, false to deactivate
   */
  virtual bool set_active(bool _activate) final;

 protected:

  /**
   * Initializes this (called once, when registered in the aggregator)
   */
  virtual void initialize() {}

  /**
   * Perform an activate function, will be called when set_active(true)
   * Override this when the data handler will be activated/deactivated in times. The deactivate can clean up.
   * Example, use of wifi or bluetooth connection
   * @return true if activate was success. if false, object will not be activated (and wont be used)
   */
  virtual bool activate(bool retry);

  /**
   * Perform an deactivate function, will be called when set_active(false)
   * Override this to cleanup the activation... shut down connections or other
   */
  virtual void deactivate();

 private:
  State active_state;
};

#endif //SENSOR_HANDLER_INCLUDE_ACTIVATABLE_HPP_
