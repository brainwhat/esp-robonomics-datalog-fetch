#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"  

const char* serverUrl = "http://polkadot.rpc.robonomics.network/rpc/";
HTTPClient http;

// Function makes POST request to blockchain using given parameters
String rpc_request(const char* method, const char* params[], int paramsCount, int request_id = 0) {
  String response = "";
  
  if (WiFi.status() == WL_CONNECTED) {
    if (http.begin(serverUrl)) {
      http.addHeader("Content-Type", "application/json");
      
      // Build JSON payload without the params
      String jsonBody = "{\"jsonrpc\": \"2.0\", \"id\":" + String(request_id) + ", \"method\":\"" + method + "\", \"params\":[";
      
      // Append all the params
      for (int i = 0; i < paramsCount; i++) {
        jsonBody += "\"" + String(params[i]) + "\"";
        if (i < paramsCount - 1) {
          jsonBody += ",";
        }
      }
      
      jsonBody += "]}";
      
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
  } else {
    Serial.println("WiFi not connected");
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

  const char* params[] = {"0x92972a1d208817aa0e44c51623d2d2f938fd5ac7b43b1f204c4541ac6563f06392899a30d206c34d9be6255f056e1e8fb1900ed96d872e11caa8b76a01279c2cc32784c33d3c2fc3", "0xa9d22d9b82d8f511b88694208dcccdc733d2955a80c2b2c8d181a14e5209560f"};
  String result = rpc_request("state_getStorageAt", params, 2);

  // For this one the response is
  // {"jsonrpc":"2.0","result":"0x0014","id":0}
  
}

void loop() {
  delay(5000);
}