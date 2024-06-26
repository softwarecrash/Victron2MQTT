/***************************************************************************
 *   Copyright (C) 2020-2022  Fabiano Riccardi                             *
 *                                                                         *
 *   This file is part of RTCMemory.                                       *
 *                                                                         *
 *   RTCMemory is free software; you can redistribute                      *
 *   it and/or modify it under the terms of the GNU Lesser General Public  *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   RTCMemory is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with RTCMemory; if not, see <http://www.gnu.org/licenses/>      *
 ***************************************************************************/
//10.4.204 modifyd by softwarecrash, removed fs and other things for saving lot of space
#ifndef RTCMEMORY_H
#define RTCMEMORY_H

//#include <LittleFS.h>
//#include <type_traits>

/**
 * T is the user data structure, N is the max number of bytes used by this library. N acts like a
 * "soft" limiter.
 */
template<typename T, int N = 384> class RTCMemory {
public:
  /**
   * Create RTCMemory. Provide a valid filepath to enable the backup of RTC memory on flash memory.
   * By default the library uses LittleFS, but you can select an alternative filesystem such as
   * SPIFFS.
   */
  RTCMemory();

  /**
   * Initialize the buffer using data in RTC memory. If data are invalid, load data from
   * flash. If it fails again, the buffer is cleared. After this call you can safely use this
   * instance.
   *
   * Return true if at least one data source is valid, otherwise false.
   */
  bool begin();

  /**
   * Write data to RTC memory.
   *
   * Return true if the operation is completed successfully, otherwise false.
   */
  bool save();

  /**
   * Get a pointer to the user buffer, structured accordingly to the typename T.
   *
   * Return a valid pointer to the data if begin() was called, otherwise nullptr.
   */
  T *getData();

private:
  const static unsigned int BLOCK_SIZE = 4;

  /**
   * Number of bytes to contain the user data structure. This value is ceiled to multiple of 4 to
   * respect the memory alignment of RTC memory.
   */
  const static unsigned int USER_RTC_MEMORY_SIZE = (sizeof(T) - 1) / BLOCK_SIZE * BLOCK_SIZE + BLOCK_SIZE;

  /**
   * Max size of user memory.
   */
  const static unsigned int MAX_USER_RTC_MEMORY_SIZE = 508;

  /**
   * Max size available in RTC memory.
   */
  const static unsigned int MAX_TOTAL_RTC_MEMORY_SIZE = 512;

  struct RTCData {
    uint32_t crc32;
    // The user buffer
    byte data[USER_RTC_MEMORY_SIZE];
  };

  /**
   * Offset from the first block in RTC memory. This offset is used to shift data toward the
   * highest RTC memory addresses.
   */
  const static unsigned int OFFSET = (MAX_TOTAL_RTC_MEMORY_SIZE - sizeof(RTCData)) / BLOCK_SIZE;

  // The buffer
  RTCData rtcData;

  /**
   * Tell if this class is ready to be used i.e. if begin() was called.
   */
  bool ready;

  /**
   * Calculate the CRC (32bit) of the given buffer.
   *
   * Return the CRC code.
   */
  uint32_t calculateCRC32(const uint8_t *data, size_t length) const;

  /**
   * Clear the buffer.
   */
  void clearBuffer();
};
template<typename T, int N>
RTCMemory<T, N>::RTCMemory() : ready(false) {
  static_assert(sizeof(T) <= MAX_USER_RTC_MEMORY_SIZE, "The max size of user data is 508 bytes. "
                                                       "You must reduce the size of your data "
                                                       "structure.");
  static_assert(sizeof(RTCData) <= N, "You reach the maximum amount of memory reserved to "
                                      "RTCMemory lib. You may increase this limit beyond 384 bytes "
                                      "and up to 512 bytes, but you will lose RTC data after an "
                                      "OTA update.");
}

template<typename T, int N> bool RTCMemory<T, N>::begin() {
  if (ready) {
    return true;
  }

  if (!ESP.rtcUserMemoryRead(OFFSET, (uint32_t *)&rtcData, sizeof(RTCData))) {
    return false;
  }

  uint32_t crcOfData = calculateCRC32((uint8_t *)&rtcData.data, USER_RTC_MEMORY_SIZE);

  ready = true;

  if (crcOfData != rtcData.crc32) {
      clearBuffer();
      return false;
  }
  return true;
}

template<typename T, int N> bool RTCMemory<T, N>::save() {
  if (ready) {
    uint32_t crcOfData = calculateCRC32((uint8_t *)&rtcData.data, USER_RTC_MEMORY_SIZE);
    rtcData.crc32 = crcOfData;

    if (ESP.rtcUserMemoryWrite(OFFSET, (uint32_t *)&rtcData, sizeof(RTCData))) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

template<typename T, int N> T *RTCMemory<T, N>::getData() {
  if (ready) { return reinterpret_cast<T *>(rtcData.data); }
  return nullptr;
};


template<typename T, int N> uint32_t RTCMemory<T, N>::calculateCRC32(const uint8_t *data, size_t length) const {
  uint32_t crc = 0xffffffff;
  while (length--) {
    uint8_t c = *data++;
    for (uint32_t i = 0x80; i > 0; i >>= 1) {
      bool bit = crc & 0x80000000;
      if (c & i) { bit = !bit; }
      crc <<= 1;
      if (bit) { crc ^= 0x04c11db7; }
    }
  }
  return crc;
}

template<typename T, int N> void RTCMemory<T, N>::clearBuffer() {
  memset((void *)&rtcData.data, 0, USER_RTC_MEMORY_SIZE);
  uint32_t result = calculateCRC32((uint8_t *)rtcData.data, USER_RTC_MEMORY_SIZE);
  rtcData.crc32 = result;
}

#endif  // END RTCMEMORY_H
