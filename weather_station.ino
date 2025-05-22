#include <Arduino.h>
#include <U8g2lib.h>
#include <RTClib.h>
#include <Adafruit_BME280.h>
#include "Si115X.h"
#include <Wire.h>
#include <HardwareSerial.h>

#define I2C_SDA 32
#define I2C_SCL 33

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
RTC_DS3231 rtc;
Adafruit_BME280 bme;

HardwareSerial pmsSensor(2);

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);
  
  pmsSensor.begin(9600, SERIAL_8N1, 16, 17);
  Serial.begin(115200);
  
  if (!rtc.begin()) {
    Serial.println("RTC init fail");
    while (1)
      delay(10);
  }

  if (!bme.begin(0x76)) {
    Serial.println("BME init fail");
    while (1)
      delay(10);
  }

  if (!u8g2.begin()) {
    Serial.println("OLED init fail");
    while (1)
      delay(10);
  }

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_t0_11_tf); // 12 y difference seems to be good with this font
}

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

pms5003data data;

boolean readPMSdata() {
  if (pmsSensor.available() < 32) {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start byte
  if (pmsSensor.peek() != 0x42) {
    pmsSensor.read();
    return false;
  }
  
  // Now read all 32 bytes
  if (pmsSensor.available() < 32) {
    return false;
  }
  
  uint8_t buffer[32];
  pmsSensor.readBytes(buffer, 32);
  
  // Calculate checksum
  uint16_t sum = 0;
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
  
  // Put it into a nice struct
  memcpy((void *)&data, (void *)buffer_u16, 30);
  
  if (sum != data.checksum) {
    return false;
  }
  
  return true;
}

void loop() {
  DateTime now = rtc.now();
  char time[20];
  snprintf(time, sizeof(time), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

  double temp = bme.readTemperature(), pres = bme.readPressure(), hum = bme.readHumidity();
  char temp_str[20], bme_str[20], hum_str[20];
  snprintf(temp_str, sizeof(temp_str), "T: %.1fC", temp);
  snprintf(bme_str, sizeof(bme_str), "P: %.1fhPa", pres);
  snprintf(hum_str, sizeof(hum_str), "H: %.1f%%", hum);

  char pm10_str[20], pm25_str[20], pm100_str[20];
  if(readPMSdata()){
    snprintf(pm10_str, sizeof(pm10_str), "PM 1.0: %d", data.pm10_standard);
    snprintf(pm25_str, sizeof(pm25_str), "PM 2.5: %d", data.pm25_standard);
    snprintf(pm100_str, sizeof(pm100_str), "PM 10: %d", data.pm100_standard);
  }

  u8g2.clearBuffer();
  u8g2.drawStr(0, 8, time);
  u8g2.drawStr(0, 20, temp_str);
  u8g2.drawStr(0, 32, bme_str);
  u8g2.drawStr(0, 44, hum_str);
  u8g2.drawStr(0, 56, pm10_str);
  u8g2.sendBuffer();

  delay(1000);
}