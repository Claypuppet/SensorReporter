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
  explicit Activatable(bool active) : initialized(false), active(active) {}

  /**
   * Initializes the object if it is active but not initialized.
   * If successfully initialized, sets initialized to true
   */
  virtual void perform_initialize() final{
    if(!initialized && initialize()) {
      initialized = true;
      if(active) {
        activate();
      }
    }
  };

  /**
   * Activates / deactivates the data handler. If the activate / deactivate fails, status wont change
   * Requires the data handler to be initialized
   * @param active: true to activate, false to deactivate
   */
  virtual void set_active(bool _active) final{
    if(!initialized || active == _active) {
      return;
    }
    if(_active && activate()) {
      active = true;
    } else if(!_active) {
      deactivate();
      active = false;
    }
  }

  /// Getter
  virtual bool is_initialized() const final{
    return initialized;
  }

  /// Getter
  virtual bool is_active() const final{
    return active;
  }

 protected:
  /**
   * Initialize the object, this will only be called once.
   * Override this to set pin modes, serial begin or other one time settings
   * @return true if initialize was success. if false, data handler will not be initialized (and wont be used)
   */
  virtual bool initialize(){
    return true;
  }

  /**
   * Perform an activate function, will be called when set_active(true)
   * Override this when the data handler will be activated/deactivated in times. The deactivate can clean up.
   * Example, use of wifi or bluetooth connection
   * @return true if activate was success. if false, object will not be activated (and wont be used)
   */
  virtual bool activate(){
    return true;
  }

  /**
   * Perform an deactivate function, will be called when set_active(false)
   * Override this to cleanup the activation... shut down connections or other
   */
  virtual void deactivate(){}

 private:
  bool initialized;
  bool active;
};

#endif //SENSOR_HANDLER_INCLUDE_ACTIVATABLE_HPP_
