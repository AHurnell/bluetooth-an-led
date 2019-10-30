

#include "Particle.h"

const size_t SCAN_RESULT_MAX = 30;

//#define PRINT_SPEED 7005 // 20 ms between prints

//SYSTEM_MODE(MANUAL);//disables wifi

BleScanResult scanResults[SCAN_RESULT_MAX];
BleUuid fitnessMachineService(0x1826); //a uuid advertises the type of device, this example is a fitness machine
int led = D7; // Instead of writing D7 over and over again, we'll write led, this is the built in LED
float arbitraryInformation = 0.77777, interval=1000;//arbitraryInformation is data for broadcast, the interval is a period of time in ms
bool lightOn=false;
 BleAdvertisingData advData;

void setup()
{
  Particle.publish("set up", "started");
  updateAdvertisingData(false);
  Particle.publish("set up", "finished"+String(BLE_MAX_ADV_DATA_LEN));
  pinMode(led, OUTPUT);
}

void loop() {
  lightOn=true;
  digitalWrite(led, HIGH);//light turns on
  // We'll leave it on for 1 second... during which time we broadcast that status
  updateAdvertisingData(true);
  delay(interval);
  //delay(PRINT_SPEED);//optionally delay by longer
  lightOn=false;
  digitalWrite(led, LOW);//light turns off
  // We'll leave it off for 1 second... during which time we broadcast that status
  updateAdvertisingData(true);
  delay(interval);
  arbitraryInformation=arbitraryInformation+random(200)-100;// this arbitrary information represents data that for broadcast
}


void updateAdvertisingData(bool updateOnly)
{
  uint8_t buf[BLE_MAX_ADV_DATA_LEN];//BLE_MAX_ADV_DATA_LEN is a constant
  //uint8_t is an unsigned char, aka a number between 0 and 255
  size_t offset = 0;
  if(lightOn){
    buf[offset++] = 0x01;//if light is on this byte will be 01
  }
  else{
    buf[offset++] = 0x00;// think each time you do this you increast the "offset"
  }
  // Copy information into the buffer
  memcpy(&buf[offset], &arbitraryInformation, 4); // think this makes a sequence of bytes, it is vital.
  offset += 4;
  advData.appendLocalName("BC");//using this local name generates bytes "03:09:42:43" in the UART raw data. Using "BB" as a name generates "03:09:42:42" 
  advData.appendCustomData(buf, offset);

  if (updateOnly)
  {
    // Only update data
    BLE.setAdvertisingData(&advData);
    Particle.publish("Light status", String(lightOn)+" "+offset);//used for monitoring the buffer size
  }
  else
  {
    BLE.setAdvertisingInterval(interval);
    BLE.advertise(&advData);
    Particle.publish("something else", " is happening"+offset);
  }
}
