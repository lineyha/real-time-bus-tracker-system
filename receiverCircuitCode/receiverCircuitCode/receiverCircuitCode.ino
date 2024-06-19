#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "LoRa_E32.h" 

#ifndef STASSID
#define STASSID "" // !! WiFi name
#define STAPSK ""  // !! WiFi password
#endif


LoRa_E32 LoRaSS(D2,D3);

int commaIndex;
int httpCode;
String postData;

struct Signal {
  byte longitude[4];
  byte latitude [4];
} data;





void setup() {
  Serial.begin(9600);
  LoRaSS.begin();
  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() { 

    while(LoRaSS.available()>1 && (WiFi.status() == WL_CONNECTED) ){
    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url

    http.begin(client, "http://yasirduman.com/post.php"); // HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n"); 


    ResponseStructContainer rsc = LoRaSS.receiveMessage(sizeof(Signal));
    struct Signal data = *(Signal*)rsc.data;
    rsc.close();

    Serial.println("Sifre doğru");
    Serial.print("Kordinat: ");
    Serial.println(*(float*)(data.longitude),6);
    Serial.print("Büyük sayı: ");
    Serial.println(*(float*)(data.latitude),6);
    String flon = String(*(float*)(data.longitude),6);
    String flat = String(*(float*)(data.latitude),6);
    postData = "{\"flat\":\""+flat+"\",\"flon\":\""+flon+"\",\"wifiID\":\""+1+"\"}";
    httpCode = http.POST(postData);

    if (httpCode > 0) {
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
         Serial.println("received payload:\n<<");
         Serial.println(payload);
         Serial.println(">>");
       }
     } else {
       Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
     }
  http.end();
  delay(500);  
  
  }
}
