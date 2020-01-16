//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Victor Tchistiak - 2019
//
//This example demostrates master mode bluetooth connection and pin 
//it creates a bridge between Serial and Classical Bluetooth (SPP)
//this is an extention of the SerialToSerialBT example by Evandro Copercini - 2018
//

#include "BluetoothSerial.h"
#include <Metro.h>


BluetoothSerial SerialBT;
int first = 1;
char c[100];
//String MACadd = "AA:BB:CC:11:22:33";
// 00043E9CB464
uint8_t address[6]  = {0x00, 0x04, 0x3E, 0x9C, 0xB4, 0x64};
//uint8_t address[6]  = {0x00, 0x1D, 0xA5, 0x02, 0xC3, 0x22};
//String name = "anikoviphone";
//char *pin = "1234"; //<- standard pin would be provided by default
bool connected;

int home_gps_fix = 0;
int uav_gps_fix = 0;
int home_num_sat = 0;
int uav_num_sat = 0;

int32_t rel_alt = 0;
int32_t uav_alt = 0;
int32_t home_alt = 0;
int32_t home_lon;
int32_t home_lat;
int32_t uav_lon;
int32_t uav_lat;
int Bearing = 0;
int Elevation = 0;
int32_t home_dist = 0;
float lonScaleDown = 0.0;
int16_t home_bearing = 0;

int16_t home_magx = 0;
int16_t home_magy = 0;
int16_t home_magz = 0;
int8_t home_vbat = 0;

//Minimum distance in meters where it will stop moving servos.
#define DONTTRACKUNDER  5

int stepw = 1000;
int steps = 5;

//##### LOOP RATES
Metro loop1hz = Metro(1000); // 1hz loop
Metro loop10hz = Metro(100); // 10hz loop 
Metro loop20hz = Metro(50); // 20hz loop  
Metro loop50hz = Metro(20); // 50hz loop
Metro loopDisplay = Metro(5000); // 1hz loop

#define PAN_MAXPWM 1500     //max pan servo pwm value
#define PAN_MAXANGLE 90     //Max angle clockwise (on the right) relative to PAN_MAXPWM. 
#define PAN_MINPWM 1500     //min pan servo pwm valuemin pan servo pwm value
#define PAN_MINANGLE 90      //Max angle counter clockwise (on the left) relative to PAN_MINPWM.

#define TILT_MAXPWM 1500    //max tilt pwm value 
#define TILT_MAXANGLE 90    //max tilt angle considering 0° is facing toward.
#define TILT_MINPWM 1500    //min tilt pwm value
#define TILT_MINANGLE 0     //minimum tilt angle. Considering 0 is facing toward, a -10 value would means we can tilt 10° down.

#define VOLTAGE_RATIO 600   // Default multiplier for battery voltage reading * 100. This can eb adjustd later from the menu.

//Configuration stored in EEprom
struct config_t // 28 bytes
{
    int config_crc;
    int pan_minpwm;
    int pan_minangle; 
    int pan_maxpwm;
    int pan_maxangle;
    int tilt_minpwm;
    int tilt_minangle;
    int tilt_maxpwm;
    int tilt_maxangle;
    int baudrate;
    int telemetry;
    int bearing;
    uint8_t osd_enabled;
    uint8_t bearing_method;
    uint16_t voltage_ratio;
} configuration;

void setup() {

  configuration.pan_minpwm = PAN_MINPWM;
  configuration.pan_minangle = PAN_MINANGLE;
  configuration.pan_maxpwm = PAN_MAXPWM;
  configuration.pan_maxangle = PAN_MAXANGLE;
  configuration.tilt_minpwm = TILT_MINPWM;
  configuration.tilt_minangle = TILT_MINANGLE;
  configuration.tilt_maxpwm = TILT_MAXPWM;
  configuration.tilt_maxangle = TILT_MAXANGLE;
  configuration.baudrate = 6;
  configuration.telemetry = 0;
  configuration.bearing = 0;
  configuration.osd_enabled = 0;
  configuration.bearing_method = 1;
  configuration.voltage_ratio = VOLTAGE_RATIO;  // ratio*10
  
  Serial.begin(115200);
  Serial2.begin(115200);
  //SerialBT.setPin(pin);
  SerialBT.begin("ESP32test", true); 
//  Serial.println("The device started, now you can pair it with bluetooth!");
  //SerialBT.setPin(pin);
  Serial.println("The device started in master mode, make sure remote BT device is on!");
  
  // connect(address) is fast (upto 10 secs max), connect(name) is slow (upto 30 secs max) as it needs
  // to resolve name to address first, but it allows to connect to different devices with the same name.
  // Set CoreDebugLevel to Info to view devices bluetooth address and device names
//  connected = SerialBT.connect(name);

  
//  connected = SerialBT.connect(address);
//
//  if(connected) {
//    Serial.println("Connected Succesfully!");
//  } else {
//    while(!SerialBT.connected(10000)) {
//      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app."); 
//    }
//    Serial.println("Connected Succesfully!");
//  }



  
// disconnect() may take upto 10 secs max
//  if (SerialBT.disconnect()) {
//    Serial.println("Disconnected Succesfully!");
//  }
//  // this would reconnect to the name(will use address, if resolved) or address used with connect(name/address).
//  SerialBT.connect();


  loop1hz.reset();
  loop10hz.reset();
  loop20hz.reset();
  loop50hz.reset();
  loopDisplay.reset();
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
  char c1 = ' ';
  if (Serial2.available()) {
    while (Serial2.available() && c1 != '$') {
      c1 = readFC();
    }
    if (c1 != '$') {
      return;
    }

    char c2 = readFC();
    char c3 = readFC();
    char len = readFC();
    char id = readFC();
    for (int i = 0; i < len; i++) {
      c[i] = readFC();
    }
    char checksum = readFC();

    extractHomeData(id, c);
    //showMessage(id, "ground: ", c, len);
  }
}

void extractHomeData(int id, char c[]) {
  if (id == 106) {
    home_gps_fix = readUINT8(c, 0);
    home_num_sat = readUINT8(c, 1);
    home_lat = readUINT32(c, 2);
    home_lon = readUINT32(c, 6);
    home_alt = readUINT16(c, 10);
  }
  
  if (id == 102) {
    home_magx = readUINT16(c, 12);
    home_magy = readUINT16(c, 14);
    home_magz = readUINT16(c, 16);
  }

  if (id == 110) {
    home_vbat = readUINT8(c, 0);
  }
}

void extractUavData(int id, char c[]) {
  if (id == 106) {
    uav_gps_fix = readUINT8(c, 0);
    uav_num_sat = readUINT8(c, 1);
    uav_lat = readUINT32(c, 2);
    uav_lon = readUINT32(c, 6);
    uav_alt = readUINT16(c, 10);
  }
  
  if (id == 102) {

  }

  if (id == 110) {

  }
}

int16_t readUINT16(char c[], int x) {
  return ((unsigned int) (c[x + 1] * 256 + c[x]));
}

int8_t readUINT8(char c[], int x) {
  return ((unsigned int) (c[x]));
}

int32_t readUINT32(char c[], int x) {
  return ((unsigned int) (c[x + 3] * 256* 256* 256 + c[x + 2] * 256 * 256 + c[x + 1] * 256 + c[x]));
}

char readFC() {
  while (!Serial2.available()) {
    delay(1);
  }
  return Serial2.read();
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

    extractUavData(id, c);
    //showMessage(id, "plane:  ", c, len);
  }
}

void loop() {
//  if (loop1hz.check() == 1) {
//    sendCodeToFC(110);
//  }
  
  if (loop20hz.check() == 1) {
    readFromFC();
    readFromBT();   
  }

  if (loop10hz.check() == 1) {
    sendCodeToFC(106);
    sendCodeToBT(106);
    sendCodeToFC(102);
    sendCodeToFC(110);
  }

  if (loop50hz.check() == 1) {
    //update servos
    if((home_dist / 100) > DONTTRACKUNDER) {
        //servoPathfinder(Bearing,Elevation); // refresh servo 
    }
  }

  if (loopDisplay.check() == 1) {
    debugData();
  }
}


void antenna_tracking() {
// Tracking general function
    //only move servo if gps has a 3D fix, or standby to last known position.

    if (home_gps_fix && uav_gps_fix) {  
        rel_alt = uav_alt - home_alt; // relative altitude to ground in decimeters
        calc_tracking( home_lon, home_lat, uav_lon, uav_lat, rel_alt); //calculate tracking bearing/azimuth
        //set current GPS bearing relative to home_bearing
        if(Bearing >= home_bearing){
            Bearing -= home_bearing;
        } else { 
            Bearing += 360 - home_bearing;
        }
    } 
}

void calc_tracking(int32_t lon1, int32_t lat1, int32_t lon2, int32_t lat2, int32_t alt) {
    //calculating Bearing & Elevation  in degree decimal
    Bearing = calc_bearing(lon1,lat1,lon2,lat2);
    Elevation = calc_elevation(alt);
}

int16_t calc_bearing(int32_t lon1, int32_t lat1, int32_t lon2, int32_t lat2) {
    float dLat = (lat2 - lat1);
    float dLon = (float)(lon2 - lon1) * lonScaleDown;
    home_dist = sqrt(sq(fabs(dLat)) + sq(fabs(dLon))) * 1.113195; // home dist in cm.
    int16_t b = (int)round( -90 + (atan2(dLat, -dLon) * 57.295775));
    if(b < 0) b += 360; 
    return b; 
}

int16_t calc_elevation(int32_t alt) {
    float at = atan2(alt, home_dist);
    at = at * 57,2957795;
    int16_t e = (int16_t)round(at);
    return e;
}

void calc_longitude_scaling(int32_t lat) {
    float rads       = (abs((float)lat) / 10000000.0) * 0.0174532925;
    lonScaleDown = cos(rads);
}

void servoPathfinder(int angle_b, int angle_a){   // ( bearing, elevation )
//find the best way to move pan servo considering 0° reference face toward
    if (angle_b <= 180) {
      if ( configuration.pan_maxangle >= angle_b ) {
        //define limits
        if (angle_a <= configuration.tilt_minangle) {
        // checking if we reach the min tilt limit
            angle_a = configuration.tilt_minangle;
        } else if (angle_a >configuration.tilt_maxangle) {
            //shouldn't happend but just in case
            angle_a = configuration.tilt_maxangle; 
            }
        } else if ( configuration.pan_maxangle < angle_b ) {
        //relevant for 180° tilt config only, in case bearing is superior to pan_maxangle
        angle_b = 180+angle_b;
        if (angle_b >= 360) {
            angle_b = angle_b - 360;
        }
        // invert pan axis 
        if ( configuration.tilt_maxangle >= ( 180-angle_a )) {
            // invert pan & tilt for 180° Pan 180° Tilt config
            angle_a = 180-angle_a;
        }
        else if (configuration.tilt_maxangle < ( 180-angle_a )) {
            // staying at nearest max pos
            angle_a = configuration.tilt_maxangle;
        }
      }
    } else if ( angle_b > 180 ) {
      if( configuration.pan_minangle > 360-angle_b ) {
          if (angle_a < configuration.tilt_minangle) {
              // checking if we reach the min tilt limit
              angle_a = configuration.tilt_minangle;
          }
      } else if ( configuration.pan_minangle <= 360-angle_b ) {
          angle_b = angle_b - 180;
          if ( configuration.tilt_maxangle >= ( 180-angle_a )) {
              // invert pan & tilt for 180/180 conf
              angle_a = 180-angle_a;
          }
          else if (configuration.tilt_maxangle < ( 180-angle_a)) {
              // staying at nearest max pos
              angle_a = configuration.tilt_maxangle;
          }
      }
    }   
    Serial.print(angle_b);
    Serial.print(" ");
    Serial.print(angle_a);
    Serial.println(""); 
    //move_servo(pan_servo, 1, angle_b, configuration.pan_minangle, configuration.pan_maxangle);
    //move_servo(tilt_servo, 2, angle_a, configuration.tilt_minangle, configuration.tilt_maxangle);
}


//void retrieve_mag() {
//    // Retrieve the raw values from the compass (not scaled).
//    MagnetometerRaw raw = compass.ReadRawAxis();
//    // Retrieved the scaled values from the compass (scaled to the configured scale).
//    MagnetometerScaled scaled = compass.ReadScaledAxis();
//    //
//    // Calculate heading when the magnetometer is level, then correct for signs of axis.
//    float heading = atan2(scaled.YAxis, scaled.XAxis);
//
//    // Once you have your heading, you must then add your ‘Declination Angle’, which is the ‘Error’ of the magnetic field in your location.
//    // Find yours here: http://www.magnetic-declination.com/
//
//    float declinationAngle = MAGDEC / 1000; 
//    heading += declinationAngle;
//    
//    // Correct for when signs are reversed.
//    if (heading < 0)    heading += 2*PI;
//    
//    // Check for wrap due to addition of declination.
//    if (heading > 2*PI) heading -= 2*PI;
//    
//    // Convert radians to degrees for readability.
//    home_bearing = (int)round(heading * 180/M_PI);
//}
