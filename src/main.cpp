#include <Arduino.h>
#include <WiFi.h>
#include <Robonomics.h>

#define WIFI_SSID "Semen-2 g"
#define WIFI_PASSWORD "Mohave-2021"

#define ROBONOMICS_HOST "kusama.rpc.robonomics.network"
#define PREDEFINED_ADDRESS "4DCZ7M6cBpEkpsa7FqETcTVw8dMefxF47cxq3ZEAhJEFYr6d" 

Robonomics robonomics;

void fetchDatalog(const char* address, int index) {
  Serial.println("Fetching datalog...");
  
  // Create JSON message for the request
  String method = "datalog_record";
  JSONVar params;
  params[0] = address;
  params[1] = index;
  
  // Create request
  BlockchainUtils blockchainUtils;
  blockchainUtils.setup(ROBONOMICS_HOST);
  String requestMessage = blockchainUtils.createWebsocketMessage(method, params);
  
  // Send request and get result
  JSONVar result = blockchainUtils.rpcRequest(requestMessage);
  
  // Display result
  if (result.hasOwnProperty("result")) {
    Serial.println("Datalog record found:");
    Serial.println(JSON.stringify(result["result"]));
    
    if (result["result"].hasOwnProperty("data")) {
      String dataContent = (const char*)result["result"]["data"];
      Serial.println("Data content:");
      Serial.println(dataContent);
    }
  } // So far I always get an error 
  else if (result.hasOwnProperty("error")) {
    Serial.println("Error fetching datalog:");
    Serial.println(JSON.stringify(result["error"]));
  } else {
    Serial.println("Unknown response format");
  }
  
  blockchainUtils.disconnect();
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
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  fetchDatalog(PREDEFINED_ADDRESS, 1);
}

void loop() {
  delay(1000);
}