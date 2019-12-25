//
// Created by Jelle Bouwhuis on 12/24/19.
//

#ifndef SENSOR_REPORTER_SENSOR_HPP_
#define SENSOR_REPORTER_SENSOR_HPP_

#include <Arduino.h>

/**
 * Base class for the data retriever, dont extend this, this is just to be able to keep a list of them
 * Each retriever needs an unique ID, best defined with an enum or something
 */
class BaseDataRetriever {
 public:
  BaseDataRetriever() = default;
  virtual ~BaseDataRetriever() = default;

  /**
   * Get the retriever id
   * @return retriever id
   */
  virtual uint8_t get_retriever_id() const = 0;

  /**
   * Get pointer to last data measured
   * @return void pointer to data last measured
   */
  virtual void* get_last() = 0;

  /**
   * Retrieve new data, this is called by the aggregator and handled in the DataRetriever class
   * @return true if new data has been recorded
   */
  virtual bool retrieve_data() = 0;
};

/**
 * The DataRetriever should be extended by sensors and such, so they can be registered by the aggregator.
 * @tparam T: Type of the data object (can be int, string, or some struct type)
 */
template<typename T>
class DataRetriever : public BaseDataRetriever {
 public:
  /**
   * Construct a data retriever
   * @param measure_delay : it will wait x milliseconds after successful measurement before getting a new one
   */
  DataRetriever(uint8_t retriever_id, T initial_val, uint32_t measure_delay = 1000)
      : BaseDataRetriever(),
        retriever_id(retriever_id),
        data(initial_val),
        measure_delay(measure_delay),
        last_measure_time(0) {
  }

  virtual ~DataRetriever() = default;

  /**
   * The main function of the class, will do the measurement and store it in `data`
   * @return true if the measurement was success (new data available)
   */
  virtual bool measure() = 0;

  /**
   * Called by the aggregator to retrieve new data. If new data is read, it will be stored in `last_data` to
   * be referenced to by the aggregator measurement data.
   * @return true if new data was read.
   */
  bool retrieve_data() final {
    if((millis() - last_measure_time > 1000 || last_measure_time == 0) && measure()) {
      last_measure_time = millis();
      return true;
    }
    return false;
  }

  /**
   * Get the retriever id
   * @return retriever id
   */
  uint8_t get_retriever_id() const final {
    return retriever_id;
  }

  /**
   * Get the current data object from the retriever
   * @return current data
   */
  const T& get() const {
    return data;
  }

  /**
   * Get a pointer to the last data object of the retriever
   * @return void pointer to the last data
   */
  virtual void* get_last() final {
    return reinterpret_cast<void*>(&data);
  }

 protected:
  uint8_t retriever_id;
  T data;

 private:
  uint32_t measure_delay;
  uint32_t last_measure_time;

};

#endif //SENSOR_REPORTER_SENSOR_HPP_
