//
// Created by jadjer on 24.08.22.
//

#pragma once

#include "IProtocol.hpp"
#include <Arduino.h>

class Protocol : public IProtocol {
 public:
  Protocol(int8_t rxPin, int8_t txPin);
  ~Protocol() override;

 public:
  /**
   * @brief
   * @return
   */
  esp_err_t connect() override;

 public:
  /**
   * @brief
   * @return
   */
  CommandResult* readData() override;

  /**
   * @brief
   * @param data
   * @param len
   */
  void writeData(uint8_t const* data, size_t len) override;

 private:
  int8_t m_uartRx;
  int8_t m_uartTx;
  HardwareSerial m_serial;

 private:
  void wakeup() const;
  esp_err_t initialize();
  void waitDataFromUart(size_t len);
};
