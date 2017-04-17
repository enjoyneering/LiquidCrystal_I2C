/***************************************************************************************************/
/*
  written by : enjoyneering79
  sourse code: https://github.com/enjoyneering/

  This chip uses I2C bus to communicate, 2 pins are required to interface

  Connect chip to pins:    SDA     SCL
  Uno, Mini, Pro:          A4      A5
  Mega2560, Due:           20      21
  Leonardo:                2       3
  ATtiny85:                0(5)    2/A1(7) (ATTinyCore  - https://github.com/SpenceKonde/ATTinyCore
                                            & TinyWireM - https://github.com/SpenceKonde/TinyWireM)
  ESP8266 ESP-xx:          ANY     ANY     (ESP8266Core - https://github.com/esp8266/Arduino)
  NodeMCU 1.0:             ANY     ANY     (D2 & D1 by default)

*/
/***************************************************************************************************/
#include <TinyWireM.h>
#include <LiquidCrystal_I2C.h>

#define COLUMS 20
#define ROWS   4

uint8_t led = 1;  //connect led to ATtiny85 pin no.6 in series with 470 Ohm resistor

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);



void setup()
{
  pinMode(led, OUTPUT);

  while (lcd.begin(COLUMS, ROWS) != 1) //colums - 20, rows - 4
  {
    for (uint8_t i = 0; i > 3; i++)    //3 blinks, PCF8574/LCD is not connected or lcd pins declaration is wrong
    {
      digitalWrite(led, HIGH);
      delay(400);
      digitalWrite(led, LOW);
      delay(400);
    }
    delay(5000);
  }
  lcd.print("Hello world!");
  lcd.setCursor(0, 1);                 //set 1-st colum & 2-nd row. NOTE: 1-st colum & row started at zero
  lcd.print("Random number:");
}


void loop()
{
  lcd.setCursor(14, 1);                //set 15-th colum & 2-nd row. NOTE: 1-st colum & row started at zero
  lcd.print(random(10, 100));

  delay(1000);
}
