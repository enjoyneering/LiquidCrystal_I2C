/******************************************************************************/
/*
  PCF8574 chip uses I2C bus to communicate, 2 pins are required to  
  interface.

  Connect PCF8574 to pins :  SDA     SCL
  Uno, Mini, Pro:            A4      A5
  Mega2560, Due:             20      21
  Leonardo:                  2       3
  ATtiny85:                  0(5)    2/A1(7) (TinyWireM   - https://github.com/SpenceKonde/TinyWireM & ATTinyCore - https://github.com/SpenceKonde/ATTinyCore)
  ESP8266 ESP-xx:            ANY     ANY     (ESP8266Core - https://github.com/esp8266/Arduino)
  NodeMCU 1.0:               ANY     ANY     (D1 & D2 by default)
*/
/******************************************************************************/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

uint8_t bell[8]    = {0x04, 0x0E, 0x0E, 0x0E, 0x1F, 0x00, 0x04, 0x00};
uint8_t note[8]    = {0x01, 0x03, 0x05, 0x09, 0x0B, 0x1B, 0x18, 0x00};
uint8_t clock[8]   = {0x00, 0x0E, 0x15, 0x17, 0x11, 0x0E, 0x00, 0x00};
uint8_t heart[8]   = {0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00};
uint8_t duck[8]    = {0x00, 0xCC, 0x1D, 0x0F, 0x0F, 0x06, 0x00, 0x00};
uint8_t check[8]   = {0x00, 0x01, 0x03, 0x16, 0x1C, 0x08, 0x00, 0x00};
uint8_t lock[8]    = {0x0E, 0x11, 0x11, 0x1F, 0x1B, 0x1B, 0x1F, 0x00};
uint8_t battery[8] = {0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00};

/* some usefull icons located in the lcd ROM. NOTE: ROM content may vary, use "romPattern.ino" to find out what is in your ROM */
uint8_t arrow_left  = 0x7E; //see p.9  of GDM2004D datasheet
uint8_t arrow_right = 0x7F; //see p.9  of GDM2004D datasheet
uint8_t degree      = 0xDF; //see p.9  of GDM2004D datasheet
uint8_t alfa        = 0xE0; //see p.9  of GDM2004D datasheet
uint8_t omega       = 0xF4; //see p.9  of GDM2004D datasheet
uint8_t sum         = 0xF6; //see p.9  of GDM2004D datasheet
uint8_t pi          = 0xF7; //see p.9  of GDM2004D datasheet
uint8_t division    = 0xFD; //see p.9  of GDM2004D datasheet
uint8_t micro       = 0xE4; //see p.9  of GDM2004D datasheet
uint8_t sq_root     = 0xE8; //see p.9  of GDM2004D datasheet

uint8_t icon;
uint8_t colums = 20;
uint8_t rows   = 4;
  
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

void setup()
{
  Serial.begin(115200);
  Serial.flush();

  if (lcd.begin(colums, rows) != 1)
  {
    Serial.println("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,11,12,13,14,16 are legal.");
  }
  else
  {
    Serial.println("PCF8574 is OK...");
  }
  
  lcd.createChar(0, bell);
  lcd.createChar(1, note);
  lcd.createChar(2, clock);
  lcd.createChar(3, heart);
  lcd.createChar(4, duck);
  lcd.createChar(5, check);
  lcd.createChar(6, lock);
  lcd.createChar(7, battery);
}

void loop()
{
  lcd.setCursor(random(0, colums), random(0, rows));

  icon = random(0, 18);
  switch(icon)
  {
    case 0:
      lcd.write(icon);
      break;
    case 1:
      lcd.write(icon);
      break;
    case 2:
      lcd.write(icon);
      break;
    case 3:
      lcd.write(icon);
      break;
    case 4:
      lcd.write(icon);
      break;
    case 5:
      lcd.write(icon);
      break;
    case 6:
      lcd.write(icon);
      break;
    case 7:
      lcd.write(icon);
      break;
    case 8:
      lcd.write(arrow_left);
      break;
    case 9:
      lcd.write(arrow_right);
      break;
    case 10:
      lcd.write(degree);
      break;
    case 11:
      lcd.write(alfa);
      break;
    case 12:
      lcd.write(omega);
      break;
    case 13:
      lcd.write(sum);
      break;
    case 14:
      lcd.write(pi);
      break;
    case 15:
      lcd.write(division);
      break;
    case 16:
      lcd.write(micro);
      break;
    case 17:
      lcd.write(sq_root);
      break;
  }
  delay(500);
}
