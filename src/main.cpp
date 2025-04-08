#include <WiFi.h>
#include <HTTPClient.h>

#define WIFI_SSID "Semen-2 g"
#define WIFI_PASSWORD "Mohave-2021"

const char* serverUrl = "http://polkadot.rpc.robonomics.network/rpc/";

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

  HTTPClient http;
  
  if (http.begin(serverUrl)) {
    http.addHeader("Content-Type", "application/json");
    
    const char* jsonBody = 
      "{\"jsonrpc\": \"2.0\", \"id\":0, \"method\":\"rpc_methods\", \"params\":[]}";
    
    int httpResponseCode = http.POST(jsonBody);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error: " + String(httpResponseCode));
    }
    
    http.end();
  } else {
    Serial.println("Failed to connect to server");
  }
}

void loop() {
  delay(5000);
}