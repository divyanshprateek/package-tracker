
#include <ESP8266WiFi.h>
#include <SPI.h>
#include "MFRC522.h"
#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>


ESP8266WiFiMulti WiFiMulti;


constexpr uint8_t RST_PIN = 0;         
constexpr uint8_t SS_PIN = 2;         // Configurable, see typical pin layout above
const int buzzer =  4;
String rfid="";

MFRC522 mfrc522(SS_PIN, RST_PIN);

//void dump_byte_array(byte *buffer, byte bufferSize);

void setup() {

   pinMode(buzzer, OUTPUT); 
     Serial.begin(115200);    // Initialize serial communications
     delay(250);

        
   
    WiFiMulti.addAP("visitor","bmsce1946");


  Serial.println(F("Booting...."));
  
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  
   
  Serial.println(F("Ready!"));
  Serial.println(F("======================================================")); 
  Serial.println(F("Scan for Card and print UID:"));
}






void loop() { 
      
  String url="https://10.60.1.19/tequedlabs_project1/inventory_loc.php/?location=Area1&rfid=";

         // Look for new cards
   if ( ! mfrc522.PICC_IsNewCardPresent()) {
      delay(50);
      return;
     }
     
      // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
     }

     String location="Entry";
     // Show some details of the PICC (that is: the tag/card)
     Serial.print(F("Card UID:"));

    for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    rfid+=String(mfrc522.uid.uidByte[i],HEX);
    }
    Serial.println();
    delay(1000);

    Serial.println();
     Serial.println(rfid);
     url+=rfid;
     Serial.print(url);

     rfid="";
     
     Serial.println();
      
   // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {
        Serial.println("wifi connected");
        HTTPClient http;       
        Serial.print("[HTTP] begin...\n");
        // configure traged server and url
        http.begin(url); //HTTPS
        

        Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
             Serial.println(payload);
            }
        } else {
           Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }
     else
     {
      Serial.println("wifi not connected");
     }
    delay(5000); 
  
}


