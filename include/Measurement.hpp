//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_INCLUDE_MEASUREMENT_HPP_
#define SENSOR_REPORTER_INCLUDE_MEASUREMENT_HPP_

#include <map>

/**
 * A single measurement, keeps track if its fresh data and has method to cast the containing data
 */
struct Measurement {
  bool fresh = false;
  void* data = nullptr;

  /**
   * Get the data as type T
   * @tparam T: type of the data
   * @return the data
   */
  template<typename T>
  const T& get() const {
    return *static_cast<T*>(data);
  }
};

typedef std::map<uint8_t, Measurement> measurement_list_t;

#endif //SENSOR_REPORTER_INCLUDE_MEASUREMENT_HPP_
