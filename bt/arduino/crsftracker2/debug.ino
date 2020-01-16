void writeUINT8(char mystr[], char c[], int x) {
  Serial.print(mystr);
  Serial.print(" ");
  Serial.print((unsigned int) (c[x]));
  Serial.print(" ");
}

void showMessage(int id, char prefix[], char c[], int siz) {
  
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
      writeUINT32GPS("lat", c, 2);
      writeUINT32("lat", c, 2);
      writeUINT32GPS("lon", c, 6);
      writeUINT32("lon", c, 6);
      writeUINT16("altitude", c, 10);
      writeUINT16("speed", c, 12);
      writeUINT16("ground_course", c, 14);
      Serial.println("");
      writeRaw(c, siz);
    }
    
    if (id == 107) {
      Serial.print(prefix);
      Serial.print("MSP_COMP_GPS ");
      writeUINT16("distanceToHome", c, 0);
      writeUINT16("directionToHome", c, 2);
      writeUINT8("update", c, 4);
      Serial.println("");
    }
    if (id == 110) {
      Serial.print(prefix);
      Serial.print("MSP_ANALOG ");
      writeUINT8("vbat", c, 0);
      writeUINT16("intPowerMeterSum", c, 1);
      writeUINT16("rssi", c, 3);
      writeUINT16("amperage", c, 5);
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

void writeRaw(char c[], int siz) {
  for (int i = 0; i < siz; i++) {
    writeByteAndSpace(c[i]);
  }
  Serial.println("");
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

void writeUINT32GPS(char mystr[], char c[], int x) {
  Serial.print(mystr);
  Serial.print(" ");
  Serial.print(((unsigned int) (c[x + 3] * 256* 256* 256 + c[x + 2] * 256 * 256 + c[x + 1] * 256 + c[x])) / 10000000.0);
  Serial.print(" ");
}
void writeUINT32GPS2(char mystr[], char c[], int x) {
  Serial.print(mystr);
  Serial.print(" ");
  Serial.print(((unsigned int) (c[x] * 256* 256* 256 + c[x + 1] * 256 * 256 + c[x + 2] * 256 + c[x + 3])) / 10000000.0);
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

void debugData() {
    Serial.print("h_fix: ");
    Serial.print(home_gps_fix);
    Serial.print(" h_sat: ");
    Serial.print(home_num_sat);
    Serial.print(" h_lat: ");
    Serial.print(home_lat);
    Serial.print(" h_lon: ");
    Serial.print(home_lon);    
    Serial.print(" h_alt: ");
    Serial.print(home_alt);    

    Serial.print(" u_fix: ");
    Serial.print(uav_gps_fix);
    Serial.print(" u_sat: ");
    Serial.print(uav_num_sat);
    Serial.print(" u_lat: ");
    Serial.print(uav_lat);
    Serial.print(" u_lon: ");
    Serial.print(uav_lon);    
    Serial.print(" u_alt: ");
    Serial.print(uav_alt);        

    Serial.print(" h_magx: ");
    Serial.print(home_magx);
    Serial.print(" h_magy: ");
    Serial.print(home_magy);
    Serial.print(" h_magz: ");
    Serial.print(home_magz);    

    Serial.print(" h_vbat: ");
    Serial.print(home_vbat);   
    
    Serial.println("");
}
