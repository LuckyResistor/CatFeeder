//
// Lucky Resistor's Cat Feeder Project
// ---------------------------------------------------------------------------
// (c)2017 by Lucky Resistor. See LICENSE for details.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "Display.h"


#include "Data.h"

#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

#include <avr/pgmspace.h>


namespace Display {


using namespace lr;


// Custom characters for the LCD display.
const uint8_t cClockCharacterMask[] PROGMEM = {
  0b01110,
  0b11011,
  0b11011,
  0b11001,
  0b11111,
  0b11111,
  0b01110,
  0b00000,
};
const char cClockCharacter = '\x01';
const uint8_t cFeedCharacterMask[] PROGMEM = {
  0b00000,
  0b10001,
  0b10101,
  0b10101,
  0b11111,
  0b11111,
  0b11111,
  0b00000
};
const char cFeedCharacter = '\x02';
const uint8_t cArrowLeftCharacterMask[] PROGMEM = {
  0b00001,
  0b00011,
  0b00111,
  0b01111,
  0b00111,
  0b00011,
  0b00001,
  0b00000
};
const char cArrowLeftCharacter = '\x03';
const uint8_t cArrowRightCharacterMask[] PROGMEM = {
  0b10000,
  0b11000,
  0b11100,
  0b11110,
  0b11100,
  0b11000,
  0b10000,
  0b00000
};
const char cArrowRightCharacter = '\x04';
const uint8_t cDiagTRCharacterMask[] PROGMEM = {
  0b11111,
  0b01111,
  0b01111,
  0b00111,
  0b00111,
  0b00011,
  0b00011,
  0b00001,
};
const char cDiagTRCharacter = '\x05';
const uint8_t cDiagTLCharacterMask[] PROGMEM = {
  0b11111,
  0b11110,
  0b11110,
  0b11100,
  0b11100,
  0b11000,
  0b11000,
  0b10000,
};
const char cDiagTLCharacter = '\x06';
const uint8_t cBatteryCharacterMask[] PROGMEM = {
  0b01110,
  0b11011,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b11111,
  0b11111,
};
const char cBatteryCharacter = '\x07';
const uint8_t cArrowUpCharacterMask[] PROGMEM = {
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
};
const char cArrowUpCharacter = '\x08';

/// The menu texts.
///
const char cMenu1Text[] PROGMEM = "Adjust Alarm";
const char cMenu2Text[] PROGMEM = "Set Time";
const char cMenu3Text[] PROGMEM = "Test Feeder";
const char cMenu4Text[] PROGMEM = "Test Buttons";
PGM_P cMenuText[] = {cMenu1Text, cMenu2Text, cMenu3Text, cMenu4Text};

/// The global instance for the LCD display.
static Adafruit_RGBLCDShield _lcd = Adafruit_RGBLCDShield();

/// The current displayed view.
///
static View _currentView = View::Off;


/// Print a decimal number padded with an optional zero to the LCD.
///
/// @param value The value to display.
///
void lcdPrintPaddedDecimal(uint8_t value) {
  if (value < 10) {
    _lcd.print('0');
  }
  _lcd.print(value, DEC);
}


void begin()
{
    // Setup the LCD display.
    _lcd.begin(16, 2);
    // Add custom characters from the masks.
    uint8_t character[8];
    memcpy_P(character, cClockCharacterMask, 8);
    _lcd.createChar(cClockCharacter, character);
    memcpy_P(character, cFeedCharacterMask, 8);
    _lcd.createChar(cFeedCharacter, character);
    memcpy_P(character, cArrowLeftCharacterMask, 8);
    _lcd.createChar(cArrowLeftCharacter, character);
    memcpy_P(character, cArrowRightCharacterMask, 8);
    _lcd.createChar(cArrowRightCharacter, character);
    memcpy_P(character, cDiagTRCharacterMask, 8);
    _lcd.createChar(cDiagTRCharacter, character);
    memcpy_P(character, cDiagTLCharacterMask, 8);
    _lcd.createChar(cDiagTLCharacter, character);
    memcpy_P(character, cBatteryCharacterMask, 8);
    _lcd.createChar(cBatteryCharacter, character);
    memcpy_P(character, cArrowUpCharacterMask, 8);
    _lcd.createChar(cArrowUpCharacter, character);
}


void setPowerSave(bool enabled)
{
    if (enabled) {
        _lcd.setBacklight(0b000); // Off
    } else {
        _lcd.setBacklight(0b110); // Teal
    }
}


void initializeWelcome()
{
    _lcd.setBacklight(0b101); // Violet
    _lcd.print(F("   Cat Feeder"));
    _lcd.setCursor(0, 1);
    _lcd.print(F("    Welcome!"));
}


void initializeStatus()
{
    _lcd.setBacklight(0b110); // Teal
}


void menuDrawScreen(uint8_t menuIndex)
{
    _lcd.setCursor(0, 0);
    _lcd.print(F("      MENU      "));
    _lcd.setCursor(0, 1);
    _lcd.print(cArrowRightCharacter);
    _lcd.print(' ');
    const char *menuText = cMenuText[menuIndex];
    for (uint8_t i = 0; i < 12; ++i) {
        const char menuChar = pgm_read_byte(menuText); 
        if (menuChar != '\0') {
            _lcd.print(menuChar);
            ++menuText;
        } else {
            _lcd.print(' ');
        }
    }
    _lcd.print(' ');
    _lcd.print(cArrowLeftCharacter);
}


void updateMenu(uint8_t menuIndex)
{
    menuDrawScreen(menuIndex);
}


void initializeMenu()
{
    _lcd.setBacklight(0b111); // White
    menuDrawScreen(0);
}


void initializeSetTime()
{
    _lcd.setBacklight(0b100); // Blue
}


void initializeSetAlarm()
{
    _lcd.setBacklight(0b100); // Blue
}


void initializeFeederTest()
{
    _lcd.setBacklight(0b011); // Yellow
    _lcd.print(F("  Feeder Test   "));
    _lcd.setCursor(0, 1);
    _lcd.print(F("\x04 Press Select  "));
}


void updateFeederTest(bool running)
{
    if (running) {
        _lcd.setCursor(0, 1);
        _lcd.print(F("Running...      "));
    } else {
        _lcd.setCursor(0, 1);
        _lcd.print(F("\x04 Press Select  "));
    }
}


void initializeKeyTest()
{
    _lcd.setBacklight(0b101); // Pink
    _lcd.print(F("Key Pad Test:"));
    _lcd.setCursor(0, 1);
}


void initializeFixHw()
{
    _lcd.setBacklight(0b001); // Red!
    _lcd.print(F("HW Config Issue!"));
    _lcd.setCursor(0, 1);
    _lcd.print(F("Select \x04 Reset "));
    _lcd.blink();
}


void updateFixHw()
{
    _lcd.setCursor(0, 1);
    _lcd.print(F("Reset...       "));
}


void initializeAlarm()
{
    _lcd.setBacklight(0b010); // Green
    _lcd.print(F("\x02 Feeding Time \x02"));
    _lcd.setCursor(0, 1);
    _lcd.print(F("      ...       "));
}


void updateAlarm()
{
    _lcd.setCursor(0, 1);
    _lcd.print(F("     done!      "));
}


void switchToView(View view)
{
    if (_currentView != view) {
        _currentView = view;
        _lcd.clear();
        _lcd.noBlink();
        _lcd.noCursor();
        switch(view) {
        case View::Welcome: initializeWelcome(); break;
        case View::Status: initializeStatus(); break;
        case View::Menu: initializeMenu(); break;
        case View::SetTime: initializeSetTime(); break;
        case View::SetAlarm: initializeSetAlarm(); break;
        case View::FeederTest: initializeFeederTest(); break;
        case View::KeyTest: initializeKeyTest(); break;
        case View::FixHw: initializeFixHw(); break;
        case View::Alarm: initializeAlarm(); break;
        default: break;
        }
    }
}


void updateStatus(bool blinkState)
{
    switchToView(View::Status);
    
    // Date and time.
    _lcd.setCursor(0, 0);
    _lcd.print(cClockCharacter);
    _lcd.print(Data::now.toString(DateTime::Format::ShortTime));
    _lcd.print(' ');
    _lcd.print(Data::now.toString(DateTime::Format::ShortDate));
    lcdPrintPaddedDecimal(Data::now.getYear()%100);
    
    // Alarm.
    _lcd.setCursor(0, 1);
    _lcd.print(cFeedCharacter);
    lcdPrintPaddedDecimal(Data::alarmHour);
    _lcd.print(':');
    lcdPrintPaddedDecimal(Data::alarmMinute);

    // Warnings
    _lcd.setCursor(14, 1);
    if (blinkState) {
        if (Data::warning == Data::Warning::RtcBatteryLow) {
            _lcd.print(F("\x01\x07"));
        } else if (Data::warning == Data::Warning::RtcBatteryLow) {
            _lcd.print(F("\x04\x07"));
        } else {
            _lcd.print(F("  "));
        }
    } else {
        _lcd.print(F("  "));
    }
}


void updateSetTime(uint8_t adjustIndex, bool blinkState)
{
    _lcd.setCursor(0, 0);
    _lcd.print(cClockCharacter);
    lcdPrintPaddedDecimal(Data::now.getHour());
    _lcd.print(':');
    lcdPrintPaddedDecimal(Data::now.getMinute());
    _lcd.print(' ');
    lcdPrintPaddedDecimal(Data::now.getDay());
    _lcd.print('.');
    lcdPrintPaddedDecimal(Data::now.getMonth());
    _lcd.print('.');
    lcdPrintPaddedDecimal(Data::now.getYear() % 100);
    _lcd.setCursor(0, 1);
    if (blinkState) {
        switch (adjustIndex) {
        case 0: _lcd.print(F(" \x08\x08             ")); break;
        case 1: _lcd.print(F("    \x08\x08          ")); break;
        case 2: _lcd.print(F("       \x08\x08       ")); break;
        case 3: _lcd.print(F("          \x08\x08    ")); break;
        case 4: _lcd.print(F("             \x08\x08 ")); break;
        case 5: _lcd.print(F("  OK? \x04 Select  ")); break;
        }
    } else {
        _lcd.print(F("                "));
    }
}


void updateSetAlarm(uint8_t adjustIndex, bool blinkState)
{
    _lcd.setCursor(0, 0);
    _lcd.print(cFeedCharacter);
    lcdPrintPaddedDecimal(Data::alarmHour);
    _lcd.print(':');
    lcdPrintPaddedDecimal(Data::alarmMinute);
    _lcd.setCursor(0, 1);
    if (blinkState) {
        switch (adjustIndex) {
        case 0: _lcd.print(F(" \x08\x08             ")); break;
        case 1: _lcd.print(F("    \x08\x08          ")); break;
        case 2: _lcd.print(F("  OK? \x04 Select  ")); break;
        }
    } else {
        _lcd.print(F("                "));
    }
}


void updateKeyTest(char c)
{
    static uint8_t pos = 0;
    switchToView(View::KeyTest);
    if (c != ' ') {
        _lcd.setCursor(pos, 1);
        _lcd.print(c);
        _lcd.print('_');
        if (++pos >= 16) {
            pos = 0;
            _lcd.setCursor(pos, 1);
            _lcd.print('_');
        }
    }
}


uint8_t readButtons()
{
    return _lcd.readButtons();
}


}

