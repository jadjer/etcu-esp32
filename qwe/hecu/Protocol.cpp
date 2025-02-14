//
// Created by jadjer on 24.08.22.
//

#include "Protocol.hpp"
#include "utils.hpp"
#include <esp_log.h>

Protocol::Protocol(int8_t rxPin, int8_t txPin) : m_serial(2) {
  m_uartRx = rxPin;
  m_uartTx = txPin;
}

Protocol::~Protocol() = default;

esp_err_t Protocol::connect() {
  wakeup();
  return initialize();
}

CommandResult* Protocol::readData() {
  auto result = new CommandResult;

  // Wait minimum package from uart
  waitDataFromUart(4);

  {
    waitDataFromUart(3);

    uint8_t buffer[3];

    size_t len = m_serial.readBytes(buffer, 3);
    if (len != 3) {
      log_w("Error read data from uart");
      delete result;
      return nullptr;
    }

    result->code = buffer[0];
    result->length = buffer[1];
    result->command = buffer[2];
  }

  if (result->length < 4) {
    log_w("The message is very small");
    delete result;
    return nullptr;
  }

  if (result->length > SOC_UART_FIFO_LEN) {
    log_w("The message is too big");
    delete result;
    return nullptr;
  }

  result->data = new uint8_t [result->length];
  result->data[0] = result->code;
  result->data[1] = result->length;
  result->data[2] = result->command;

  {
    size_t responsePayloadLength = result->length - 3;

    //    Wait full package from uart
    waitDataFromUart(responsePayloadLength);

    //    Read package data from uart
    size_t len = m_serial.readBytes(&result->data[3], responsePayloadLength);
    if (len != responsePayloadLength) {
      log_w("Error read data from uart");
      delete[] result->data;
      delete result;
      return nullptr;
    }

    result->checksum = result->data[result->length - 1];
  }

  uint8_t calculatedChecksum = calcChecksum(result->data, result->length - 1);
  if (result->checksum != calculatedChecksum) {
    log_w("The checksum does not match");
    delete[] result->data;
    delete result;
    return nullptr;
  }

  log_buf_i(result->data, result->length);

  return result;
}

void Protocol::writeData(uint8_t const* data, size_t len) {
  m_serial.flush(false);
  m_serial.write(data, len);

  //  Wait echo data from ECU
  waitDataFromUart(len);

  //  Echo data's delete from RX buffer
  uint8_t buffer[len];
  m_serial.readBytes(buffer, len);

  log_buf_i(data, len);
}

void Protocol::wakeup() const {
  log_i("Wakeup ECU...");

  pinMode(m_uartTx, OUTPUT);

  digitalWrite(m_uartTx, LOW);
  delay(70);

  digitalWrite(m_uartTx, HIGH);
  delay(130);
}

esp_err_t Protocol::initialize() {
  log_i("Initialize ECU...");

  uint8_t wakeupMessage[] = {0xFE, 0x04, 0xFF, 0xFF};
  uint8_t initializeMessage[] = {0x72, 0x05, 0x00, 0xF0, 0x99};

  m_serial.begin(10400, SERIAL_8N1, m_uartRx, m_uartTx);

  log_i("Send wakeup message...");
  writeData(wakeupMessage, sizeof(wakeupMessage));

  delay(20);

  log_i("Send initialize message...");
  writeData(initializeMessage, sizeof(initializeMessage));

  auto result = readData();
  if (result == nullptr) { return ESP_ERR_INVALID_RESPONSE; }

  delete[] result->data;

  if (result->code != 0x02) {
    delete result;
    return ESP_ERR_INVALID_RESPONSE;
  }

  if (result->length != 0x04) {
    delete result;
    return ESP_ERR_INVALID_SIZE;
  }

  if (result->command != 0x00) {
    delete result;
    return ESP_ERR_INVALID_ARG;
  }

  if (result->checksum != 0xFA) {
    delete result;
    return ESP_ERR_INVALID_CRC;
  }

  delete result;
  return ESP_OK;
}

void Protocol::waitDataFromUart(size_t len) {
  while (m_serial.available() < len) { delay(1); }
}
