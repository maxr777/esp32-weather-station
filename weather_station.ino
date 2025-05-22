#include <Arduino.h>
#include <U8g2lib.h>
#include <RTClib.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SI1145.h>
#include <Wire.h>

#define I2C_SDA 32
#define I2C_SCL 33

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
RTC_DS3231 rtc;
Adafruit_BME280 bme;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);
  
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

void loop() {
  DateTime now = rtc.now();
  char time[20];
  snprintf(time, sizeof(time), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

  double temp = bme.readTemperature(), pres = bme.readPressure(), hum = bme.readHumidity();
  char temp_str[20], bme_str[20], hum_str[20];
  snprintf(temp_str, sizeof(temp_str), "T: %.1fC", temp);
  snprintf(bme_str, sizeof(bme_str), "P: %.1fhPa", pres);
  snprintf(hum_str, sizeof(hum_str), "H: %.1f%%", hum);

  u8g2.clearBuffer();
  u8g2.drawStr(0, 8, time);
  u8g2.drawStr(0, 20, temp_str);
  u8g2.drawStr(0, 32, bme_str);
  u8g2.drawStr(0, 44, hum_str);
  u8g2.sendBuffer();

  delay(1000);
}