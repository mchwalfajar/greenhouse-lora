#include <loraid.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_SHT31.h>

Adafruit_SHT31 sht31 = Adafruit_SHT31();                          
BH1750 lightMeter;
int soilPin = A0;

float temp,hum,lux,soil;

    long interval = 120000;                                       // 2m interval to send message
    long previousMillis = 0;                                      // will store last time message sent          
    unsigned int counter = 0;                                     // message counter  

void setup () {
  lora.init();                                                    // setup loraid access
  lora.setDeviceClass(CLASS_A);                                   // set LoRaWAN Class
  lora.setDataRate(2);                                            // set data rate    
  lora.setAccessKey("181b1730efb9f453:1860de96ede6acb0");         // Antares account key
  lora.setDeviceId("a0031518");                                   // LoRa device address
  
  lightMeter.begin(0x23);
  sht31.begin(0x44);
}

void loop() {
  temp = sht31.readTemperature();                                  // temperature data
  hum = sht31.readHumidity();                                      // humidity data
  lux = lightMeter.readLightLevel();                               // lux data  
  soil = analogRead(soilPin);
  soil = map(soil,2048,10,0,100);                                  // soil data

  unsigned long currentMillis = millis();
  char dataSend[50];
  int recvStatus = 1;

  /* check if more than 10s */
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis; 

    String data = "T:" + String(temp) + "," + "H:" + String(hum) + "," + "L:" + String(lux) + "," + "S:" + String(soil);
    data.toCharArray(dataSend,50);  
    lora.sendToAntares((unsigned char *)dataSend,strlen(dataSend),0);
  }

  recvStatus = lora.readData(dataSend);
  if(recvStatus) {
    Serial.println(dataSend);
  }
    
  lora.update();                                                      // check LoRa RX
}
