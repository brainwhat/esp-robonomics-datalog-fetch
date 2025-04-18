#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
// #include <scale.h>
#include "secrets.h"  

const char* serverUrl = "http://polkadot.rpc.robonomics.network/rpc/";
HTTPClient http;

String rpc_request(const char* method, const char* params[], int paramsCount, int request_id = 0) {
  String response = "";
  
    if (http.begin(serverUrl)) {
      http.addHeader("Content-Type", "application/json");
      
      // Use ArduinoJSON to create the request payload
      DynamicJsonDocument jsonDoc(1024); 
      
      // Add single instance elements
      jsonDoc["jsonrpc"] = "2.0";
      jsonDoc["id"] = request_id;
      jsonDoc["method"] = method;
      
      // Create and populate params array
      JsonArray paramsArray = jsonDoc.createNestedArray("params");
      for (int i = 0; i < paramsCount; i++) {
        paramsArray.add(params[i]);
      }
      
      // Serialize JSON to string
      String jsonBody;
      serializeJson(jsonDoc, jsonBody);
      
      int httpResponseCode = http.POST(jsonBody);
      
      if (httpResponseCode > 0) {
        response = http.getString();
        Serial.println("Response code: " + String(httpResponseCode));
        Serial.println("Response: " + response);
      } else {
        Serial.println("Error: " + String(httpResponseCode));
      }
      
      http.end();
    } else {
      Serial.println("Failed to connect to server");
    }
  
  return response;
}

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Example usage of the new rpc_request function
  const char *params[] = {"0x92972a1d208817aa0e44c51623d2d2f938fd5ac7b43b1f204c4541ac6563f06392899a30d206c34d9be6255f056e1e8fb1900ed96d872e11caa8b76a01279c2cc32784c33d3c2fc3", "0xa9d22d9b82d8f511b88694208dcccdc733d2955a80c2b2c8d181a14e5209560f"};
  String result = rpc_request("state_getStorageAt", params, 2);
  
  DynamicJsonDocument dest(256);
  DeserializationError error = deserializeJson(dest, result);

 if (error) {
   Serial.print(F("deserializeJson() failed: "));
   Serial.println(error.f_str());
   return;
 }

 const char* res= dest["result"];
 Serial.println(res);
}

void loop() {
  delay(5000);
}