/***************************************************************************************************/
/*
   This is an Arduino library for HD44780, S6A0069, KS0066U, NT3881D, LC7985, ST7066, SPLC780,
   WH160xB, AIP31066, GDM200xD, ADM0802A LCD displays.

   Screens are operated in 4 bit mode over i2c bus with 8-bit I/O expander PCF8574x.
   Typical displays sizes: 8x2, 16x1, 16x2, 16x4, 20x2, 20x4 & etc.

   written by : enjoyneering79
   sourse code: https://github.com/enjoyneering/


   This chip uses I2C bus to communicate, specials pins are required to interface
   Board:                                    SDA                    SCL                    Level
   Uno, Mini, Pro, ATmega168, ATmega328..... A4                     A5                     5v
   Mega2560................................. 20                     21                     5v
   Due, SAM3X8E............................. 20                     21                     3.3v
   Leonardo, Micro, ATmega32U4.............. 2                      3                      5v
   Digistump, Trinket, ATtiny85............. 0/physical pin no.5    2/physical pin no.7    5v
   Blue Pill, STM32F103xxxx boards.......... PB7                    PB6                    3.3v/5v
   ESP8266 ESP-01........................... GPIO0/D5               GPIO2/D3               3.3v/5v
   NodeMCU 1.0, WeMos D1 Mini............... GPIO4/D2               GPIO5/D1               3.3v/5v
   ESP32.................................... GPIO21/D21             GPIO22/D22             3.3v

   Frameworks & Libraries:
   ATtiny Core           - https://github.com/SpenceKonde/ATTinyCore
   ESP32 Core            - https://github.com/espressif/arduino-esp32
   ESP8266 Core          - https://github.com/esp8266/Arduino
   ESP8266 I2C lib fixed - https://github.com/enjoyneering/ESP8266-I2C-Driver
   STM32 Core            - https://github.com/rogerclarkmelbourne/Arduino_STM32

   GNU GPL license, all text above must be included in any redistribution,
   see link for details  - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/

#include "LiquidCrystal_I2C.h"


/**************************************************************************/
/*
    LiquidCrystal_I2C()

    Constructor. Initializes the class variables, defines I2C address,
    LCD & PCF8574 pins.
*/
/**************************************************************************/  
LiquidCrystal_I2C::LiquidCrystal_I2C(PCF8574_address addr, uint8_t P0, uint8_t P1, uint8_t P2, uint8_t P3, uint8_t P4, uint8_t P5, uint8_t P6, uint8_t P7, backlightPolarity polarity)
{
  uint8_t PCF8574_TO_LCD[8] = {P0, P1, P2, P3, P4, P5, P6, P7}; //PCF8574 ports to LCD pins mapping array

  _PCF8574_address        = addr;
  _PCF8574_initialisation = true;
  _backlightPolarity      = polarity;

  /* maping LCD pins to PCF8574 ports */
  for (uint8_t i = 0; i < 8; i++)
  {
    switch(PCF8574_TO_LCD[i])
    {
      case 4:                   //RS pin
        _LCD_TO_PCF8574[7] = i;
        break;

      case 5:                   //RW pin
        _LCD_TO_PCF8574[6] = i;
        break;

      case 6:                   //EN pin
        _LCD_TO_PCF8574[5] = i;
        break;

      case 14:                  //D7 pin
        _LCD_TO_PCF8574[4] = i;
        break;

      case 13:                  //D6 pin
        _LCD_TO_PCF8574[3] = i;
        break;

      case 12:                  //D5 pin
        _LCD_TO_PCF8574[2] = i;
        break;

      case 11:                  //D4 pin
        _LCD_TO_PCF8574[1] = i;
        break;

      case 16:                  //BL pin
        _LCD_TO_PCF8574[0] = i;
        break;

      default:
        _PCF8574_initialisation = false; //safety check, make sure the declaration of lcd pins is right
        break;
    }
  }

  /* backlight control via PCF8574 */
  switch (_backlightPolarity)
  {
    case POSITIVE:
      _backlightValue = LCD_BACKLIGHT_ON;
      break;

    case NEGATIVE:
      _backlightValue = ~LCD_BACKLIGHT_ON;
      break;
  }

  _backlightValue <<= _LCD_TO_PCF8574[0];
}

/**************************************************************************/
/*
    begin()

    Initializes, resets & configures I2C bus & LCD

    NOTE:
    - Wire.endTransmission() returned code:
      0 - success
      1 - data too long to fit in transmit data16
      2 - received NACK on transmit of address
      3 - received NACK on transmit of data
      4 - other error
*/
/**************************************************************************/
#if defined(ESP8266)
bool LiquidCrystal_I2C::begin(uint8_t lcd_colums, uint8_t lcd_rows, lcd_font_size f_size, uint8_t sda, uint8_t scl)
{
  Wire.begin(sda, scl);
  Wire.setClock(100000UL);                            //experimental! ESP8266 i2c bus speed: 100kHz..400kHz/100000UL..400000UL, default 100000UL
  Wire.setClockStretchLimit(230);                     //experimental! default 230
#else
bool LiquidCrystal_I2C::begin(uint8_t lcd_colums, uint8_t lcd_rows, lcd_font_size f_size)
{
  Wire.begin();
  Wire.setClock(100000UL);                            //experimental! AVR i2c bus speed: AVR 31kHz..400kHz/31000UL..400000UL, default 100000UL
#endif

  if (_PCF8574_initialisation == false) return false; //safety check, make sure the declaration of lcd pins is right

  Wire.beginTransmission(_PCF8574_address);
  if (Wire.endTransmission() != 0) return false;      //safety check, make sure the PCF8574 is connected

  writePCF8574(PCF8574_ALL_LOW);                      //safety check, set all PCF8574 pins low

  _lcd_colums 	 = lcd_colums;
  _lcd_rows      = lcd_rows;
  _lcd_font_size = f_size;

  initialization();                                   //soft reset & 4-bit mode initialization

  return true;
}

/**************************************************************************/
/*
    clear()

    Clears display & move cursor to home position

    NOTE:
    - clear by fill it with space
    - cursor home position (0, 0)
    - command duration > 1.53 - 1.64ms
*/
/**************************************************************************/
void LiquidCrystal_I2C::clear(void)
{
  send(LCD_INSTRUCTION_WRITE, LCD_CLEAR_DISPLAY, LCD_CMD_LENGTH_8BIT);

  delay(LCD_HOME_CLEAR_DELAY);
}

/**************************************************************************/
/*
    home()

    Moves cursor position to home position

    NOTE:
    - sets DDRAM address to 0 in address counter, returns display to
      home position, but DDRAM contents remain unchanged
    - command duration > 1.53 - 1.64ms
*/
/**************************************************************************/
void LiquidCrystal_I2C::home(void)
{
  send(LCD_INSTRUCTION_WRITE, LCD_RETURN_HOME, LCD_CMD_LENGTH_8BIT);

  delay(LCD_HOME_CLEAR_DELAY);
}

/**************************************************************************/
/*
    setCursor()

    Sets cursor position

    NOTE:
    - cursor start position (0, 0)
    - cursor end   position (lcd_colums - 1, lcd_rows - 1)
    - DDRAM data/text is sent & received after this setting
*/
/**************************************************************************/
void LiquidCrystal_I2C::setCursor(uint8_t colum, uint8_t row)
{
  uint8_t row_address_offset[] = {0x00, 0x40, uint8_t(0x00 + _lcd_colums), uint8_t(0x40 + _lcd_colums)};

  /* safety check, cursor position & array are zero indexed */
  if (row   >= _lcd_rows)   row   = (_lcd_rows   - 1);
  if (colum >= _lcd_colums) colum = (_lcd_colums - 1);

  send(LCD_INSTRUCTION_WRITE, LCD_DDRAM_ADDR_SET | (row_address_offset[row] + colum), LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    noDisplay()

    Clears text from the screen

    NOTE:
    - text remains in DDRAM
*/
/**************************************************************************/
void LiquidCrystal_I2C::noDisplay(void)
{
  _displayControl &= ~LCD_DISPLAY_ON;

  send(LCD_INSTRUCTION_WRITE, LCD_DISPLAY_CONTROL | _displayControl, LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    display()

    Retrives text from DDRAM

    NOTE:
    - text remains in DDRAM
*/
/**************************************************************************/
void LiquidCrystal_I2C::display(void)
{
  _displayControl |= LCD_DISPLAY_ON;

  send(LCD_INSTRUCTION_WRITE, LCD_DISPLAY_CONTROL | _displayControl, LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    noCursor()

    Turns OFF the underline cursor
*/
/**************************************************************************/
void LiquidCrystal_I2C::noCursor(void)
{
  _displayControl &= ~LCD_UNDERLINE_CURSOR_ON;

  send(LCD_INSTRUCTION_WRITE, LCD_DISPLAY_CONTROL | _displayControl, LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    cursor()

    Turns ON the underline cursor
*/
/**************************************************************************/
void LiquidCrystal_I2C::cursor(void)
{
  _displayControl |= LCD_UNDERLINE_CURSOR_ON;

  send(LCD_INSTRUCTION_WRITE, LCD_DISPLAY_CONTROL | _displayControl, LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    noBlink()

    Turns OFF the blinking cursor
*/
/**************************************************************************/
void LiquidCrystal_I2C::noBlink(void)
{
  _displayControl &= ~LCD_BLINK_CURSOR_ON;

  send(LCD_INSTRUCTION_WRITE, LCD_DISPLAY_CONTROL | _displayControl, LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    blink()

    Turns ON the blinking cursor
*/
/**************************************************************************/
void LiquidCrystal_I2C::blink(void)
{
  _displayControl |= LCD_BLINK_CURSOR_ON;

  send(LCD_INSTRUCTION_WRITE, LCD_DISPLAY_CONTROL | _displayControl, LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    scrollDisplayLeft()

    Scrolls once current row with text on the display to the left

    NOTE:
    - call this function just before write() or print()
    - text grows from cursor to the left
*/
/**************************************************************************/
void LiquidCrystal_I2C::scrollDisplayLeft(void)
{
  send(LCD_INSTRUCTION_WRITE, LCD_CURSOR_DISPLAY_SHIFT | LCD_DISPLAY_SHIFT | LCD_SHIFT_LEFT, LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    scrollDisplayRight()

    Scrolls once current row with text on the display to the right

    NOTE:
    - call this function just before write() or print()
    - text & cursor grows together to the left from cursor position
*/
/**************************************************************************/
void LiquidCrystal_I2C::scrollDisplayRight(void)
{
  send(LCD_INSTRUCTION_WRITE, LCD_CURSOR_DISPLAY_SHIFT | LCD_DISPLAY_SHIFT | LCD_SHIFT_RIGHT, LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    leftToRight()

    Sets text direction from left to right
*/
/**************************************************************************/
void LiquidCrystal_I2C::leftToRight(void)
{
  _displayMode |= LCD_ENTRY_LEFT;

  send(LCD_INSTRUCTION_WRITE, LCD_ENTRY_MODE_SET | _displayMode, LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    rightToLeft()

    Sets text direction from right to left
*/
/**************************************************************************/
void LiquidCrystal_I2C::rightToLeft(void)
{
  _displayMode &= ~LCD_ENTRY_LEFT;

  send(LCD_INSTRUCTION_WRITE, LCD_ENTRY_MODE_SET | _displayMode, LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    autoscroll()

    Autoscrolls the text rightToLeft() or rightToRight() on the display

    NOTE:
    - whole text on the display shift when byte written, but cursor stays
    - same as scrollDisplayRight() or scrollDisplayLeft() but no need to
      call it the loop, just call it once it setup()
*/
/**************************************************************************/
void LiquidCrystal_I2C::autoscroll(void) 
{
  _displayMode |= LCD_ENTRY_SHIFT_ON;

  send(LCD_INSTRUCTION_WRITE, LCD_ENTRY_MODE_SET | _displayMode, LCD_CMD_LENGTH_8BIT);
}


/**************************************************************************/
/*
    noAutoscroll()

    Stops text autoscrolling on the display

    NOTE:
    - whole text on the display stays, cursor shifts when byte written
*/
/**************************************************************************/
void LiquidCrystal_I2C::noAutoscroll(void)
{
  _displayMode &= ~LCD_ENTRY_SHIFT_ON;

  send(LCD_INSTRUCTION_WRITE, LCD_ENTRY_MODE_SET | _displayMode, LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    createChar()

    Fills 64-bytes CGRAM, with custom characters from dynamic memory

    NOTE:
    - 8 patterns for 5x8DOTS display, write address 0..7
      & read address 0..7/8..15
    - 4 patterns for 5x10DOTS display, wrire address 0..3
      & read address 0..3/0..7
*/
/**************************************************************************/
void LiquidCrystal_I2C::createChar(uint8_t CGRAM_address, uint8_t *char_pattern)
{
  uint8_t CGRAM_capacity = 0;
  int8_t  font_size      = 0;

  /* set CGRAM capacity */
  switch (_lcd_font_size)
  {
    case LCD_5x8DOTS:
      CGRAM_capacity = 7;                                                                      //8 patterns, 0..7
      font_size      = 8;
      break;

    case LCD_5x10DOTS:
      CGRAM_capacity = 3;                                                                      //4 patterns, 0..3
      font_size      = 10;
      break;
  }

  /* safety check, make sure "CGRAM_address" never exceeds the "CGRAM_capacity" */
  if (CGRAM_address > CGRAM_capacity) CGRAM_address = CGRAM_capacity;

  send(LCD_INSTRUCTION_WRITE, LCD_CGRAM_ADDR_SET | (CGRAM_address << 3), LCD_CMD_LENGTH_8BIT); //set CGRAM address

  for (uint8_t i = 0; i < font_size; i++)
  {
    send(LCD_DATA_WRITE, char_pattern[i], LCD_CMD_LENGTH_8BIT);                                //write data from dynamic memory to CGRAM address
  }
}

/**************************************************************************/
/*
    createChar()

    Fills 64-bytes CGRAM, with custom characters from flash memory

    NOTE:
    - 8 patterns for 5x8DOTS display, write address 0..7
      & read address 0..7/8..15
    - 4 patterns for 5x10DOTS display, wrire address 0..3
      & read address 0..3/0..7
*/
/**************************************************************************/
#if defined (PROGMEM)
void LiquidCrystal_I2C::createChar(uint8_t CGRAM_address, const uint8_t *char_pattern)
{
  uint8_t CGRAM_capacity = 0;
  int8_t  font_size      = 0;

  /* set CGRAM capacity */
  switch (_lcd_font_size)
  {
    case LCD_5x8DOTS:
      CGRAM_capacity = 7;                                                                      //8 patterns, 0..7
      font_size      = 8;
      break;

    case LCD_5x10DOTS:
      CGRAM_capacity = 3;                                                                      //4 patterns, 0..3
      font_size      = 10;
      break;
  }

  /* safety check, make sure "CGRAM_address" never exceeds the "CGRAM_capacity" */
  if (CGRAM_address > CGRAM_capacity) CGRAM_address = CGRAM_capacity;

  send(LCD_INSTRUCTION_WRITE, LCD_CGRAM_ADDR_SET | (CGRAM_address << 3), LCD_CMD_LENGTH_8BIT); //set CGRAM address

  while (font_size-- > 0)
  {
    send(LCD_DATA_WRITE, pgm_read_byte(char_pattern++), LCD_CMD_LENGTH_8BIT);                  //write data from flash memory to CGRAM address
  }
}
#endif

/**************************************************************************/
/*
    noBacklight()

    Turns off the backlight via PCF8574. 

    NOTE:
    - doesn't affect lcd controller, because we are working with
      transistor conncted to PCF8574 port
*/
/**************************************************************************/
void LiquidCrystal_I2C::noBacklight(void)
{
  switch (_backlightPolarity)
  {
    case POSITIVE:
      _backlightValue = LCD_BACKLIGHT_OFF;
      break;

    case NEGATIVE:
      _backlightValue = ~LCD_BACKLIGHT_OFF;
      break;
  }

  _backlightValue <<= _LCD_TO_PCF8574[0];

  writePCF8574(PCF8574_ALL_LOW);
}

/**************************************************************************/
/*
    backlight()

    Turns on backlight via PCF8574.

    NOTE:
    - doesn't affect lcd controller, because we are working with
      transistor conncted to PCF8574 port
*/
/**************************************************************************/
void LiquidCrystal_I2C::backlight(void)
{
  switch (_backlightPolarity)
  {
    case POSITIVE:
      _backlightValue = LCD_BACKLIGHT_ON;
      break;

    case NEGATIVE:
      _backlightValue = ~LCD_BACKLIGHT_ON;
      break;
  }

  _backlightValue <<= _LCD_TO_PCF8574[0];

  writePCF8574(PCF8574_ALL_LOW);
}

/**************************************************************************/
/*
    write()

    Replaces function "write()" in Arduino class "Print" & sends character
    to the LCD
*/
/**************************************************************************/
#if defined(ARDUINO) && ((ARDUINO) >= 100)
size_t LiquidCrystal_I2C::write(uint8_t value)
#else
void LiquidCrystal_I2C::write(uint8_t value)
#endif
{
  send(LCD_DATA_WRITE, value, LCD_CMD_LENGTH_8BIT);

  #if defined(ARDUINO) && ((ARDUINO) >= 100)
  return 1;
  #endif
}

/**************************************************************************/
/*
    initialization()

    Soft reset lcd & activate 4-bit interface

    NOTE:
    - for correct LCD operation it is necessary to do the internal circuit
      reset & initialization procedure. See 4-bit initializations
      procedure fig.24 on p.46 of HD44780 datasheet and p.17 of 
      WH1602B/WH1604B datasheet for details.
*/
/**************************************************************************/
void LiquidCrystal_I2C::initialization(void)
{
  uint8_t displayFunction = 0; //don't change!!! default bits value DB7, DB6, DB5, DB4=(DL), DB3=(N), DB2=(F), DB1, DB0

  /*
     HD44780 & clones needs ~40ms after voltage rises above 2.7v
     some Arduino boards can start & execute code at 2.4v, so we'll wait 500ms
  */
  delay(500);

  /*
     FIRST ATTEMPT: set 8-bit mode
     - wait > 4.1ms, some LCD even slower than 4.5ms
     - for Hitachi & Winstar displays
  */
  send(LCD_INSTRUCTION_WRITE, LCD_FUNCTION_SET | LCD_8BIT_MODE, LCD_CMD_LENGTH_4BIT);
  delay(5);

  /*
     SECOND ATTEMPT: set 8-bit mode
     - wait > 100us.
     - for Hitachi, not needed for Winstar displays
  */
  send(LCD_INSTRUCTION_WRITE, LCD_FUNCTION_SET | LCD_8BIT_MODE, LCD_CMD_LENGTH_4BIT);
  delayMicroseconds(200);
	
  /*
     THIRD ATTEMPT: set 8 bit mode
     - used for Hitachi, not needed for Winstar displays
  */
  send(LCD_INSTRUCTION_WRITE, LCD_FUNCTION_SET | LCD_8BIT_MODE, LCD_CMD_LENGTH_4BIT);
  delayMicroseconds(100);
	
  /*
     FINAL ATTEMPT: set 4-bit interface
     - Busy Flag (BF) can be checked after this instruction
  */
  send(LCD_INSTRUCTION_WRITE, LCD_FUNCTION_SET | LCD_4BIT_MODE, LCD_CMD_LENGTH_4BIT);

  /* sets qnt. of lines */
  if (_lcd_rows > 1) displayFunction |= LCD_2_LINE;    //line bit located at BD3 & zero/1 line by default

  /* sets font size, 5x8 by default */
  if (_lcd_font_size == LCD_5x10DOTS)
  {
    displayFunction |= LCD_5x10DOTS;                   //font bit located at BD2
    if(_lcd_rows != 1) displayFunction &= ~LCD_2_LINE; //safety check, two rows displays can't display 10 pixel font
  }

  /* initializes lcd functions: qnt. of lines, font size, etc., this settings can't be changed after this point */
  send(LCD_INSTRUCTION_WRITE, LCD_FUNCTION_SET | LCD_4BIT_MODE | displayFunction, LCD_CMD_LENGTH_8BIT);
	
  /* initializes lcd controls: turn display off, underline cursor off & blinking cursor off */
  _displayControl = LCD_UNDERLINE_CURSOR_OFF | LCD_BLINK_CURSOR_OFF;
  noDisplay();

  /* clear display */
  clear();

  /* initializes lcd basics: sets text direction "left to right" & cursor movement to the right */
  _displayMode = LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_OFF;
  send(LCD_INSTRUCTION_WRITE, LCD_ENTRY_MODE_SET | _displayMode, LCD_CMD_LENGTH_8BIT);

  display();
}

/**************************************************************************/
/*
    send()

    The most anvanced & fastes way to write COMMAND or DATA/TEXT to LCD
    
    NOTE:
    - all inputs formated as follow: 
      - mode : RS,RW,E=1,DB7,DB6,DB5,DB4,BCK_LED=0
      - value: DB7,DB6,DB5,DB4,DB3,DB2,DB1,DB0

    - duration of command > 43usec for GDM2004D
    - duration of the En pulse > 450nsec
*/
/**************************************************************************/
void LiquidCrystal_I2C::send(uint8_t mode, uint8_t value, uint8_t length)
{
  uint8_t  halfByte = 0; //lsb or msb
  uint8_t  data     = 0;

  /* 4-bit or 1-st part of 8-bit command */
  halfByte  = value >> 3;                     //0,0,0,DB7,DB6,DB5,DB4,DB3
  halfByte &= 0x1E;                           //0,0,0,DB7,DB6,DB5,DB4,BCK_LED=0
  data      = portMapping(mode | halfByte);   //RS,RW,E=1,DB7,DB6,DB5,DB4,BCK_LED=0

  writePCF8574(data);                         //send command
                                              //En pulse duration > 450nsec
  bitClear(data, _LCD_TO_PCF8574[5]);         //RS,RW,E=0,DB7,DB6,DB5,DB4,BCK_LED=0
  writePCF8574(data);                         //execute command
  delayMicroseconds(LCD_COMMAND_DELAY);       //command duration

  /* second part of 8-bit command */
  if (length == LCD_CMD_LENGTH_8BIT)
  {
    halfByte  = value << 1;                   //DB6,DB5,DB4,DB3,DB2,DB1,DB0,0
    halfByte &= 0x1E;                         //0,0,0,DB3,DB2,DB1,DB0,BCK_LED=0
    data      = portMapping(mode | halfByte); //RS,RW,E=1,DB3,DB2,DB1,DB0,BCK_LED=0

    writePCF8574(data);                       //send command
                                              //En pulse duration > 450nsec
    bitClear(data, _LCD_TO_PCF8574[5]);       //RS,RW,E=0,DB3,DB2,DB1,DB0,BCK_LED=0
    writePCF8574(data);                       //execute command
    delayMicroseconds(LCD_COMMAND_DELAY);     //command duration
  }
}

/**************************************************************************/
/*
    portMapping()

    All magic of all lcd pins to ports mapping is happening here!!!

    NOTE:
    - input value formated as:
        7  6  5  4  3   2   1   0-bit
      - RS,RW,E,DB7,DB6,DB5,DB4,BCK_LED
      - RS,RW,E,DB3,DB2,DB1,DB0,BCK_LED

    - lcd pin to PCF8574 ports table/array formated
        0       1   2  3   4   5  6  7
      {BCK_LED,DB4,DB5,DB6,DB7,E,RW,RS} 

    - shifts value bits to the right PCF8574 ports
      {BCK_LED,DB4,DB5,DB6,DB7,E,RW,RS} shift-> to ports position P7..P0
      {BCK_LED,DB4,DB5,DB6,DB7,E,RW,RS} shift-> to ports position P7..P0

    - "switch case" is 32us faster than
      bitWrite(data, _LCD_TO_PCF8574[i], bitRead(value, i));
*/
/**************************************************************************/
uint8_t LiquidCrystal_I2C::portMapping(uint8_t value)
{
  uint8_t data = 0;

  /* mapping value = RS,RW,E,DB7,DB6,DB5,DB4,BCK_LED */
  for (int8_t i = 7; i >= 0; i--)
  {
    switch (bitRead(value, i))              //"switch case" has smaller footprint than "if else"
    {
      case 1:
        data |= 0x01 << _LCD_TO_PCF8574[i];
        break;
    }
  }

  return data; 
}

/**************************************************************************/
/*
    writePCF8574()

    Masks backlight with data & writes it to PCF8574 over I2C

    NOTE:
    - Wire.endTransmission() returned code:
      0 - success
      1 - data too long to fit in transmit data16
      2 - received NACK on transmit of address
      3 - received NACK on transmit of data
      4 - other error
*/
/**************************************************************************/
bool LiquidCrystal_I2C::writePCF8574(uint8_t value)
{
  Wire.beginTransmission(_PCF8574_address);

  #if defined(ARDUINO) && ((ARDUINO) >= 100)
  Wire.write(value | _backlightValue);
  #else
  Wire.send(value | _backlightValue);
  #endif

  if (Wire.endTransmission(true) == 0) return true;
                                       return false;
}

/**************************************************************************/
/*
    readPCF8574()

    Reads byte* from PCF8574 over I2C

    *logic values on the PCF8574 pins P0...P7

    NOTE:
    - if PCF8574 output is written low before read, the low is always
      returned, regardless of the device state connected
      to the I/O, see Quasi-Bidirectional I/O for more details.
    - if PCF8574 output is written high before read, devices has fully
      control of PCF8574 I/O.
*/
/**************************************************************************/
uint8_t LiquidCrystal_I2C::readPCF8574()
{
  #if defined(_VARIANT_ARDUINO_STM32_)
  Wire.requestFrom(_PCF8574_address, 1);
  #else
  Wire.requestFrom(_PCF8574_address, 1, true); //true, stop message after transmission & releas I2C bus
  #endif
  if (Wire.available() != 1) return false;     //check "wire.h" rxBuffer & error handler, collision on the i2c bus

  /* reads byte from "wire.h" rxBuffer */
  #if defined(ARDUINO) && ((ARDUINO) >= 100)
  return Wire.read();
  #else
  return Wire.receive();
  #endif
}

/**************************************************************************/
/*
    readBusyFlag()

    Reads busy flag (BF)

    NOTE:
    - set RS=0 & RW=1 to retrive busy flag
    - set PCF8574 input pins to HIGH, see Quasi-Bidirectional I/O
    - DB7 = 1, lcd busy
      DB7 = 0, lcd ready
    - input value formated as:
        7  6  5  4  3   2   1   0-bit
      - RS,RW,E,DB7,DB6,DB5,DB4,BCK_LED
      - RS,RW,E,DB3,DB2,DB1,DB0,BCK_LED
*/
/**************************************************************************/
bool LiquidCrystal_I2C::readBusyFlag()
{
  send(LCD_BUSY_FLAG_READ, PCF8574_DATA_HIGH, LCD_CMD_LENGTH_4BIT); //set RS=0, RW=1 & input pins to HIGH, see Quasi-Bidirectional I/O

  return bitRead(readPCF8574(), _LCD_TO_PCF8574[4]);
}

/**************************************************************************/
/*
    getCursorPosition()

    Returns contents of address counter

    NOTE:
    - set RS=0 & RW=1 to retrive address counter
    - set PCF8574 input pins to HIGH, see Quasi-Bidirectional I/O
    - address counter content DB6,DB5,DB4,DB3,DB2,DB1,DB0 
    - input value formated as:
        7  6  5  4  3   2   1   0-bit
      - RS,RW,E,DB7,DB6,DB5,DB4,BCK_LED
      - RS,RW,E,DB3,DB2,DB1,DB0,BCK_LED
*/
/**************************************************************************/
uint8_t LiquidCrystal_I2C::getCursorPosition()
{
  uint8_t data     = 0;
  uint8_t position = 0;

  send(LCD_BUSY_FLAG_READ, PCF8574_DATA_HIGH, LCD_CMD_LENGTH_4BIT); //set RS=0, RW=1 & input pins to HIGH, see Quasi-Bidirectional I/O

  data = readPCF8574();                                             //read RS,RW,E,DB7,DB6,DB5,DB4,BCK_LED

  /* saving DB6,DB5,DB4 bits*/
  for (int8_t i = 3; i >= 1; i--)
  {
    bitWrite(position, (3 + i), bitRead(data, _LCD_TO_PCF8574[i])); //xx,DB6,DB5,DB4,DB3,DB2,DB1,DB0
  }

  send(LCD_BUSY_FLAG_READ, PCF8574_DATA_HIGH, LCD_CMD_LENGTH_4BIT); //set RS=0, RW=1 & input pins to HIGH, see Quasi-Bidirectional I/O

  data = readPCF8574();                                             //read RS,RW,E,DB3,DB2,DB1,DB0,BCK_LED

  /* saving DB3,DB2,DB1,DB0 bits */
  for (int8_t i = 4; i >= 1; i--)
  {
    bitWrite(position, (i - 1), bitRead(data, _LCD_TO_PCF8574[i])); //xx,DB6,DB5,DB4,DB3,DB2,DB1,DB0
  }

  return position;
}

/*************** !!! arduino not standard API functions !!! ***************/
/**************************************************************************/
/*
    printHorizontalGraph(name, row, value, maxValue)

    Prints horizontal graph
*/
/**************************************************************************/
void LiquidCrystal_I2C::printHorizontalGraph(char name, uint8_t row, uint16_t currentValue, uint16_t maxValue)
{
  uint8_t currentGraph = 0;
  uint8_t colum        = 0;

  if (currentValue > maxValue) currentValue = maxValue;          //safety check, to prevent ESP8266 crash

  currentGraph = map(currentValue, 0, maxValue, 0, _lcd_colums);

  setCursor(colum, row);
  send(LCD_DATA_WRITE, name, LCD_CMD_LENGTH_8BIT);

  /* draw the horizontal bar without clearing the display, to eliminate flickering */
  for (colum = 1; colum < currentGraph; colum++)
  {
    setCursor(colum, row);
    send(LCD_DATA_WRITE, 0xFF, LCD_CMD_LENGTH_8BIT);             //print 0xFF - built in "solid square" symbol, see p.17 & p.30 of HD44780 datasheet
  }

  /* fill the rest with spaces */
  while (colum++ < _lcd_colums)
  {
    send(LCD_DATA_WRITE, 0x20, LCD_CMD_LENGTH_8BIT);             //print 0x20 - built in "space" symbol, see p.17 & p.30 of HD44780 datasheet
  }
}

/**************************************************************************/
/*
    displayOff()

    Turns off backlight via PCF8574 & clears text from the screen

    NOTE:
    - text remains in DDRAM
*/
/**************************************************************************/
void LiquidCrystal_I2C::displayOff(void)
{
  noBacklight();
  noDisplay();
}

/**************************************************************************/
/*
    displayOn()

    Turns on backlight via PCF8574 & shows text from DDRAM
*/
/**************************************************************************/
void LiquidCrystal_I2C::displayOn(void)
{
  display();
  backlight();
}

/**************************************************************************/
/*
    setBrightness()

    NOTE:
    - to use this function, the "LED" jumper on the back of backpack has
      to be removed & the top pin has to be connected to one of Arduino
      PWM pin in series with 470 Ohm resistor
    - recomended min. value = 25, max. value = 255 (0.5v .. 4.5v)
                 min. value = 0,  max. value = 255 (0.0v .. 4.5v)  
*/
/**************************************************************************/
void LiquidCrystal_I2C::setBrightness(uint8_t pin, uint8_t value, backlightPolarity polarity)
{
  pinMode(pin, OUTPUT);

  if (polarity == NEGATIVE) value = 255 - value;

  analogWrite(pin, value);
}
