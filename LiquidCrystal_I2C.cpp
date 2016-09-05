/******************************************************************************/
/*
  This is an Arduino library for HD44780, S6A0069, KS0066U, NT3881D, LC7985, 
  ST7066, SPLC780, WH160xB, AIP31066, GDM200xD LCD displays, operated in 4 bit
  mode over I2C bus with 8-bit I/O expander PCF8574. Typical displays resolutions
  are: 16x1, 16x2, 16x4, 20x2, 20x4 and etc.

  written by enjoyneering79

  These IC uses I2C bus to communicate, 2 pins are required to  
  interface

  Connect PCF8574 to pins :  SDA     SCL
  Uno, Mini, Pro:            A4      A5
  Mega2560, Due:             20      21
  Leonardo:                  2       3
  ATtiny85:                  0(5)    2/A1(7) (TinyWireM   - https://github.com/SpenceKonde/TinyWireM)
  ESP8266 ESP-xx:            ANY     ANY     (ESP8266Core - https://github.com/esp8266/Arduino)
  NodeMCU 1.0:               ANY     ANY     (D1 & D2 by default)

  BSD license, all text above must be included in any redistribution
*/
/******************************************************************************/

#include "LiquidCrystal_I2C.h"


/**************************************************************************/
/*
    LCD - LiquidCrystal_I2C()

    Constructor. Initializes the class variables, defines I2C address,
    LCD & PCF8574 pins.

    NOTE: The constructor does not reset & initialize the LCD.
*/
/**************************************************************************/  
LiquidCrystal_I2C::LiquidCrystal_I2C(PCF8574_address addr, uint8_t P0, uint8_t P1, uint8_t P2, uint8_t P3, uint8_t P4, uint8_t P5, uint8_t P6, uint8_t P7, switchPolarity polarity)
{
  /* PCF8574 ports -> LCD pins mapping array */
  uint8_t PCF8574_TO_LCD[8] = {P0, P1, P2, P3, P4, P5, P6, P7};

  _PCF8574_address        = addr;
  _PCF8574_initialisation = true;
  _switchPolarity         = polarity;

  /* LCD pins -> PCF8574 ports mapping array. */
  for (uint8_t i = 0; i < 8; i++)
  {
    switch(PCF8574_TO_LCD[i])
    {
      case 4:         //RS pin
        _LCD_TO_PCF8574[0] = i;
        break;
      case 5:         //RW pin
        _LCD_TO_PCF8574[1] = i;
        break;
      case 6:         //EN pin
        _LCD_TO_PCF8574[2] = i;
        break;
      case 14:        //D7 pin
        _LCD_TO_PCF8574[3] = i;
        break;
      case 13:        //D6 pin
        _LCD_TO_PCF8574[4] = i;
        break;
      case 12:        //D5 pin
        _LCD_TO_PCF8574[5] = i;
        break;
      case 11:        //D4 pin
        _LCD_TO_PCF8574[6] = i;
        break;
      case 16:        //BL pin
        _LCD_TO_PCF8574[7] = i;
        break;
      default:
        _PCF8574_initialisation = false; //safety check - make sure the declaration of lcd pins is right
        break;
    }
  }

  /* backlight controled by PCF8574 is ON (by default). NOTE: backlight controled by LCD is not configured yet */
  if (_switchPolarity == POSITIVE)
  {
    _backlightValue = LCD_BACKLIGHT_ON;
  }
  else
  {
    _backlightValue = ~LCD_BACKLIGHT_ON;  
  }
  _backlightValue = _backlightValue << (_LCD_TO_PCF8574[7]);
}

/**************************************************************************/
/*
    LCD - begin()

    Initializes, resets & configures I2C and LCD (call this function before
    doing anything else)

    NOTE: For correct LCD operation it is necessary to do the internal circuit
          reset & initialization procedure. See 4-bit initializations
          procedure fig.24 on p.46 of HD44780 datasheet and p.17 of 
          WH1602B/WH1604B datasheet for details.

          Wire.endTransmission() returned code:
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
 #else
bool LiquidCrystal_I2C::begin(uint8_t lcd_colums, uint8_t lcd_rows, lcd_font_size f_size)
{
 #endif
  /* safety check - make sure the declaration of lcd pins is right */
  if (_PCF8574_initialisation == false)
  {
    return false;
  }
 #if defined(ESP8266)
  Wire.begin(sda, scl);
 #else
   Wire.begin();
 #endif
  Wire.beginTransmission(_PCF8574_address);
  /* safety check - make sure the PCF8574 is connected */
  if (Wire.endTransmission() != 0)
  {
    return false;
  }
  /* safety check - set all PCF8574 pins low */
  writePCF8574(PCF8574_ALL_LOW);

  _lcd_colums 	 = lcd_colums;
  _lcd_rows      = lcd_rows;
  _lcd_font_size = f_size;

  if (_lcd_rows > 1)
  {
    _displayFunction |= LCD_2_LINE;
  }
  else
  {
    _displayFunction |= LCD_1_LINE; 
  }

  if (_lcd_font_size == LCD_5x8DOTS)
  {
    _displayFunction |= LCD_5x8DOTS;
  }
  else
  {
    _displayFunction |= LCD_5x10DOTS;
    /* safety check - only 1 row displays have 10 pixel font */
    if(_lcd_rows != 1)
    {
      _displayFunction |= LCD_1_LINE;
    }
  }

  /* !!! START OF 4-BIT INITIALIZATIONS PROCEDURE !!! */
  /* 
  HD44780/WH160xB & etc need ~40ms after voltage rises above 2.7v
  some Arduino can run at 2.4v so we'll wait 60ms
  NB: max. delay is 65535 us
  */
  delayMicroseconds(60000);

  /*
  FIRST ATTEMPT: set 8 bit mode. wait min 4.1ms
  some LCDs need more than 4.5ms. so we'll wait 6ms
  NB: used for Hitachi & Winstar
  */
  send(LCD_INSTRUCTION_WRITE, LCD_8BIT_MODE, 4);
  delayMicroseconds(6000);

  /*
  SECOND ATTEMPT: set 8 bit mode. wait min 100us.
  NB: used for Hitachi only (not needed for Winstar displays)
  */
  send(LCD_INSTRUCTION_WRITE, LCD_8BIT_MODE, 4);
  delayMicroseconds(150);
	
  /*
  THIRD ATTEMPT: set 8 bit mode.
  NB: used for Hitachi only (not needed for Winstar displays)
  */
  send(LCD_INSTRUCTION_WRITE, LCD_8BIT_MODE, 4);
  delayMicroseconds(150);
	
  /* FINAL ATTEMPT: set 4-bit interface. Busy Flag can be checked after this instruction */
  send(LCD_INSTRUCTION_WRITE, LCD_4BIT_MODE, 4);

  /* initializes lcd functions: qnt. of lines, font size, etc. (can't be changed after this point) */
  send(LCD_INSTRUCTION_WRITE, LCD_4BIT_MODE | _displayFunction, 8);
	
  /* initializes lcd controls: turn display off, set cursor off and blinking off (by default) */
  _displayControl = LCD_CURSOR_OFF | LCD_BLINK_OFF;
  noDisplay();

  /* clear display */
  clear();

  /* initializes lcd basics: sets text direction "left to right" & cursor movement to the right (by default) */
  _displayMode = LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DECREMENT;
  send(LCD_INSTRUCTION_WRITE, LCD_ENTRY_MODE_SET | _displayMode, 8);

  /* !!! END OF 4-BIT INITIALIZATIONS PROCEDURE !!! */
	
  display();

  return true;
}

/**************************************************************************/
/*
    LCD - clear()

    Clear display (fill the screen with spacers) and  & set cursor position to zero
*/
/**************************************************************************/
void LiquidCrystal_I2C::clear(void)
{
  send(LCD_INSTRUCTION_WRITE, LCD_CLEAR_DISPLAY, 8);
  delayMicroseconds(LCD_HOME_CLEAR_DELAY);
}

/**************************************************************************/
/*
    LCD - home()

    Set cursor position to start position

    NOTE: cursor start position (0, 0)
*/
/**************************************************************************/
void LiquidCrystal_I2C::home(void)
{
  send(LCD_INSTRUCTION_WRITE, LCD_RETURN_HOME, 8);
  delayMicroseconds(LCD_HOME_CLEAR_DELAY);
}

/**************************************************************************/
/*
    LCD - setCursor()

    Sets cursor position

    NOTE: cursor start position (0, 0)
          cursor end   position (lcd_colums - 1, lcd_rows - 1)
*/
/**************************************************************************/
void LiquidCrystal_I2C::setCursor(uint8_t colum, uint8_t row)
{

  uint8_t row_address_offset[] = {0x00, 0x40, uint8_t(0x00 + _lcd_colums), uint8_t(0x40 + _lcd_colums)};

  /* safety check - cursor position & array are zero indexed */
  if (row >= _lcd_rows)
  {
    row = _lcd_rows - 1;
  }
  if (colum >= _lcd_colums)
  {
    colum = _lcd_colums - 1;
  }

  send(LCD_INSTRUCTION_WRITE, LCD_SET_DDRAM_ADDR | (colum + row_address_offset[row]), 8);
}

/**************************************************************************/
/*
    LCD - noDisplay()

    Turns the display OFF & clears text from the screen

    NOTE: text & data remains in DDRAM !!!
*/
/**************************************************************************/
void LiquidCrystal_I2C::noDisplay(void)
{
  _displayControl &= LCD_DISPLAY_OFF;
  send(LCD_INSTRUCTION_WRITE, LCD_DISPLAY_CONTROL | _displayControl, 8);
}

/**************************************************************************/
/*
    LCD - display()

    Turns the display ON & retrives text from DDRAM

    NOTE: text & data remains in DDRAM !!!
*/
/**************************************************************************/
void LiquidCrystal_I2C::display(void)
{
  _displayControl |= LCD_DISPLAY_ON;
  send(LCD_INSTRUCTION_WRITE, LCD_DISPLAY_CONTROL | _displayControl, 8);
}

/**************************************************************************/
/*
    LCD - noCursor()

    Turns OFF the underline cursor
*/
/**************************************************************************/
void LiquidCrystal_I2C::noCursor(void)
{
  _displayControl &= LCD_CURSOR_OFF;
  send(LCD_INSTRUCTION_WRITE, LCD_DISPLAY_CONTROL | _displayControl, 8);
}

/**************************************************************************/
/*
    LCD - cursor()

    Turns ON the underline cursor
*/
/**************************************************************************/
void LiquidCrystal_I2C::cursor(void)
{
  _displayControl |= LCD_CURSOR_ON;
  send(LCD_INSTRUCTION_WRITE, LCD_DISPLAY_CONTROL | _displayControl, 8);
}

/**************************************************************************/
/*
    LCD - noBlink()

    Turns OFF the blinking cursor
*/
/**************************************************************************/
void LiquidCrystal_I2C::noBlink(void)
{
  _displayControl &= LCD_BLINK_OFF;
  send(LCD_INSTRUCTION_WRITE, LCD_DISPLAY_CONTROL | _displayControl, 8);
}

/**************************************************************************/
/*
    LCD - blink()

    Turns ON the blinking cursor
*/
/**************************************************************************/
void LiquidCrystal_I2C::blink(void)
{
  _displayControl |= LCD_BLINK_ON;
  send(LCD_INSTRUCTION_WRITE, LCD_DISPLAY_CONTROL | _displayControl, 8);
}

/**************************************************************************/
/*
    LCD - scrollDisplayLeft()

    Scrolls the text on the display to the left, without changing the RAM
*/
/**************************************************************************/
void LiquidCrystal_I2C::scrollDisplayLeft(void)
{
  send(LCD_INSTRUCTION_WRITE, LCD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_LEFT, 8);
}

/**************************************************************************/
/*
    LCD - scrollDisplayRight()

    Scrolls the text on the display to the right, without changing the RAM
*/
/**************************************************************************/
void LiquidCrystal_I2C::scrollDisplayRight(void)
{
  send(LCD_INSTRUCTION_WRITE, LCD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_RIGHT, 8);
}

/**************************************************************************/
/*
    LCD - leftToRight()

    Sets text direction - from Left to Right
*/
/**************************************************************************/
void LiquidCrystal_I2C::leftToRight(void)
{
  _displayMode |= LCD_ENTRY_LEFT;
  send(LCD_INSTRUCTION_WRITE, LCD_ENTRY_MODE_SET | _displayMode, 8);
}

/**************************************************************************/
/*
    LCD - rightToLeft()

    Sets text direction - from Right to Left
*/
/**************************************************************************/
void LiquidCrystal_I2C::rightToLeft(void)
{
  _displayMode |= LCD_ENTRY_RIGHT;
  send(LCD_INSTRUCTION_WRITE, LCD_ENTRY_MODE_SET | _displayMode, 8);
}

/**************************************************************************/
/*
    LCD - autoscroll()

    Autoscrolls the text on the display
*/
/**************************************************************************/
void LiquidCrystal_I2C::autoscroll(void) 
{
  _displayMode |= LCD_ENTRY_SHIFT_INCREMENT;
  send(LCD_INSTRUCTION_WRITE, LCD_ENTRY_MODE_SET | _displayMode, 8);
}

/**************************************************************************/
/*
    LCD - noAutoscroll()

    Stops text autoscrolling on the display
*/
/**************************************************************************/
void LiquidCrystal_I2C::noAutoscroll(void)
{
  _displayMode |= LCD_ENTRY_SHIFT_DECREMENT;
  send(LCD_INSTRUCTION_WRITE | LCD_ENTRY_MODE_SET, _displayMode, 8);
}

/**************************************************************************/
/*
    LCD - createChar()

    Fills the 64 bytes CGRAM, with custom characters

    NOTE: 8 locations for 5x8DOTS  display, at index 0-7
          4 locations for 5x10DOTS display, at index 0-3
*/
/**************************************************************************/
void LiquidCrystal_I2C::createChar(uint8_t CGRAM_address, uint8_t char_pattern[])
{
  uint8_t CGRAM_capacity;
  uint8_t font_size;

  if (_lcd_font_size == LCD_5x8DOTS)
  {
    CGRAM_capacity = 0x07;
    font_size = 8;
  }
  else
  {
    CGRAM_capacity = 0x3;
    font_size = 10;
  }

  /* safety check - make sure CGRAM_address never exceeds the CGRAM_capacity */
  CGRAM_address &= CGRAM_capacity;

  send(LCD_INSTRUCTION_WRITE, LCD_SET_CGRAM_ADDR | (CGRAM_address << 3), 8);

  for (uint8_t i = 0; i < font_size; i++)
  {
    send(LCD_DATA_WRITE, char_pattern[i], 8);
  }
}

/**************************************************************************/
/*
    LCD - noBacklight()

    Turns OFF the backlight with PCF8574. 

    NOTE: doesn't affect lcd controller, because we are working with
          transistor conncted to the PCF8574 port
*/
/**************************************************************************/
void LiquidCrystal_I2C::noBacklight(void)
{
  if (_switchPolarity == POSITIVE)
  {
    _backlightValue = LCD_BACKLIGHT_OFF;
  }
  else
  {
    _backlightValue = ~LCD_BACKLIGHT_OFF;  
  }
  _backlightValue = _backlightValue << (_LCD_TO_PCF8574[7]);

  writePCF8574(PCF8574_ALL_LOW | _backlightValue);
}

/**************************************************************************/
/*
    LCD - backlight()

    Turns ON the backlight with PCF8574.

    NOTE: doesn't affect lcd controller, because we are working with
          transistor conncted to the PCF8574 port
*/
/**************************************************************************/
void LiquidCrystal_I2C::backlight(void)
{
  if (_switchPolarity == POSITIVE)
  {
    _backlightValue = LCD_BACKLIGHT_ON;
  }
  else
  {
    _backlightValue = ~LCD_BACKLIGHT_ON;  
  }
  _backlightValue = _backlightValue << (_LCD_TO_PCF8574[7]);

  writePCF8574(PCF8574_ALL_LOW | _backlightValue);
}


/**************************************************************************/
/*
    LCD - write()

    Arduino Class <Print> calls this to send characters to the LCD
*/
/**************************************************************************/
inline size_t LiquidCrystal_I2C::write(uint8_t value)
{
  send(LCD_DATA_WRITE, value, 8);
  return 1;
}


/**************************************************************************/
/*
    LCD - send()

    The most anvanced & fastes way to write COMMAND or DATA/TEXT to LCD
    
    NOTE: all inputs formated as follow 
          mode  - RS,RW,E,DB7,DB6,DB5,DB4,BCK_LED
          value - DB7,DB6,DB5,DB4,DB3,DB2,DB1,DB0
*/
/**************************************************************************/
inline void LiquidCrystal_I2C::send(uint8_t mode, uint8_t value, uint8_t length)
{
  uint8_t data;
  uint8_t msb;
  uint8_t lsb;

  switch(length)
  {
    case 4:
      lsb = value >> 4;                      //isolate 4 LBS bits (leftmost bits)
      lsb = lsb << 1;                        //shift 1 bit left for future backlight value
      data = portMapping(mode | lsb);        //mask register command & value
      writePCF8574(data);                    //send the command with En pulse HIGH
      delayMicroseconds(LCD_EN_PULSE_DELAY); //En pulse must be > 450ns
      bitClear(data, (_LCD_TO_PCF8574[2]));  //set En pin to LOW to execute the command
      writePCF8574(data);                    //execute the command
      delayMicroseconds(LCD_COMMAND_DELAY);  //LCD needs > ~37 .. 43us to finish the command
      break;
    case 8:
      lsb = value >> 4;                      //see p.42 & p.17 of HD44780 datasheet 
      lsb = lsb << 1;
      data = portMapping(mode | lsb);
      writePCF8574(data);
      delayMicroseconds(LCD_EN_PULSE_DELAY);
      bitClear(data, (_LCD_TO_PCF8574[2]));
      writePCF8574(data);
      delayMicroseconds(LCD_COMMAND_DELAY);
      
      msb = value << 4;
      msb = msb >> 3;
      data = portMapping(mode | msb);
      writePCF8574(data);
      delayMicroseconds(LCD_EN_PULSE_DELAY);
      bitClear(data, (_LCD_TO_PCF8574[2]));
      writePCF8574(data);
      delayMicroseconds(LCD_COMMAND_DELAY);
      break;
    default:
      writePCF8574(PCF8574_ALL_LOW);
      break;
  }
}


/**************************************************************************/
/*
    LCD - portMapping()

    All magic of pins -> ports mapping is happening here!!!

    NOTE: Isolates HD44780 bits & shifts to the right PCF8574 ports position
          RS,RW,E,DB7,DB6,DB5,DB4,BCK_LED -> P0,P1,P2,P3,P4,P5,P6,P7
          RS,RW,E,DB3,DB2,DB1,DB0,BCK_LED -> P0,P1,P2,P3,P4,P5,P6,P7  
*/
/**************************************************************************/
uint8_t LiquidCrystal_I2C::portMapping(uint8_t value)
{
  uint8_t data;

  for (uint8_t i = 0; i < 8; i++)
  {
    bitWrite(data, _LCD_TO_PCF8574[i], bitRead(value, (7 - i)));
  }

  return data; 
}


/**************************************************************************/
/*
    LCD - writePCF8574()

    Adds Backlight value to the data & writes a byte to PCF8574 over I2C
*/
/**************************************************************************/
void LiquidCrystal_I2C::writePCF8574(uint8_t _data)
{
  Wire.beginTransmission(_PCF8574_address);
 #if ARDUINO >= 100
  Wire.write(_data | _backlightValue);
 #else
  Wire.send(_data | _backlightValue);
 #endif
  Wire.endTransmission();
}

/**************************************************************************/
/*
    LCD - readPCF8574()

    Reads byte* from PCF8574 over I2C

    * - the logic values on the PCF8574 pins P0...P7

    NOTE: If the PCF8574 output is written low before a read, a low is always
          read from the I/O, regardless of the state of the device connected
          to the I/O.
          Therefore, if the PCF8574 outputs are all high (either from power-up
          reset initialization or from a write) before reading is performed,
          any devices connected to the I/O can fully control the I/O.
*/
/**************************************************************************/
uint8_t LiquidCrystal_I2C::readPCF8574()
{
  uint8_t pollCounter;

  Wire.requestFrom(_PCF8574_address, 1); //requesting 1 byte
  while (Wire.available() != 1)          //safety check - slave could send less than requested
  {
    pollCounter++;
    if (pollCounter > 3)                 //tries 3 times, before returns zero
    {
      return 0x00;
    }
    delayMicroseconds(5);
  }

 #if ARDUINO >= 100
  return Wire.read();
 #else
  return Wire.receive();
 #endif
}

/**************************************************************************/
/*
    LCD - busyFlagCheck()

    Checks the LCD's Busy Flag (BF). Retuns: 1 - busy, 0 - ready.   
    Contents of address counter (cursor position) also can be read. 

    NOTE: To retrive the BF, set RS = 0 & RW = 1

          DB7 pin = 1     - Internaly operated (busy)
          DB7 pin = 0     - Next instruction accepted (ready)
          address counter - DB6, DB5, DB4, DB3, DB2, DB1, DB0
*/
/**************************************************************************/
bool LiquidCrystal_I2C::busyFlagCheck()
{
  send(LCD_BUSY_FLAG_READ, PCF8574_DATA_HIGH, 4);

  return bitRead(readPCF8574(), _LCD_TO_PCF8574[3]);
}


/* !!! Arduino Unsupported API functions !!! */

/**************************************************************************/
/*
    LCD - status()
    
    return DB7, DB6, DB5, DB4, DB3, DB2, DB1, DB0 values from LCD
*/
/**************************************************************************/
uint8_t LiquidCrystal_I2C::status()
{
  send(LCD_DATA_READ, PCF8574_DATA_HIGH, 4);

  return readPCF8574(); 
}


/**************************************************************************/
/*
    LCD - printHorizontalGraph(name, row, value, maxValue)
*/
/**************************************************************************/
void LiquidCrystal_I2C::printHorizontalGraph(char name, uint8_t row, uint16_t currentValue, uint16_t maxValue)
{
    uint16_t currentGraph;
    uint8_t  colum;

    currentGraph = map(currentValue, 0, maxValue, 0, _lcd_colums);

    setCursor(colum, row);
    send(LCD_DATA_WRITE, name, 8);

    /* draw the horizontal bar without clearing the display, to eliminate flickering */
    for (colum = 1; colum < currentGraph; colum++)
    {
      setCursor(colum, row);
      send(LCD_DATA_WRITE, 0xFF, 8); /* print 0xFF - built in "solid square" char. see p.17 & p.30 of HD44780 datasheet */
    }

    /* fill the left overs (from the previous draw) with spaces */
    for (colum; colum < _lcd_colums; colum++)
    {
      send(LCD_DATA_WRITE, 0x20, 8); /* print 0x20 - built in "space" char. see p.17 & p.30 of HD44780 datasheet */
    }
}


/**************************************************************************/
/*
    LCD - draw_vertical_graph()
*/
/**************************************************************************/
void LiquidCrystal_I2C::printVerticalGraph(uint8_t colum, uint8_t row, uint16_t currentValue, uint16_t maxValue)
{
  uint16_t currentGraph;

  uint8_t verticalBar_0[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  uint8_t verticalBar_1[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F};
  uint8_t verticalBar_2[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x1F};
  uint8_t verticalBar_3[8] = {0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F};
  uint8_t verticalBar_4[8] = {0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F};
  uint8_t verticalBar_5[8] = {0x00,0x00,0x00,0x1F,0x1F,0x1F,0x1F,0x1F};
  uint8_t verticalBar_6[8] = {0x00,0x00,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F};
  uint8_t verticalBar_7[8] = {0x00,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F};
  uint8_t verticalBar_8[8] = {0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F};


  currentGraph = map(currentValue, 0, maxValue, 0, 8);
  switch(currentGraph)
  {
    case 0:
      createChar(0x07, verticalBar_0);
      break;
    case 1:
      createChar(0x07, verticalBar_1);
      break;
    case 2:
      createChar(0x07, verticalBar_2);
      break;
    case 3:
      createChar(0x07, verticalBar_3);
      break;
    case 4:
      createChar(0x07, verticalBar_4);
      break;
    case 5:
      createChar(0x07, verticalBar_5);
      break;
    case 6:
      createChar(0x07, verticalBar_6);
      break;
    case 7:
      createChar(0x07, verticalBar_7);
      break;
    case 8:
      createChar(0x07, verticalBar_8);
      break;
    default:
      break;
  }
  setCursor(colum, row);
  send(LCD_DATA_WRITE, 0x07, 8);      /* print custom char from 7th CGRAM Address. */
}


/**************************************************************************/
/*
    LCD - setBrightness()

    !!! - to use this function, the "LED" jumper on the back has to be removed,
          and the bottom pin has to be connected to one of Arduino pwm pin

    NOTE: recomended min. value = 25, max. value = 255 (0.5v .. 4.5v)
                     min. value = 0,  max. value = 255 (0.0v .. 4.5v)  
*/
/**************************************************************************/
void LiquidCrystal_I2C::setBrightness(uint8_t pin, uint8_t value, switchPolarity polarity)
{
  pinMode(pin, OUTPUT);

  if (polarity == NEGATIVE)
  {
    value = 255 - value;
  }

  analogWrite(pin, value);
}
