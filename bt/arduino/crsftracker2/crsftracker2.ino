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
char c[100];
String MACadd = "AA:BB:CC:11:22:33";
// 00043E9CB464
uint8_t address[6]  = {0x00, 0x04, 0x3E, 0x9C, 0xB4, 0x64};
//uint8_t address[6]  = {0x00, 0x1D, 0xA5, 0x02, 0xC3, 0x22};
String name = "anikoviphone";
char *pin = "1234"; //<- standard pin would be provided by default
bool connected;

int stepw = 1000;
int steps = 4;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  //SerialBT.setPin(pin);
  SerialBT.begin("ESP32test", true); 
  Serial.println("The device started, now you can pair it with bluetooth!");
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
  // disconnect() may take upto 10 secs max
//  if (SerialBT.disconnect()) {
//    Serial.println("Disconnected Succesfully!");
//  }
//  // this would reconnect to the name(will use address, if resolved) or address used with connect(name/address).
//  SerialBT.connect();
  
}

void sendCodeToFC(int code) {
  Serial2.write('$');
  Serial2.write('M');
  Serial2.write('<');
  Serial2.write(0);
  Serial2.write(code);
  Serial2.write(code);
}

void sendCodeToBT(int code) {
  SerialBT.write('$');
  SerialBT.write('M');
  SerialBT.write('<');
  SerialBT.write(0);
  SerialBT.write(code);
  SerialBT.write(code);
}

void readFromFC() {
  if (Serial2.available()) {
    char c1 = Serial2.read();
    char c2 = Serial2.read();
    char c3 = Serial2.read();
    char len = Serial2.read();
    char id = Serial2.read();
    for (int i = 0; i < len; i++) {
      c[i] = Serial2.read();
    }
    char checksum = Serial2.read();

    showMessage(id, "ground: ", c);
  }
}

char readBT() {
  while (!SerialBT.available()) {
    delay(1);
  }
  return SerialBT.read();
}
void readFromBT() {

  if (SerialBT.available()) {
    char c1 = readBT();
    char c2 = readBT();
    char c3 = readBT();
    char len = readBT();
    char id = readBT();
    for (int i = 0; i < len; i++) {
      c[i] = readBT();
    }
    char checksum = readBT();

    showMessage(id, "plane:  ", c);
  }
}

void showMessage(int id, char prefix[], char c[]) {
  
    if (id == 100) {
      Serial.print(prefix);
      Serial.print("MSP_IDENT ");
      writeUINT8("version", c, 0);
      writeUINT8("multitype", c, 1);
      writeUINT8("msp_version", c, 2);
      writeUINT32("capability", c, 3);
      Serial.println("");
    }

    if (id == 106) {
      Serial.print(prefix);
      Serial.print("MSP_RAW_GPS ");
      writeUINT8("fix", c, 0);
      writeUINT8("numSat", c, 1);
      writeUINT32("lat", c, 2);
      writeUINT32("lon", c, 6);
      writeUINT16("altitude", c, 10);
      writeUINT16("speed", c, 12);
      writeUINT16("ground_course", c, 14);
      Serial.println("");
    }
    
    if (id == 107) {
      Serial.print(prefix);
      Serial.print("MSP_COMP_GPS ");
      writeUINT16("distanceToHome", c, 0);
      writeUINT16("directionToHome", c, 2);
      writeUINT8("update", c, 4);
      Serial.println("");
    }
    if (id == 102) {
      Serial.print(prefix);
      Serial.print("MSP_IDENT ");
      writeINT16("accx", c, 0);
      writeINT16("accy", c, 2);
      writeINT16("accz", c, 4);
      writeINT16("gyrx", c, 6);
      writeINT16("gyry", c, 8);
      writeINT16("gyrz", c, 10);
      writeINT16("magx", c, 12);
      writeINT16("magy", c, 14);
      writeINT16("magz", c, 16);
      Serial.println("");
    }

}

void writeUINT8(char mystr[], char c[], int x) {
  Serial.print(mystr);
  Serial.print(" ");
  Serial.print((unsigned int) (c[x]));
  Serial.print(" ");
}

void writeINT8(char mystr[], char c[], int x) {
  Serial.print(mystr);
  Serial.print(" ");
  Serial.print((int) (c[x]));
  Serial.print(" ");
}

void writeUINT16(char mystr[], char c[], int x) {
  Serial.print(mystr);
  Serial.print(" ");
  Serial.print((unsigned int) (c[x + 1] * 256 + c[x]));
  Serial.print(" ");
}

void writeINT16(char mystr[], char c[], int x) {
  Serial.print(mystr);
  Serial.print(" ");
  Serial.print((int) (c[x + 1] * 256 + c[x]));
  Serial.print(" ");
}

void writeUINT32(char mystr[], char c[], int x) {
  Serial.print(mystr);
  Serial.print(" ");
  Serial.print((unsigned int) (c[x + 3] * 256* 256* 256 + c[x + 2] * 256 * 256 + c[x + 1] * 256 + c[x]));
  Serial.print(" ");
}

void writeINT32(char mystr[], char c[], int x) {
  Serial.print(mystr);
  Serial.print(" ");
  Serial.print((int) (c[x + 3] * 256* 256* 256 + c[x + 2] * 256 * 256 + c[x + 1] * 256 + c[x]));
  Serial.print(" ");
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
void loop() {
  if (first == 1) {
    sendCodeToFC(100);
    sendCodeToBT(100);
    first=2;
  }
  if (first == 2 && (millis() % (steps * stepw)) / stepw > 0) {
    sendCodeToFC(106);
    sendCodeToBT(106);
    first=3;
  }
  if (first == 3 && (millis() % (steps * stepw)) / stepw > 1) {
    sendCodeToFC(107);
    sendCodeToBT(107);
    first=4;
  }
  if (first == 4 && (millis() % (steps * stepw)) / stepw > 2) {
    sendCodeToFC(102);
    sendCodeToBT(102);
    first=5;
  }
  if (first == 5 && (millis() % (steps * stepw)) / stepw == 0) {
    first=1;
  }
//  if (Serial.available()) {
//    Serial2.write(Serial.read());
//  }
  readFromFC();
  readFromBT();
  delay(20);
}
