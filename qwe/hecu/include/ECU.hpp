//
// Created by jadjer on 01.09.22.
//

#pragma once

#include "CommandResult.hpp"
#include "IProtocol.hpp"
#include "data/EngineData.hpp"
#include "data/ErrorData.hpp"
#include "data/SensorsData.hpp"
#include "data/UnknownData.hpp"
#include "data/VehicleData.hpp"
#include <esp_err.h>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

class ECU {
 public:
  explicit ECU(IProtocol* protocol);
  ~ECU();

 public:
  /**
 * @brief
 * @return
 */
  esp_err_t connect();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool isConnected() const;

 public:
  /**
 * @brief
 */
  void detectAllTables();

  /**
 * @brief
 */
  void detectActiveTables();

  /**
 * @brief
 */
  void updateAllData();

 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] VehicleData getVehicleData() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] EngineData getEngineData() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] SensorsData getSensorsData() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] ErrorData getErrorData() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] UnknownData getUnknownData() const;

 private:
  std::mutex m_mutex;
  IProtocol* m_protocol;

 private:
  bool m_isConnected;
  bool m_enableTable10;
  bool m_enableTable11;
  bool m_enableTable20;
  bool m_enableTable21;

 private:
  VehicleData m_vehicleData;
  EngineData m_engineData;
  SensorsData m_sensorsData;
  ErrorData m_errorData;
  UnknownData m_unknownData;

 private:
  CommandResult* updateDataFromTable(uint8_t table);
  esp_err_t updateDataFromTable0();
  esp_err_t updateDataFromTable10();
  esp_err_t updateDataFromTable11();
  esp_err_t updateDataFromTable20();
  esp_err_t updateDataFromTable21();
  esp_err_t updateDataFromTableD0();
  esp_err_t updateDataFromTableD1();
};
