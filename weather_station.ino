#include <Arduino.h>
#include <U8g2lib.h>
#include <RTClib.h>
#include <Wire.h>

#define I2C_SDA 32
#define I2C_SCL 33

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
RTC_DS3231 rtc;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);
  
  Serial.begin(115200);
  
  if (!rtc.begin()) {
    Serial.println("RTC init fail");
    while (1);
  }

  if (!u8g2.begin()) {
    Serial.println("OLED init fail");
    while (1);
  }
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_t0_11_tf);
  // u8g2.drawStr(0, 24, "Hello World!");
  // u8g2.sendBuffer();
}

void loop() {
  DateTime now = rtc.now();
  char time[9];
  snprintf(time, sizeof(time), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

  u8g2.clearBuffer();
  u8g2.drawStr(0, 20, time);
  u8g2.sendBuffer();

  delay(1000);
}