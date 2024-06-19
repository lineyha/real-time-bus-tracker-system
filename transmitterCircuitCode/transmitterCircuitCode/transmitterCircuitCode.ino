#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "LoRa_E32.h"

TinyGPS gps;
SoftwareSerial ss(A1, A2); // TX,RX
SoftwareSerial mySerial(A4, A5); 
LoRa_E32 LoRaSS(&mySerial);
#define channel 420  // frequency value must be the same as other modules
#define addressToSendTo 2 // address of the LoRa to send the message to


struct Signal {
  byte longitude[4];
  byte latitude[4];
} data;
 

void setup()
{

  LoRaSS.begin();
  Serial.begin(115200);
  ss.begin(9600);
  
}

void loop()
{

  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;


    for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }


  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
      

    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
    Serial.println();
    Serial.println(flon,6);
    
    *(float*)(data.longitude) = flon;
    *(float*)(data.latitude) = flat;
    ResponseStatus rs = LoRaSS.sendFixedMessage(highByte(addressToSendTo), lowByte(addressToSendTo), channel, &data, sizeof(Signal));
  }
  
  gps.stats(&chars, &sentences, &failed);
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
}



