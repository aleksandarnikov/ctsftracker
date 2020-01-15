//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Victor Tchistiak - 2019
//
//This example demostrates master mode bluetooth connection and pin 
//it creates a bridge between Serial and Classical Bluetooth (SPP)
//this is an extention of the SerialToSerialBT example by Evandro Copercini - 2018
//

#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
int first = 1;

String MACadd = "AA:BB:CC:11:22:33";
// 00043E9CB464
uint8_t address[6]  = {0x00, 0x04, 0x3E, 0x9C, 0xB4, 0x64};
//uint8_t address[6]  = {0x00, 0x1D, 0xA5, 0x02, 0xC3, 0x22};
String name = "anikoviphone";
char *pin = "1234"; //<- standard pin would be provided by default
bool connected;

void setup() {
  Serial.begin(115200);
  //SerialBT.setPin(pin);
  SerialBT.begin("ESP32test", true); 
//  Serial.println("The device started, now you can pair it with bluetooth!");
  //SerialBT.setPin(pin);
  Serial.println("The device started in master mode, make sure remote BT device is on!");
  
  // connect(address) is fast (upto 10 secs max), connect(name) is slow (upto 30 secs max) as it needs
  // to resolve name to address first, but it allows to connect to different devices with the same name.
  // Set CoreDebugLevel to Info to view devices bluetooth address and device names
//  connected = SerialBT.connect(name);
  connected = SerialBT.connect(address);

  if(connected) {
    Serial.println("Connected Succesfully!");
  } else {
    while(!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app."); 
    }
    Serial.println("Connected Succesfully!");
  }
//  // disconnect() may take upto 10 secs max
//  if (SerialBT.disconnect()) {
//    Serial.println("Disconnected Succesfully!");
//  }
//  // this would reconnect to the name(will use address, if resolved) or address used with connect(name/address).
//  SerialBT.connect();
  
}

void sendCode(int code) {
  SerialBT.write('$');
  SerialBT.write('M');
  SerialBT.write('<');
  SerialBT.write(0);
  SerialBT.write(code);
  SerialBT.write(code);
}

void loop() {
  if (first == 1) {
    Serial.println("");
    sendCode(100);
    first=2;
  }
  if (first == 2 && millis() > 10000) {
    Serial.println("");
    sendCode(106);
    first=3;
  }
  if (first == 3 && millis() > 20000) {
    Serial.println("");
    sendCode(107);
    first=4;
  }
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    char c = SerialBT.read();
    writeByteAndSpace(c);

  }
  delay(20);
}

void writeByteAndSpace(char c) {
    if (c / 16 >= 10) {
      Serial.write('A' + (c / 16) - 10);       
    } else {
      Serial.write('0' + (c / 16)); 
    }
    if ((c % 16) >= 10) {
      Serial.write('A' + (c % 16) - 10);       
    } else {
      Serial.write('0' + (c % 16)); 
    }
    Serial.write(32);
}
