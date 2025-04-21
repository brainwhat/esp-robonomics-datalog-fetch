#ifndef SCALE_DECODER_H
#define SCALE_DECODER_H

#include <Arduino.h>
#include <vector>

// Function declarations
size_t get_compact_length(uint8_t first_byte);
uint64_t decode_compact_integer(const uint8_t* input, size_t& offset);
std::vector<int64_t> decodeScaleData(const std::vector<uint8_t>& encodedData);
int64_t* decode_scale(const char* hexString, size_t* output_length);

#endif // SCALE_DECODER_H