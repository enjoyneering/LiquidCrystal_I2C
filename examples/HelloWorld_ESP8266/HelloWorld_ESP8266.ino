/***************************************************************************************************/
/*
  PCF8574 chip uses I2C bus to communicate, 2 pins are required to interface

  Connect PCF8574 to pins :  SDA     SCL
  Uno, Mini, Pro:            A4      A5
  Mega2560, Due:             20      21
  Leonardo:                  2       3
  ATtiny85:                  0(5)    2/A1(7) (ATTinyCore  - https://github.com/SpenceKonde/ATTinyCore
                                              & TinyWireM - https://github.com/SpenceKonde/TinyWireM)
  ESP8266 ESP-xx:            ANY     ANY     (ESP8266Core - https://github.com/esp8266/Arduino)
  NodeMCU 1.0:               ANY     ANY     (D2 & D1 by default)

  NOTE: - ESP8266 ESP-xx needs ~350mA..800mA power supply, to avoid ESP8266 reset during power-up add
          the following components between the Vcc & GND rails.
        - Connect a large capacitor ~470uF x 10v across the Vcc & GND to minimize voltage fluctuations
        - Connect a small 0.1uF decoupling capacitor across the Vcc & GND, very close to the ESP pins
        - Skipping thouse components will cause the ESP8266 resets,
          then missing.
*/
/***************************************************************************************************/
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define COLUMS 20
#define ROWS   4

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

void setup()
{
  Serial.begin(115200);

  while (lcd.begin(COLUMS, ROWS, LCD_5x8DOTS, D2, D1) != 1) //colums - 20, rows - 4, pixels - LCD_5x8DOTS, SDA - D2, SCL - D1
  {
    Serial.println("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal.");
    delay(5000);
  }
  lcd.print("PCF8574 is OK...");

  lcd.setCursor(0, 1);           //set 1-st colum & 2-nd row. NOTE: 1-st colum & row started at zero
  lcd.print("Hello world!");
  lcd.setCursor(0, 2);           //set 1-st colum & 3-rd row. NOTE: 1-st colum & row started at zero
  lcd.print("Random number:");
}


void loop()
{
  lcd.setCursor(14, 2);         //set 15-th colum & 3-rd  row. NOTE: 1-st colum & row started at zero
  lcd.print(random(10, 100));

  delay(1000);
}
