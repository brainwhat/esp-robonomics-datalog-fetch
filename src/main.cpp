#include <Arduino.h>
#include <string.h>
#include <stdio.h>

// Включаем Rust библиотеку 
extern "C" { 
  #include "rust_scale_codec_wrapper.h"
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Создаем строку, которую будем кодировать/декодировать
  const char *original_string = "Hello, Rust";
  Serial.printf("Original string: '%s'\n", original_string);

  // 1. Кодируем строку
  ScaleEncodedResult encoded_result = encode_string_to_scale(original_string);

  if (encoded_result.ptr == NULL) {
    Serial.printf("Error during encoding");
    while(1) { delay(1000); } // Halt
    return;
  }

  Serial.printf("Encoding successful. Length: %zu\n", encoded_result.len);

  // Выведем закодированную строку как последовательность байтов
  char hex_buffer[encoded_result.len * 2 + 1]; 
  hex_buffer[encoded_result.len * 2] = '\0';
  for (size_t i = 0; i < encoded_result.len; ++i) {
      sprintf(&hex_buffer[i*2], "%02x", encoded_result.ptr[i]);
  }
  Serial.printf("Encoded data (hex): %s\n", hex_buffer);


  // 2. Декодируем строку
  char *decoded_c_string = decode_scale_to_string(encoded_result.ptr, encoded_result.len);

  if (decoded_c_string == NULL) {
    Serial.printf("Error during decoding");
    free_scale_encoded_result(encoded_result); // Освободим выделенную память
    return;
  }

  Serial.printf("Decoded string: '%s'\n", decoded_c_string);

  // 4. Освободим всю освобожденную память
  Serial.printf("Freeing encoded result memory...\n");
  free_scale_encoded_result(encoded_result); 

  Serial.printf("Freeing decoded string memory...\n");
  free_decoded_string(decoded_c_string);   
}

void loop() {
  delay(1000); 
}
