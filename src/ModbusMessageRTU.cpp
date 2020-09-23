// =================================================================================================
// ModbusClient: Copyright 2020 by Michael Harwerth, Bert Melis and the contributors to ModbusClient
//               MIT license - see license.md for details
// =================================================================================================
#include "ModbusMessageRTU.h"
#include <string.h>
#include <Arduino.h>

// calcCRC: calculate Modbus CRC16 on a given array of bytes
uint16_t RTUCRC::calcCRC(const uint8_t *data, uint16_t len) {
  // CRC16 pre-calculated tables
  const uint8_t crcHiTable[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
  };

  const uint8_t crcLoTable[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
  };

  register uint8_t crcHi = 0xFF;
  register uint8_t crcLo = 0xFF;
  register uint8_t index;

  while (len--) {
    index = crcLo ^ *data++;
    crcLo = crcHi ^ crcHiTable[index];
    crcHi = crcLoTable[index];
  }
  return (crcHi << 8 | crcLo);
}

// Default constructor: call ModbusRequest constructor, then init CRC
RTURequest::RTURequest(uint16_t dataLen, uint32_t token) :
  ModbusRequest(dataLen, token),
  CRC(0) { }

// Factory methods to create valid Modbus messages from the parameters
// 1. no additional parameter (FCs 0x07, 0x0b, 0x0c, 0x11)
RTURequest *RTURequest::createRTURequest(Error& returnCode, uint8_t serverID, uint8_t functionCode, uint32_t token) {
  RTURequest *returnPtr = nullptr;
  // Check parameter for validity
  returnCode = checkData(serverID, functionCode);
  // No error? 
  if (returnCode == SUCCESS)
  {
    // Yes, all fine. Create new RTURequest instance
    returnPtr = new RTURequest(2, token);
    returnPtr->add(serverID);
    returnPtr->add(functionCode);
    returnPtr->CRC = RTUCRC::calcCRC(returnPtr->MM_data, returnPtr->MM_index);
  }
  return returnPtr;
}

// 2. one uint16_t parameter (FC 0x18)
RTURequest *RTURequest::createRTURequest(Error& returnCode, uint8_t serverID, uint8_t functionCode, uint16_t p1, uint32_t token) {
  RTURequest *returnPtr = nullptr;
  // Check parameter for validity
  returnCode = checkData(serverID, functionCode, p1);
  // No error? 
  if (returnCode == SUCCESS)
  {
    // Yes, all fine. Create new RTURequest instance
    returnPtr = new RTURequest(4, token);
    returnPtr->add(serverID);
    returnPtr->add(functionCode);
    returnPtr->add(p1);
    returnPtr->CRC = RTUCRC::calcCRC(returnPtr->MM_data, returnPtr->MM_index);
  }
  return returnPtr;
}

// 3. two uint16_t parameters (FC 0x01, 0x02, 0x03, 0x04, 0x05, 0x06)
RTURequest *RTURequest::createRTURequest(Error& returnCode, uint8_t serverID, uint8_t functionCode, uint16_t p1, uint16_t p2, uint32_t token) {
  RTURequest *returnPtr = nullptr;
  // Check parameter for validity
  returnCode = checkData(serverID, functionCode, p1, p2);
  // No error? 
  if (returnCode == SUCCESS)
  {
    // Yes, all fine. Create new RTURequest instance
    returnPtr = new RTURequest(6, token);
    returnPtr->add(serverID);
    returnPtr->add(functionCode);
    returnPtr->add(p1);
    returnPtr->add(p2);
    returnPtr->CRC = RTUCRC::calcCRC(returnPtr->MM_data, returnPtr->MM_index);
  }
  return returnPtr;
}

// 4. three uint16_t parameters (FC 0x16)
RTURequest *RTURequest::createRTURequest(Error& returnCode, uint8_t serverID, uint8_t functionCode, uint16_t p1, uint16_t p2, uint16_t p3, uint32_t token) {
  RTURequest *returnPtr = nullptr;
  // Check parameter for validity
  returnCode = checkData(serverID, functionCode, p1, p2, p3);
  // No error? 
  if (returnCode == SUCCESS)
  {
    // Yes, all fine. Create new RTURequest instance
    returnPtr = new RTURequest(8, token);
    returnPtr->add(serverID);
    returnPtr->add(functionCode);
    returnPtr->add(p1);
    returnPtr->add(p2);
    returnPtr->add(p3);
    returnPtr->CRC = RTUCRC::calcCRC(returnPtr->MM_data, returnPtr->MM_index);
  }
  return returnPtr;
}

// 5. two uint16_t parameters, a uint8_t length byte and a uint16_t* pointer to array of words (FC 0x10)
RTURequest *RTURequest::createRTURequest(Error& returnCode, uint8_t serverID, uint8_t functionCode, uint16_t p1, uint16_t p2, uint8_t count, uint16_t *arrayOfWords, uint32_t token) {
  RTURequest *returnPtr = nullptr;
  // Check parameter for validity
  returnCode = checkData(serverID, functionCode, p1, p2, count, arrayOfWords);
  // No error? 
  if (returnCode == SUCCESS)
  {
    // Yes, all fine. Create new RTURequest instance
    returnPtr = new RTURequest(7 + count, token);
    returnPtr->add(serverID);
    returnPtr->add(functionCode);
    returnPtr->add(p1);
    returnPtr->add(p2);
    returnPtr->add(count);
    for (uint8_t i = 0; i < (count >> 1); ++i) {
      returnPtr->add(arrayOfWords[i]);
    }
    returnPtr->CRC = RTUCRC::calcCRC(returnPtr->MM_data, returnPtr->MM_index);
  }
  return returnPtr;
}

// 6. two uint16_t parameters, a uint8_t length byte and a uint8_t* pointer to array of bytes (FC 0x0f)
RTURequest *RTURequest::createRTURequest(Error& returnCode, uint8_t serverID, uint8_t functionCode, uint16_t p1, uint16_t p2, uint8_t count, uint8_t *arrayOfBytes, uint32_t token) {
  RTURequest *returnPtr = nullptr;
  // Check parameter for validity
  returnCode = checkData(serverID, functionCode, p1, p2, count, arrayOfBytes);
  // No error? 
  if (returnCode == SUCCESS)
  {
    // Yes, all fine. Create new RTURequest instance
    returnPtr = new RTURequest(7 + count, token);
    returnPtr->add(serverID);
    returnPtr->add(functionCode);
    returnPtr->add(p1);
    returnPtr->add(p2);
    returnPtr->add(count);
    for (uint8_t i = 0; i < count; ++i) {
      returnPtr->add(arrayOfBytes[i]);
    }
    returnPtr->CRC = RTUCRC::calcCRC(returnPtr->MM_data, returnPtr->MM_index);
  }
  return returnPtr;
}

// 7. generic constructor for preformatted data ==> count is counting bytes!
RTURequest *RTURequest::createRTURequest(Error& returnCode, uint8_t serverID, uint8_t functionCode, uint16_t count, uint8_t *arrayOfBytes, uint32_t token) {
  RTURequest *returnPtr = nullptr;
  // Check parameter for validity
  returnCode = checkData(serverID, functionCode, count, arrayOfBytes);
  // No error? 
  if (returnCode == SUCCESS)
  {
    // Yes, all fine. Create new RTURequest instance
    returnPtr = new RTURequest(2 + count, token);
    returnPtr->add(serverID);
    returnPtr->add(functionCode);
    for (uint8_t i = 0; i < count; ++i) {
      returnPtr->add(arrayOfBytes[i]);
    }
    returnPtr->CRC = RTUCRC::calcCRC(returnPtr->MM_data, returnPtr->MM_index);
  }
  return returnPtr;
}

// Default constructor for RTUResponse: call ModbusResponse constructor
RTUResponse::RTUResponse(uint16_t dataLen) :
  ModbusResponse(dataLen) { }

// isValidCRC: check if CRC value matches CRC calculated over MM_data.
// *** Note: assumption is made that ModbusRTU has already extracted the CRC from the received message!
bool RTUResponse::isValidCRC() {
  if (MM_data && MM_index) {
    if (CRC == RTUCRC::calcCRC(MM_data, MM_index)) return true;
  }
  return false;
}

// setCRC: set CRC value without calculation
// To be done by ModbusRTU after receiving a response. CRC shall be extracted from the response!
void RTUResponse::setCRC(uint16_t crc) {
  CRC = crc;
}

#ifdef TESTING
// Test: Output CRC and message to Serial.
void RTURequest::dump(const char *label) {
  char buffer[80];

  snprintf(buffer, 80, "%s CRC:%04X", label, CRC);
  Serial.println(buffer);
  ModbusMessage::dump("Data");
}

// ********************** Test: Output CRC and message to Serial.
void RTUResponse::dump(const char *label) {
  char buffer[80];

  snprintf(buffer, 80, "%s CRC:%04X", label, CRC);
  Serial.println(buffer);
  ModbusMessage::dump("Data");
}
#endif
