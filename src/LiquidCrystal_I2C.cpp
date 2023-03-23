/***************************************************************************************************/
/*
   This is an Arduino library for HD44780, S6A0069, KS0066U, NT3881D, LC7985, ST7066, SPLC780,
   WH160xB, AIP31066, GDM200xD, ADM0802A LCD displays

   written by : enjoyneering
   sourse code: https://github.com/enjoyneering/

   NOTE:
   - screens are operated in 4-bit mode over I2C bus with 8-bit I/O expander PCF8574x
   - typical displays sizes: 8x2, 16x1, 16x2, 16x4, 20x2, 20x4 & etc


   This device uses I2C bus to communicate, specials pins are required to interface
   Board                                     SDA              SCL              Level
   Uno, Mini, Pro, ATmega168, ATmega328..... A4               A5               5v
   Mega2560................................. 20               21               5v
   Due, SAM3X8E............................. 20               21               3.3v
   MKR Zero, XIAO SAMD21, SAMD21xx.......... PA08             PA09             3.3v
   Leonardo, Micro, ATmega32U4.............. 2                3                5v
   Digistump, Trinket, Gemma, ATtiny85...... PB0/D0           PB2/D2           3.3v/5v
   Blue Pill*, STM32F103xxxx boards*........ PB7/PB9          PB6/PB8          3.3v/5v
   ESP8266 ESP-01**......................... GPIO0            GPIO2            3.3v/5v
   NodeMCU 1.0**, WeMos D1 Mini**........... GPIO4/D2         GPIO5/D1         3.3v/5v
   ESP32***................................. GPIO21/D21       GPIO22/D22       3.3v
                                             GPIO16/D16       GPIO17/D17       3.3v
                                            *hardware I2C Wire mapped to Wire1 in stm32duino
                                             see https://github.com/stm32duino/wiki/wiki/API#I2C
                                           **most boards has 10K..12K pullup-up resistor
                                             on GPIO0/D3, GPIO2/D4/LED & pullup-down on
                                             GPIO15/D8 for flash & boot
                                          ***hardware I2C Wire mapped to TwoWire(0) aka GPIO21/GPIO22 in Arduino ESP32

   Supported frameworks:
   Arduino Core - https://github.com/arduino/Arduino/tree/master/hardware
   ATtiny  Core - https://github.com/SpenceKonde/ATTinyCore
   ESP8266 Core - https://github.com/esp8266/Arduino
   ESP32   Core - https://github.com/espressif/arduino-esp32
   STM32   Core - https://github.com/stm32duino/Arduino_Core_STM32
   SAMD    Core - https://github.com/arduino/ArduinoCore-samd


   GNU GPL license, all text above must be included in any redistribution,
   see link for details - https://www.gnu.org/licenses/licenses.html
*/
/***************************************************************************************************/

#include "LiquidCrystal_I2C.h"


/**************************************************************************/
/*
    LiquidCrystal_I2C()

    Constructor. Initializes class variables, defines I2C address,
    LCD & PCF8574 pins
*/
/**************************************************************************/  
LiquidCrystal_I2C::LiquidCrystal_I2C(pcf8574Address addr, uint8_t P0, uint8_t P1, uint8_t P2, uint8_t P3, uint8_t P4, uint8_t P5, uint8_t P6, uint8_t P7, backlightPolarity polarity)
{
  uint8_t pcf8574ToLCD[8] = {P0, P1, P2, P3, P4, P5, P6, P7}; //PCF8574 ports to LCD pins mapping array

  _pcf8574Address     = addr;
  _backlightPolarity  = polarity;
  _pcf8574PortsMaping = true;

  /* maping LCD pins to PCF8574 ports */
  for (uint8_t i = 0; i < 8; i++)
  {
    switch(pcf8574ToLCD[i])
    {
      case 4:                   //RS pin
        _lcdToPCF8574[7] = i;
        break;

      case 5:                   //RW pin
        _lcdToPCF8574[6] = i;
        break;

      case 6:                   //EN pin
        _lcdToPCF8574[5] = i;
        break;

      case 14:                  //D7 pin
        _lcdToPCF8574[4] = i;
        break;

      case 13:                  //D6 pin
        _lcdToPCF8574[3] = i;
        break;

      case 12:                  //D5 pin
        _lcdToPCF8574[2] = i;
        break;

      case 11:                  //D4 pin
        _lcdToPCF8574[1] = i;
        break;

      case 16:                  //BL pin
        _lcdToPCF8574[0] = i;
        break;

      default:
        _pcf8574PortsMaping = false; //safety check, make sure the declaration of lcd pins is right
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

  _backlightValue <<= _lcdToPCF8574[0];
}

/**************************************************************************/
/*
    begin()

    Initialize I2C bus, resets & configures LCD

    NOTE:
    - returned value by "Wire.endTransmission()":
      - 0, success
      - 1, data too long to fit in transmit data buffer
      - 2, received NACK on transmit of address
      - 3, received NACK on transmit of data
      - 4, other error
*/
/**************************************************************************/
#if defined (ARDUINO_ARCH_AVR)
bool LiquidCrystal_I2C::begin(uint8_t columns, uint8_t rows, lcdFontSize fontSize, uint32_t speed, uint32_t stretch)
{
  Wire.begin();

  Wire.setClock(speed);                                    //experimental! AVR I2C bus speed 31kHz..400kHz, default 100000Hz

  #if !defined (__AVR_ATtiny85__)                          //for backwards compatibility with ATtiny Core
  Wire.setWireTimeout(stretch, false);                     //experimental! default 25000usec, true=Wire hardware will be automatically reset to default on timeout
  #endif

#elif defined (ARDUINO_ARCH_ESP8266)
bool LiquidCrystal_I2C::begin(uint8_t columns, uint8_t rows, lcdFontSize fontSize, uint8_t sda, uint8_t scl, uint32_t speed, uint32_t stretch)
{
  Wire.begin(sda, scl);

  Wire.setClock(speed);                                    //experimental! ESP8266 I2C bus speed 1kHz..400kHz, default 100000Hz

  Wire.setClockStretchLimit(stretch);                      //experimental! default 150000usec

#elif defined (ARDUINO_ARCH_ESP32)
bool LiquidCrystal_I2C::begin(uint8_t columns, uint8_t rows, lcdFontSize fontSize, int32_t sda, int32_t scl, uint32_t speed, uint32_t stretch) //"int32_t" for Master SDA & SCL, "uint8_t" for Slave SDA & SCL
{
  if (Wire.begin(sda, scl, speed) != true) {return false;} //experimental! ESP32 I2C bus speed ???kHz..400kHz, default 100000Hz

  Wire.setTimeout(stretch / 1000);                         //experimental! default 50msec

#elif defined (ARDUINO_ARCH_STM32)
bool LiquidCrystal_I2C::begin(uint8_t columns, uint8_t rows, lcdFontSize fontSize, uint32_t sda, uint32_t scl, uint32_t speed) //"uint32_t" for pins only, "uint8_t" calls wrong "setSCL(PinName scl)"
{
  Wire.begin(sda, scl);

  Wire.setClock(speed);                                    //experimental! STM32 I2C bus speed ???kHz..400kHz, default 100000Hz

#elif defined (ARDUINO_ARCH_SAMD)
bool LiquidCrystal_I2C::begin(uint8_t columns, uint8_t rows, lcdFontSize fontSize, uint32_t speed)
{
  Wire.begin();

  Wire.setClock(speed);                                    //experimental! SAMD21 I2C bus speed ???kHz..400kHz, default 100000Hz

#else
bool LiquidCrystal_I2C::begin(uint8_t columns, uint8_t rows, lcdFontSize fontSize)
{
  Wire.begin();
#endif

  if (_pcf8574PortsMaping == false) {return false;}        //safety check, make sure lcd pins declaration is right

  Wire.beginTransmission(_pcf8574Address);

  if (Wire.endTransmission() != 0) {return false;}         //safety check, make sure the PCF8574 is connected

  _writePCF8574(PCF8574_PORTS_LOW);                        //safety, set all PCF8574 pins low

  _lcdColumns  = columns;
  _lcdRows     = rows;
  _lcdFontSize = fontSize;

  _initialization();                                       //soft reset LCD & 4-bit mode initialization

  return true;
}


/**************************************************************************/
/*
    clear()

    Clears display & move cursor to home position

    NOTE:
    - fills display with spaces
    - moves cursor to home position (0, 0)
    - command duration > 1.53msec..1.64msec
*/
/**************************************************************************/
void LiquidCrystal_I2C::clear()
{
  _send(LCD_INSTRUCTION_WRITE, LCD_CLEAR_DISPLAY, LCD_CMD_LENGTH_8BIT);

  delay(LCD_HOME_CLEAR_DELAY);
}


/**************************************************************************/
/*
    home()

    Moves cursor position to home position

    NOTE:
    - sets DDRAM address to 0 in address counter, returns display to
      home position, but DDRAM contents remain unchanged
    - command duration > 1.53msec..1.64msec
*/
/**************************************************************************/
void LiquidCrystal_I2C::home()
{
  _send(LCD_INSTRUCTION_WRITE, LCD_RETURN_HOME, LCD_CMD_LENGTH_8BIT);

  delay(LCD_HOME_CLEAR_DELAY);
}


/**************************************************************************/
/*
    setCursor()

    Sets cursor position

    NOTE:
    - cursor position range (0, 0)..(column - 1, row - 1)
    - DDRAM data/text is sent & received after this setting
*/
/**************************************************************************/
void LiquidCrystal_I2C::setCursor(uint8_t column, uint8_t row)
{
  uint8_t rowAddressOffset[4] = {0x00, 0x40, (uint8_t)(_lcdColumns + 0x00), (uint8_t)(_lcdColumns + 0x40)};

  column = constrain(column, 0, (_lcdColumns - 1)); //check column value range, see NOTE
  row    = constrain(row,    0, (_lcdRows    - 1)); //check row value range, see NOTE

  _send(LCD_INSTRUCTION_WRITE, (LCD_DDRAM_ADDR_SET | (rowAddressOffset[row] + column)), LCD_CMD_LENGTH_8BIT);
}


/**************************************************************************/
/*
    noDisplay()

    Clears text from the screen

    NOTE:
    - text remains in DDRAM
*/
/**************************************************************************/
void LiquidCrystal_I2C::noDisplay()
{
  _displayControl &= ~LCD_DISPLAY_ON;

  _send(LCD_INSTRUCTION_WRITE, (LCD_DISPLAY_CONTROL | _displayControl), LCD_CMD_LENGTH_8BIT);
}


/**************************************************************************/
/*
    display()

    Retrives text from DDRAM

    NOTE:
    - text remains in DDRAM
*/
/**************************************************************************/
void LiquidCrystal_I2C::display()
{
  _displayControl |= LCD_DISPLAY_ON;

  _send(LCD_INSTRUCTION_WRITE, (LCD_DISPLAY_CONTROL | _displayControl), LCD_CMD_LENGTH_8BIT);
}


/**************************************************************************/
/*
    noCursor()

    Turns OFF underline cursor
*/
/**************************************************************************/
void LiquidCrystal_I2C::noCursor()
{
  _displayControl &= ~LCD_UNDERLINE_CURSOR_ON;

  _send(LCD_INSTRUCTION_WRITE, (LCD_DISPLAY_CONTROL | _displayControl), LCD_CMD_LENGTH_8BIT);
}


/**************************************************************************/
/*
    cursor()

    Turns ON underline cursor
*/
/**************************************************************************/
void LiquidCrystal_I2C::cursor()
{
  _displayControl |= LCD_UNDERLINE_CURSOR_ON;

  _send(LCD_INSTRUCTION_WRITE, (LCD_DISPLAY_CONTROL | _displayControl), LCD_CMD_LENGTH_8BIT);
}


/**************************************************************************/
/*
    noBlink()

    Turns OFF blinking cursor
*/
/**************************************************************************/
void LiquidCrystal_I2C::noBlink()
{
  _displayControl &= ~LCD_BLINK_CURSOR_ON;

  _send(LCD_INSTRUCTION_WRITE, (LCD_DISPLAY_CONTROL | _displayControl), LCD_CMD_LENGTH_8BIT);
}


/**************************************************************************/
/*
    blink()

    Turns ON blinking cursor
*/
/**************************************************************************/
void LiquidCrystal_I2C::blink()
{
  _displayControl |= LCD_BLINK_CURSOR_ON;

  _send(LCD_INSTRUCTION_WRITE, (LCD_DISPLAY_CONTROL | _displayControl), LCD_CMD_LENGTH_8BIT);
}


/**************************************************************************/
/*
    scrollDisplayLeft()

    Scrolls the current row to the left by one character

    NOTE:
    - call this function just before "write()" or "print()"
    - text grows from cursor to the left
*/
/**************************************************************************/
void LiquidCrystal_I2C::scrollDisplayLeft()
{
  _send(LCD_INSTRUCTION_WRITE, (LCD_CURSOR_DISPLAY_SHIFT | LCD_DISPLAY_SHIFT | LCD_SHIFT_LEFT), LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    scrollDisplayRight()

    Scrolls the current row to the right by one character

    NOTE:
    - call this function just before "write()" or "print()"
    - text & cursor grows together to the left from cursor position
*/
/**************************************************************************/
void LiquidCrystal_I2C::scrollDisplayRight()
{
  _send(LCD_INSTRUCTION_WRITE, (LCD_CURSOR_DISPLAY_SHIFT | LCD_DISPLAY_SHIFT | LCD_SHIFT_RIGHT), LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    leftToRight()

    Sets text direction from left to right
*/
/**************************************************************************/
void LiquidCrystal_I2C::leftToRight()
{
  _displayMode |= LCD_ENTRY_LEFT;

  _send(LCD_INSTRUCTION_WRITE, (LCD_ENTRY_MODE_SET | _displayMode), LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    rightToLeft()

    Sets text direction from right to left
*/
/**************************************************************************/
void LiquidCrystal_I2C::rightToLeft()
{
  _displayMode &= ~LCD_ENTRY_LEFT;

  _send(LCD_INSTRUCTION_WRITE, (LCD_ENTRY_MODE_SET | _displayMode), LCD_CMD_LENGTH_8BIT);
}

/**************************************************************************/
/*
    autoscroll()

    Autoscrolls text "rightToLeft()" or "rightToRight()" on the display

    NOTE:
    - whole text on the display shift when byte written, but cursor stays
    - same as "scrollDisplayRight()" or "scrollDisplayLeft()" but no need
      to call it the "loop()", just call it once in "setup()"
*/
/**************************************************************************/
void LiquidCrystal_I2C::autoscroll() 
{
  _displayMode |= LCD_ENTRY_SHIFT_ON;

  _send(LCD_INSTRUCTION_WRITE, (LCD_ENTRY_MODE_SET | _displayMode), LCD_CMD_LENGTH_8BIT);
}


/**************************************************************************/
/*
    noAutoscroll()

    Stops text autoscrolling on the display

    NOTE:
    - whole text on the display stays, cursor shifts when byte written
*/
/**************************************************************************/
void LiquidCrystal_I2C::noAutoscroll()
{
  _displayMode &= ~LCD_ENTRY_SHIFT_ON;

  _send(LCD_INSTRUCTION_WRITE, (LCD_ENTRY_MODE_SET | _displayMode), LCD_CMD_LENGTH_8BIT);
}


/**************************************************************************/
/*
    createChar()

    Fills 64-bytes CGRAM, with custom characters from MCU dynamic memory

    NOTE:
    - by default there is no array boundary check & system will
      crash if you call outside of array addressing, the sefer method
      to call like this:
      - "createChar(address, myChar, (sizeof(myChar) / sizeof(myChar[0])))"
        or
        "createChar(address, myChar, (sizeof(myChar) / sizeof(uint8_t)))"

    - 5x8DOTS display:
      - 8 custom characters (patterns)
      - 5-pixels x 8-rows characters size 
      - write address range 0..7 
      - read  address range 0..7 (8..15 also work)

    - 5x10DOTS display:
      - 4 custom characters (patterns)
      - 5-pixels x 10-rows characters size 
      - write address range 0..3
      - read  address range 0..3 (0..7 also work)

    - array size:
      int16_t arr[] = {10,20,30,40,50,60};
        return sizeof(arr);                //24
        return sizeof(arr)/sizeof(arr[0]); //6
        return sizeof(arr[0]);             //2
*/
/**************************************************************************/
void LiquidCrystal_I2C::createChar(uint8_t cgramAddress, uint8_t *cgramChar, uint8_t cgramCharSize)
{
  switch (_lcdFontSize)
  {
    case LCD_5x8DOTS:
      cgramAddress  = constrain(cgramAddress,  0, 7);  //check CGRAM write address range, see NOTE
      cgramCharSize = constrain(cgramCharSize, 0, 8);  //check 5x8DOTS rows quantity, maximum 8-rows
      break;

    case LCD_5x10DOTS:
      cgramAddress  = constrain(cgramAddress,  0, 3);  //check CGRAM write address range, see NOTE
      cgramCharSize = constrain(cgramCharSize, 0, 10); //check 5x10DOTS rows quantity, maximum 10-rows
      break;
  }

  _send(LCD_INSTRUCTION_WRITE, (LCD_CGRAM_ADDR_SET | (cgramAddress << 3)), LCD_CMD_LENGTH_8BIT); //set custom character CGRAM address

  for (uint8_t i = 0; i < cgramCharSize; i++)                                                    //write custom character row by row 
  {
    _send(LCD_DATA_WRITE, cgramChar[i], LCD_CMD_LENGTH_8BIT);                                    //write row from MCU RAM to CGRAM address
  }
}


/**************************************************************************/
/*
    createChar()

    Fills 64-bytes CGRAM, with custom characters from MCU flash memory

    NOTE:
    - see "createChar(uint8_t cgramAddress, uint8_t *cgramChar)"
*/
/**************************************************************************/
#if defined (PROGMEM)
void LiquidCrystal_I2C::createChar(uint8_t cgramAddress, const uint8_t *cgramChar, uint8_t cgramCharSize)
{
  switch (_lcdFontSize)
  {
    case LCD_5x8DOTS:
      cgramAddress  = constrain(cgramAddress,  0, 7);  //check CGRAM write address range, see NOTE
      cgramCharSize = constrain(cgramCharSize, 0, 8);  //check 5x8DOTS rows quantity, maximum 8-rows
      break;

    case LCD_5x10DOTS:
      cgramAddress  = constrain(cgramAddress,  0, 3);  //check CGRAM write address range, see NOTE
      cgramCharSize = constrain(cgramCharSize, 0, 10); //check 5x10DOTS rows quantity, maximum 10-rows
      break;
  }

  _send(LCD_INSTRUCTION_WRITE, (LCD_CGRAM_ADDR_SET | (cgramAddress << 3)), LCD_CMD_LENGTH_8BIT); //set custom character CGRAM address

  for (uint8_t i = 0; i < cgramCharSize; i++)                                                    //write custom character row by row 
  {
    _send(LCD_DATA_WRITE, pgm_read_byte(&cgramChar[i]), LCD_CMD_LENGTH_8BIT);                    //write row from MCU flash memory to LCD CGRAM address
  }
}
#endif


/**************************************************************************/
/*
    noBacklight()

    Turns off the backlight via PCF8574

    NOTE:
    - doesn't affect LCD controller, because we are working with
      transistor conncted to PCF8574 port
*/
/**************************************************************************/
void LiquidCrystal_I2C::noBacklight()
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

  _backlightValue <<= _lcdToPCF8574[0];

  _writePCF8574(PCF8574_PORTS_LOW);
}


/**************************************************************************/
/*
    backlight()

    Turns on backlight via PCF8574

    NOTE:
    - see "noBacklight()" for details
*/
/**************************************************************************/
void LiquidCrystal_I2C::backlight()
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

  _backlightValue <<= _lcdToPCF8574[0];

  _writePCF8574(PCF8574_PORTS_LOW);
}


/**************************************************************************/
/*
    write()

    Sends character to LCD

    NOTE:
    - replacement for Arduino "write()" in class "Print"
*/
/**************************************************************************/
size_t LiquidCrystal_I2C::write(uint8_t character)
{
  _send(LCD_DATA_WRITE, character, LCD_CMD_LENGTH_8BIT);

  return 1;
}


/**************************************************************************/
/*
    _initialization()

    Soft reset LCD & activate 4-bit interface

    NOTE:
    - for correct LCD operation it is necessary to do the internal circuit
      reset & initialization procedure

    - see 4-bit initializations procedure fig.24 on p.46 of HD44780
      datasheet and p.17 of  WH1602B/WH1604B datasheet for details
*/
/**************************************************************************/
void LiquidCrystal_I2C::_initialization()
{
  uint8_t displayFunction = 0; //don't change!!! default bits value DB7, DB6, DB5, DB4=(DL), DB3=(N), DB2=(F), DB1, DB0

  /*
     HD44780 & clones needs ~40ms after supply voltage rises above 2.7v
     some Arduino boards can start & execute code at 2.4v, we'll wait 500ms
  */
  delay(500);

  /*
     FIRST ATTEMPT: set 8-bit mode
     - wait > 4.1msec, some LCD even slower than 4.5msec
     - for Hitachi & Winstar displays
  */
  _send(LCD_INSTRUCTION_WRITE, (LCD_FUNCTION_SET | LCD_8BIT_MODE), LCD_CMD_LENGTH_4BIT);
  delay(5);

  /*
     SECOND ATTEMPT: set 8-bit mode
     - wait > 100usec
     - for Hitachi, not needed for Winstar displays
  */
  _send(LCD_INSTRUCTION_WRITE, (LCD_FUNCTION_SET | LCD_8BIT_MODE), LCD_CMD_LENGTH_4BIT);
  delayMicroseconds(200);
	
  /*
     THIRD ATTEMPT: set 8 bit mode
     - for Hitachi, not needed for Winstar displays
  */
  _send(LCD_INSTRUCTION_WRITE, (LCD_FUNCTION_SET | LCD_8BIT_MODE), LCD_CMD_LENGTH_4BIT);
  delayMicroseconds(100);
	
  /*
     FINAL ATTEMPT: set 4-bit interface
     - the Busy Flag (BF) can be checked after this instruction
  */
  _send(LCD_INSTRUCTION_WRITE, (LCD_FUNCTION_SET | LCD_4BIT_MODE), LCD_CMD_LENGTH_4BIT);

  /* sets quantity of lines */
  if (_lcdRows > 1) {displayFunction |= LCD_2_LINE;}     //line bit located at BD3 & zero/1 line by default

  /* sets font size, 5x8 by default */
  if (_lcdFontSize == LCD_5x10DOTS)
  {
    displayFunction |= LCD_5x10DOTS;                     //font bit located at BD2

    if (_lcdRows != 1) {displayFunction &= ~LCD_2_LINE;} //safety check, 2-rows displays can't display 10-pixels hight font
  }

  /* initializes LCD functions: quantity of lines, font size, etc., this settings can't be changed after this point */
  _send(LCD_INSTRUCTION_WRITE, (LCD_FUNCTION_SET | LCD_4BIT_MODE | displayFunction), LCD_CMD_LENGTH_8BIT);
	
  /* initializes LCD controls: turn display off, underline cursor off & blinking cursor off */
  _displayControl = LCD_UNDERLINE_CURSOR_OFF | LCD_BLINK_CURSOR_OFF;
  noDisplay();

  /* clear display */
  clear();

  /* initializes LCD basics: sets text direction "left to right" & cursor movement to the right */
  _displayMode = LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_OFF;
  _send(LCD_INSTRUCTION_WRITE, (LCD_ENTRY_MODE_SET | _displayMode), LCD_CMD_LENGTH_8BIT);

  display();
}


/************************* !!! bonus function !!! *************************/
/*
    printHorizontalGraph()

    Prints linear scale horizontal graph
*/
/**************************************************************************/
void LiquidCrystal_I2C::printHorizontalGraph(char name, uint8_t row, uint16_t setValue, uint16_t maxValue)
{
 /* get quantity of "solid squares" */
  setValue = constrain(setValue, 0, maxValue);           //safety check, to prevent ESP8266 crash

  setValue = map(setValue, 0, maxValue, 0, _lcdColumns); //quantity of "solid squares"

  /* print bar name */
  setCursor(0, row);

  _send(LCD_DATA_WRITE, name, LCD_CMD_LENGTH_8BIT);      //print graph name at column 0

  /* print horizontal bar */
  for (uint8_t i = 1; i < setValue; i++)
  {
    setCursor(i, row);

    _send(LCD_DATA_WRITE, 0xFF, LCD_CMD_LENGTH_8BIT);    //0xFF=built in "solid square" symbol, see p.17 & p.30 of HD44780 datasheet
  }

  /* fill the rest with spaces */
  while (setValue++ < _lcdColumns)
  {
    _send(LCD_DATA_WRITE, 0x20, LCD_CMD_LENGTH_8BIT);    //0x20=built in "space" symbol, see p.17 & p.30 of HD44780 datasheet
  }
}


/************************* !!! bonus function !!! *************************/
/*
    displayOff()

    Turns off backlight via PCF8574 & clears text from the screen

    NOTE:
    - text remains in DDRAM
*/
/**************************************************************************/
void LiquidCrystal_I2C::displayOff()
{
  noBacklight();
  
  noDisplay();
}


/************************* !!! bonus function !!! *************************/
/*
    displayOn()

    Turns on backlight via PCF8574 & shows text from DDRAM
*/
/**************************************************************************/
void LiquidCrystal_I2C::displayOn()
{
  display();

  backlight();
}


/************************* !!! bonus function !!! *************************/
/*
    setBrightness()

    NOTE:
    - remove "LED" jumper from PCF8574 expansion board & connect top pin
      to Arduino PWM pin in series with 470 Ohm resistor

    - ESP8266 doesn't have hardware PWM, with PWM output frequecy at
      40KHz CPU is very loaded!!!

    - ESP32 & ESP32-xx has from 6..16 hardware PWM channels

    - recomended voltage on "LED" top pin 0.50v..4.5v 
*/
/**************************************************************************/
#if defined (ARDUINO_ARCH_ESP32)
void LiquidCrystal_I2C::setBrightness(uint8_t pin, uint8_t value, uint8_t channel)
#else
void LiquidCrystal_I2C::setBrightness(uint8_t pin, uint8_t value)
#endif
{
  #if !defined (ARDUINO_ARCH_ESP32)
  pinMode(pin, OUTPUT);
  #endif

  #if defined (ARDUINO_ARCH_ESP8266)
//analogWriteResolution(8);                                  //set PWM resolution 4-bit(15)..16-bit(65535), default 8-bit(256)
//analogWriteFreq(1000);                                     //set ESP8266 PWM frequecy 100Hz..40KHz, default 1000Hz
  #elif defined (ARDUINO_ARCH_ESP32)
  ledcAttachPin(pin, channel);                               //assign pin to PWM channel xx
  ledcSetup(channel, 1000, 8);                               //set PWM channel xx to 1KHz period, 8-bit(256) resolution
  #elif defined (ARDUINO_ARCH_STM32)
//analogWriteResolution(8);                                  //set PWM resolution 8-bit(256)..16-bit(65535), default 8-bit(256)
//analogWriteFrequency(1000);                                //set PWM frequecy, default 1000Hz
  #elif defined (ARDUINO_ARCH_SAMD)
//analogWriteResolution(8);                                  //set PWM resolution 8-bit(256)..16-bit(65535), default 8-bit(256)
  #endif

  if (_backlightPolarity == NEGATIVE) {value = 255 - value;} //256=8-bit PWM

  #if defined (ARDUINO_ARCH_ESP32)
  ledcWrite(channel, value);                                 //set duty cycle for PWM channel xx
  #else
  analogWrite(pin, value);                                   //set duty cycle for pin
  #endif
}


/**************************************************************************/
/*
    _send()

    The most advanced & fastest way to sent COMMAND or DATA/TEXT to LCD
    
    NOTE:
    - all inputs formated as follow: 
      - mode : RS,RW,E=1,DB7,DB6,DB5,DB4,BCK_LED=0
      - value: DB7,DB6,DB5,DB4,DB3,DB2,DB1,DB0

    - command duration for HD44780 & clones varies 37usec..43usec
    - En pulse duration > 450nsec
*/
/**************************************************************************/
void LiquidCrystal_I2C::_send(uint8_t mode, uint8_t value, uint8_t cmdLength)
{
  uint8_t halfByte; //LSB or MSB part of value

  /* 4-bit or 1-st part of 8-bit command */
  halfByte  = value >> 3;                      //0,0,0,DB7,DB6,DB5,DB4,DB3
  halfByte &= 0x1E;                            //0,0,0,DB7,DB6,DB5,DB4,BCK_LED=0 (value LBS)
  halfByte  = _portMapping(mode | halfByte);   //RS,RW,E=1,DB7,DB6,DB5,DB4,BCK_LED=0

  _writePCF8574(halfByte);                     //send command
                                               //En pulse duration > 450nsec
  bitClear(halfByte, _lcdToPCF8574[5]);        //RS,RW,E=0,DB7,DB6,DB5,DB4,BCK_LED=0
  _writePCF8574(halfByte);                     //execute command

  /* 2-nd part of 8-bit command */
  if (cmdLength == LCD_CMD_LENGTH_8BIT)
  {
    halfByte  = value << 1;                    //DB6,DB5,DB4,DB3,DB2,DB1,DB0,0
    halfByte &= 0x1E;                          //0,0,0,DB3,DB2,DB1,DB0,BCK_LED=0 (value MSB)
    halfByte  = _portMapping(mode | halfByte); //RS,RW,E=1,DB3,DB2,DB1,DB0,BCK_LED=0

    _writePCF8574(halfByte);                   //send command
                                               //En pulse duration > 450nsec
    bitClear(halfByte, _lcdToPCF8574[5]);      //RS,RW,E=0,DB3,DB2,DB1,DB0,BCK_LED=0
    _writePCF8574(halfByte);                   //execute command
  }

  delayMicroseconds(LCD_COMMAND_DELAY);        //command duration, see NOTE
}


/**************************************************************************/
/*
    _portMapping()

    Magic of LCD pins to PCF8574x I/O ports mapping!!!

    NOTE:
    - input value formated as:
      7   6   5  4    3    2    1    0-bit
      RS, RW, E, DB7, DB6, DB5, DB4, BCK_LED
      RS, RW, E, DB3, DB2, DB1, DB0, BCK_LED

    - LCD pin to PCF8574 ports table/array formated:
      0        1    2    3    4    5  6   7-array
      BCK_LED, DB4, DB5, DB6, DB7, E, RW, RS 

    - shifts value bits to the right PCF8574 ports:
      BCK_LED,DB4,DB5,DB6,DB7,E,RW,RS shift-> to ports position P7..P0
      BCK_LED,DB4,DB5,DB6,DB7,E,RW,RS shift-> to ports position P7..P0

    - "switch-case" is 32usec faster than
      "bitWrite(data, _lcdToPCF8574[i], bitRead(value, i));"
*/
/**************************************************************************/
uint8_t LiquidCrystal_I2C::_portMapping(uint8_t value)
{
  uint8_t data = 0x00;

  /* mapping value = RS,RW,E,DB7,DB6,DB5,DB4,BCK_LED */
  for (int8_t i = 7; i >= 0; i--)
  {
    switch (bitRead(value, i))              //"switch-case" faster & has smaller footprint than "if-else", see Atmel AVR4027 Application Note
    {
      case 1:
        data |= (0x01 << _lcdToPCF8574[i]);
        break;
    }
  }

  return data; 
}


/**************************************************************************/
/*
    _writePCF8574()

    Mix backlight with data & writes it to PCF8574 over I2C

    NOTE:
    - returned value by "Wire.endTransmission()":
      - 0, success
      - 1, data too long to fit in transmit data buffer
      - 2, received NACK on transmit of address
      - 3, received NACK on transmit of data
      - 4, other error
*/
/**************************************************************************/
void LiquidCrystal_I2C::_writePCF8574(uint8_t value)
{
  Wire.beginTransmission(_pcf8574Address);

  Wire.write(value | _backlightValue);     //mix backlight with data & write it to "wire.h" txBuffer

  Wire.endTransmission(true);              //write data from "wire.h" txBuffer to slave, true=send stop after transmission
}


/**************************************************************************/
/*
    _readPCF8574()

    Reads logic values on PCF8574 pins P0...P7

    NOTE:
    - if PCF8574 I/O is low before read, than low is always returned
      regardless of the device state connected to it, see Quasi-Bidirectional
      ports for more details
    - if PCF8574 I/O is high before read, than devices has full I/O control
*/
/**************************************************************************/
uint8_t LiquidCrystal_I2C::_readPCF8574()
{
  Wire.requestFrom((uint8_t)_pcf8574Address, (uint8_t)1, (uint8_t)true); //read 1-byte from slave to "wire.h" rxBuffer, true=send stop after transmission

  if (Wire.available() == 1) {return Wire.read();}                       //check for 1-byte in "wire.h" rxBuffer
                              return 0x00;
}

/**************************************************************************/
/*
    _readBusyFlag()

    Reads busy flag (BF)

    NOTE:
    - set RS=0 & RW=1 to retrive busy flag

    - if PCF8574 I/O is low before read, than low is always returned
      regardless of the device state connected to it, see Quasi-Bidirectional
      ports for more details
    - if PCF8574 I/O is high before read, than devices has full I/O control

    - DB7 = 1, lcd busy
      DB7 = 0, lcd ready

    - input value formated as:
      7  6  5  4  3   2   1   0-bit
      RS,RW,E,DB7,DB6,DB5,DB4,BCK_LED
      RS,RW,E,DB3,DB2,DB1,DB0,BCK_LED
*/
/**************************************************************************/
bool LiquidCrystal_I2C::_readBusyFlag()
{
  _send(LCD_BUSY_FLAG_READ, PCF8574_LCD_DATA_HIGH, LCD_CMD_LENGTH_4BIT); //set RS=0, RW=1 & input pins to HIGH, see NOTE

  return bitRead(_readPCF8574(), _lcdToPCF8574[4]);
}
