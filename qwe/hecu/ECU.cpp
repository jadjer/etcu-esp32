//
// Created by jadjer on 01.09.22.
//

#include "ECU.hpp"
#include "CommandResult.hpp"
#include "Protocol.hpp"
#include "utils.hpp"

ECU::ECU(IProtocol* protocol) {
  m_protocol = protocol;
  m_isConnected = false;

  m_enableTable10 = true;
  m_enableTable11 = true;
  m_enableTable20 = true;
  m_enableTable21 = true;

  m_vehicleData = {.id = "undefined", .batteryVolts = 12.7, .speed = 120, .state = 1};
  m_engineData = {.rpm = 4300, .fuelInject = 1000, .ignitionAdvance = 19, .unkData1 = 0, .unkData2 = 0, .unkData3 = 0};
  m_sensorsData = {.tpsPercent = 34.6, .tpsVolts = 1.86, .ectTemp = 87, .ectVolts = 3.2, .iatTemp = 16, .iatVolts = 2.3, .mapPressure = 69, .mapVolts = 3.6};
  m_errorData = {};
  m_unknownData = {};
}

ECU::~ECU() = default;

esp_err_t ECU::connect() {
  if (m_isConnected) { return ESP_OK; }

  esp_err_t err = m_protocol->connect();
  if (err == ESP_OK) {
    m_isConnected = true;
  }

  return err;
}

bool ECU::isConnected() const {
  return m_isConnected;
}

void ECU::detectAllTables() {
  if (not m_isConnected) { return; }

  std::lock_guard<std::mutex> lock(m_mutex);

  uint8_t address = 0x0;

  for (size_t i = 0; i <= 255; i++) {
    uint8_t message[5] = {0x72, 0x05, 0x71, address, 0x00};
    message[4] = calcChecksum(message, 4);

    address++;

    m_protocol->writeData(message, 5);
    auto result = m_protocol->readData();

    delete[] result->data;
    delete result;
  }
}

void ECU::detectActiveTables() {
  if (not m_isConnected) { return; }

  std::lock_guard<std::mutex> lock(m_mutex);

  updateDataFromTable0();

  if (updateDataFromTable10() == ESP_OK) { m_enableTable10 = true; }
  if (updateDataFromTable11() == ESP_OK) { m_enableTable11 = true; }
  if (updateDataFromTable20() == ESP_OK) { m_enableTable20 = true; }
  if (updateDataFromTable21() == ESP_OK) { m_enableTable21 = true; }
}

void ECU::updateAllData() {
  if (not m_isConnected) { return; }

  std::lock_guard<std::mutex> lock(m_mutex);

  if (m_enableTable10) { updateDataFromTable10(); }
  if (m_enableTable11) { updateDataFromTable11(); }
  if (m_enableTable20) { updateDataFromTable20(); }
  if (m_enableTable21) { updateDataFromTable21(); }

  updateDataFromTableD0();
  updateDataFromTableD1();
}

VehicleData ECU::getVehicleData() const {
  return m_vehicleData;
}

EngineData ECU::getEngineData() const {
  return m_engineData;
}

SensorsData ECU::getSensorsData() const {
  return m_sensorsData;
}

ErrorData ECU::getErrorData() const {
  return m_errorData;
}

UnknownData ECU::getUnknownData() const {
  return m_unknownData;
}

CommandResult* ECU::updateDataFromTable(uint8_t table) {
  uint8_t message[5] = {0x72, 0x05, 0x71, table, 0x00};
  message[4] = calcChecksum(message, 4);

  m_protocol->writeData(message, 5);

  return m_protocol->readData();
}

esp_err_t ECU::updateDataFromTable0() {
  auto result = updateDataFromTable(0x0);
  if (result == nullptr) { return ESP_FAIL; }
  if (result->length != 0xf) {
    delete[] result->data;
    delete result;
    return ESP_FAIL;
  }

  for (size_t i = 4; i < (result->length - 1); i++) {
    m_vehicleData.id += std::to_string(static_cast<int>(result->data[i]));
  }

  return ESP_OK;
}

esp_err_t ECU::updateDataFromTable10() {
  auto result = updateDataFromTable(0x10);
  if (result == nullptr) { return ESP_FAIL; }
  if (result->length != 0x16) {
    delete[] result->data;
    delete result;
    return ESP_FAIL;
  }

  m_engineData.rpm = (result->data[4] << 8) + result->data[5];
  m_sensorsData.tpsVolts = calcValueDivide256(result->data[6]);
  m_sensorsData.tpsPercent = calcValueDivide16(result->data[7]);
  m_sensorsData.ectVolts = calcValueDivide256(result->data[8]);
  m_sensorsData.ectTemp = calcValueMinus40(result->data[9]);
  m_sensorsData.iatVolts = calcValueDivide256(result->data[10]);
  m_sensorsData.iatTemp = calcValueMinus40(result->data[11]);
  m_sensorsData.mapVolts = calcValueDivide256(result->data[12]);
  m_sensorsData.mapPressure = result->data[13];
  m_vehicleData.batteryVolts = calcValueDivide10(result->data[16]);
  m_vehicleData.speed = result->data[17];
  m_engineData.fuelInject = (result->data[18] << 8) + result->data[19];
  m_engineData.ignitionAdvance = result->data[20];

  return ESP_OK;
}

esp_err_t ECU::updateDataFromTable11() {
  auto result = updateDataFromTable(0x11);
  if (result == nullptr) { return ESP_FAIL; }
  if (result->length != 0x19) {
    delete[] result->data;
    delete result;
    return ESP_FAIL;
  }

  m_engineData.rpm = (result->data[4] << 8) + result->data[5];
  m_sensorsData.tpsVolts = calcValueDivide256(result->data[6]);
  m_sensorsData.tpsPercent = calcValueDivide16(result->data[7]);
  m_sensorsData.ectVolts = calcValueDivide256(result->data[8]);
  m_sensorsData.ectTemp = calcValueMinus40(result->data[9]);
  m_sensorsData.iatVolts = calcValueDivide256(result->data[10]);
  m_sensorsData.iatTemp = calcValueMinus40(result->data[11]);
  m_sensorsData.mapVolts = calcValueDivide256(result->data[12]);
  m_sensorsData.mapPressure = result->data[13];
  m_vehicleData.batteryVolts = calcValueDivide10(result->data[16]);
  m_vehicleData.speed = result->data[17];
  m_engineData.fuelInject = (result->data[18] << 8) + result->data[19];
  m_engineData.ignitionAdvance = result->data[20];

  m_unknownData.unkData1 = result->data[21];
  m_unknownData.unkData2 = result->data[22];
  m_unknownData.unkData3 = result->data[23];

  return ESP_OK;
}

esp_err_t ECU::updateDataFromTable20() {
  auto result = updateDataFromTable(0x20);
  if (result == nullptr) { return ESP_FAIL; }
  if (result->length != 0x8) {
    delete[] result->data;
    delete result;
    return ESP_FAIL;
  }

  m_unknownData.unkData4 = result->data[4];
  m_unknownData.unkData5 = result->data[5];
  m_unknownData.unkData6 = result->data[6];

  return ESP_OK;
}

esp_err_t ECU::updateDataFromTable21() {
  auto result = updateDataFromTable(0x21);
  if (result == nullptr) { return ESP_FAIL; }
  if (result->length != 0xb) {
    delete[] result->data;
    delete result;
    return ESP_FAIL;
  }

  m_unknownData.unkData1 = result->data[4];
  m_unknownData.unkData2 = result->data[5];
  m_unknownData.unkData3 = result->data[6];
  m_unknownData.unkData4 = result->data[7];
  m_unknownData.unkData5 = result->data[8];
  m_unknownData.unkData6 = result->data[9];

  return ESP_OK;
}

esp_err_t ECU::updateDataFromTableD0() {
  auto result = updateDataFromTable(0xD0);
  if (result == nullptr) { return ESP_FAIL; }
  if (result->length != 0x13) {
    delete[] result->data;
    delete result;
    return ESP_FAIL;
  }

  m_unknownData.unkData7 = result->data[4];
  m_unknownData.unkData8 = result->data[5];
  m_unknownData.unkData9 = result->data[6];
  m_unknownData.unkData10 = result->data[7];
  m_unknownData.unkData11 = result->data[8];
  m_unknownData.unkData12 = result->data[9];
  m_unknownData.unkData13 = result->data[10];
  m_unknownData.unkData14 = result->data[11];
  m_unknownData.unkData15 = result->data[12];
  m_unknownData.unkData16 = result->data[13];
  m_unknownData.unkData17 = result->data[14];
  m_unknownData.unkData18 = result->data[15];
  m_unknownData.unkData19 = result->data[16];
  m_unknownData.unkData20 = result->data[17];

  return ESP_OK;
}

esp_err_t ECU::updateDataFromTableD1() {
  auto result = updateDataFromTable(0xD1);
  if (result == nullptr) { return ESP_FAIL; }
  if (result->length != 0xb) {
    delete[] result->data;
    delete result;
    return ESP_FAIL;
  }

  m_vehicleData.state = result->data[4];

  m_unknownData.unkData21 = result->data[5];
  m_unknownData.unkData22 = result->data[6];
  m_unknownData.unkData23 = result->data[7];
  m_unknownData.unkData24 = result->data[8];
  m_unknownData.unkData25 = result->data[9];

  return ESP_OK;
}
