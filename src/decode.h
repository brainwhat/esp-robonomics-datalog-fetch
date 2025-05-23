#ifndef SCALE_DECODER_H
#define SCALE_DECODER_H

#include <Arduino.h>
#include <vector>
#include <stdint.h>
#include <algorithm>

class ScaleDecoder {
private:
    const char* data;
    size_t length;
    size_t offset;

public:
    ScaleDecoder(const char* _data, size_t _length) : data(_data), length(_length), offset(0) {}

    bool hasMore() const {
        return offset < length;
    }

    // Decode a compact integer
    uint64_t decodeCompactInteger() {
        if (offset >= length) {
            Serial.println("Error: Buffer overrun while decoding compact integer");
            return 0;
        }

        uint8_t mode = (uint8_t)(data[offset] & 0x03);
        uint64_t value = 0;

        switch (mode) {
            case 0: // single-byte mode (0-63)
                value = (uint8_t)(data[offset]) >> 2;
                offset += 1;
                break;
            
            case 1: // two-byte mode (64-16383)
                if (offset + 1 >= length) {
                    Serial.println("Error: Buffer overrun while decoding 2-byte compact integer");
                    return 0;
                }
                value = ((uint16_t)((uint8_t)(data[offset]) >> 2) | 
                        ((uint16_t)(uint8_t)(data[offset + 1]) << 6));
                offset += 2;
                break;
            
            case 2: // four-byte mode (16384-1073741823)
                if (offset + 3 >= length) {
                    Serial.println("Error: Buffer overrun while decoding 4-byte compact integer");
                    return 0;
                }
                value = ((uint32_t)((uint8_t)(data[offset]) >> 2) | 
                        ((uint32_t)(uint8_t)(data[offset + 1]) << 6) | 
                        ((uint32_t)(uint8_t)(data[offset + 2]) << 14) | 
                        ((uint32_t)(uint8_t)(data[offset + 3]) << 22));
                offset += 4;
                break;
            
            case 3: // Big integer mode
                {
                    uint8_t byteLen = ((uint8_t)(data[offset]) >> 2) + 4;
                    if (offset + byteLen >= length) {
                        Serial.println("Error: Buffer overrun while decoding big compact integer");
                        return 0;
                    }
                    
                    // For ESP32, limit to 8 bytes (64-bit)
                    if (byteLen > 8) {
                        Serial.println("Error: Integer too large for 64-bit representation");
                        offset += byteLen + 1;
                        return 0;
                    }
                    
                    value = 0;
                    for (int i = 0; i < byteLen; i++) {
                        value |= (uint64_t)(uint8_t)(data[offset + i + 1]) << (i * 8);
                    }
                    offset += byteLen + 1;
                }
                break;
        }

        return value;
    }

    // Decode a boolean
    bool decodeBool() {
        if (offset >= length) {
            Serial.println("Error: Buffer overrun while decoding boolean");
            return false;
        }
        bool result = ((uint8_t)(data[offset]) != 0);
        offset += 1;
        return result;
    }

    // Decode a fixed-width integer
    template<typename T>
    T decodeInteger() {
        const size_t size = sizeof(T);
        if (offset + size > length) {
            Serial.println("Error: Buffer overrun while decoding integer");
            return 0;
        }
        
        T result = 0;
        for (size_t i = 0; i < size; i++) {
            result |= (T)(uint8_t)(data[offset + i]) << (8 * i);
        }
        offset += size;
        return result;
    }

    // Shortcuts for common integer types
    uint8_t decodeU8() { return decodeInteger<uint8_t>(); }
    uint16_t decodeU16() { return decodeInteger<uint16_t>(); }
    uint32_t decodeU32() { return decodeInteger<uint32_t>(); }
    uint64_t decodeU64() { return decodeInteger<uint64_t>(); }
    int8_t decodeI8() { return decodeInteger<int8_t>(); }
    int16_t decodeI16() { return decodeInteger<int16_t>(); }
    int32_t decodeI32() { return decodeInteger<int32_t>(); }
    int64_t decodeI64() { return decodeInteger<int64_t>(); }

    // Decode a vector of integers
    template<typename T>
    std::vector<T> decodeIntegerVector() {
        std::vector<T> result;
        size_t length = decodeCompactInteger();
        
        result.reserve(length);
        for (size_t i = 0; i < length; i++) {
            result.push_back(decodeInteger<T>());
        }
        
        return result;
    }

    // Helper methods for common vector types
    std::vector<uint8_t> decodeU8Vector() { return decodeIntegerVector<uint8_t>(); }
    std::vector<uint16_t> decodeU16Vector() { return decodeIntegerVector<uint16_t>(); }
    std::vector<uint32_t> decodeU32Vector() { return decodeIntegerVector<uint32_t>(); }
    std::vector<uint64_t> decodeU64Vector() { return decodeIntegerVector<uint64_t>(); }
    std::vector<int8_t> decodeI8Vector() { return decodeIntegerVector<int8_t>(); }
    std::vector<int16_t> decodeI16Vector() { return decodeIntegerVector<int16_t>(); }
    std::vector<int32_t> decodeI32Vector() { return decodeIntegerVector<int32_t>(); }
    std::vector<int64_t> decodeI64Vector() { return decodeIntegerVector<int64_t>(); }

    // Get current offset
    size_t getOffset() const {
        return offset;
    }
};

#endif // SCALE_DECODER_H