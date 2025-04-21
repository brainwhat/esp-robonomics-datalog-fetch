#include "decode.h"
#include <vector>
#include <cstdlib>

// Convert hex string to bytes
std::vector<uint8_t> hexStringToBytes(const char* hexString) {
  std::vector<uint8_t> bytes;
  size_t len = strlen(hexString);

  for (size_t i = 0; i < len; i += 2) {
    char byteString[3] = {hexString[i], hexString[i + 1], '\0'};
    uint8_t byte = strtoul(byteString, nullptr, 16);
    bytes.push_back(byte);
  }

  return bytes;
}

// Decode a compact integer
uint64_t decode_compact_integer(const uint8_t* input, size_t& offset, size_t dataLength) {
  if (offset >= dataLength) return 0;

  uint8_t first_byte = input[offset];
  uint8_t mode = first_byte & 0b11;

  switch (mode) {
    case 0b00: { // Single-byte mode
      offset += 1;
      return first_byte >> 2;
    }
    case 0b01: { // Two-byte mode
      if (offset + 1 >= dataLength) return 0;
      uint16_t value = (static_cast<uint16_t>(input[offset + 1]) << 6) | (first_byte >> 2);
      offset += 2;
      return value;
    }
    case 0b10: { // Four-byte mode
      if (offset + 3 >= dataLength) return 0;
      uint32_t value = 0;
      value |= static_cast<uint32_t>(input[offset + 3]) << 22;
      value |= static_cast<uint32_t>(input[offset + 2]) << 14;
      value |= static_cast<uint32_t>(input[offset + 1]) << 6;
      value |= (first_byte >> 2);
      offset += 4;
      return value;
    }
    case 0b11: { // Big-integer mode
      uint8_t count = (first_byte >> 2) & 0b111111;
      if (offset + count >= dataLength) return 0;
      uint64_t value = 0;
      for (int i = 0; i < count; i++) {
        value |= static_cast<uint64_t>(input[offset + 1 + i]) << (i * 8);
      }
      offset += count + 1;
      return value;
    }
    default:
      return 0;
  }
}

// Decode SCALE data
std::vector<int64_t> decodeScaleData(const std::vector<uint8_t>& encodedData) {
  std::vector<int64_t> result;
  const uint8_t* data = encodedData.data();
  size_t dataLength = encodedData.size();
  size_t offset = 0;

  while (offset < dataLength) {
    uint64_t value = decode_compact_integer(data, offset, dataLength);
    if (value == 0 && offset >= dataLength) break; // Avoid infinite loops
    result.push_back(value);
  }

  return result;
}

// Public interface
int64_t* decode_scale(const char* hexString, size_t* output_length) {
  std::vector<uint8_t> encodedData = hexStringToBytes(hexString);
  std::vector<int64_t> decoded = decodeScaleData(encodedData);

  *output_length = decoded.size();
  int64_t* result = new int64_t[*output_length];
  for (size_t i = 0; i < *output_length; i++) {
    result[i] = decoded[i];
  }

  return result;
}