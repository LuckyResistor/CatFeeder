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
#include "Clock.h"


#include "Configuration.h"
#include "LRPCF8523.h"

#include <avr/pgmspace.h>


using namespace lr;


namespace Clock {


static const uint8_t cCheckTable[] PROGMEM {
    // Register to check                                       Expected    Mask
    static_cast<uint8_t>(PCF8523::Register::Control1),         0b00000010, 0b10101111,
    static_cast<uint8_t>(PCF8523::Register::Control2),         0b00000000, 0b00000111,
    static_cast<uint8_t>(PCF8523::Register::Control3),         0b00100000, 0b11100011,
    static_cast<uint8_t>(PCF8523::Register::Offset),           0b00000000, 0b11111111,
    static_cast<uint8_t>(PCF8523::Register::TimerAndClockOut), 0b00111000, 0b11111111,
    0xff // End flag.
};


bool isCorrect()
{
    // check all registers as defined in the table.
    const uint8_t *entry = cCheckTable;
    while (true) {
        const uint8_t registerValue = pgm_read_byte(entry++);
        if (registerValue == 0xff) {
            break;
        }
        const uint8_t expected = pgm_read_byte(entry++);
        const uint8_t mask = pgm_read_byte(entry++);
        const uint8_t value = PCF8523::readRegister(static_cast<PCF8523::Register>(registerValue));
        if ((value & mask) != expected) {
#ifdef LR_DEBUG_ENABLED
            Serial.println(F("RTC Register Check Failed:"));
            Serial.print(F("Register: 0x"));
            Serial.println(registerValue, HEX);
            Serial.print(F("Expected: 0x"));
            Serial.println(expected, HEX);
            Serial.print(F("Mask: 0x"));
            Serial.println(mask, HEX);
            Serial.print(F("Current Value: 0x"));
            Serial.println(value, HEX);
#endif
            return false;
        }
    }
    
    // Check if the alarm mode is set correctly.
    if (PCF8523::getAlarmMode() != PCF8523::AlarmMode::HourMinute) {
#ifdef LR_DEBUG_ENABLED
        Serial.println(F("RTC Alarm Mode Check Failed:"));
        Serial.print(F("Current mode: 0x"));
        Serial.println(static_cast<int>(PCF8523::getAlarmMode()), HEX);
#endif
        return false;
    }

    return true;
}


void reset()
{
    // First do a software reset.
    // After this, most of the values are already correct.
    PCF8523::reset();
    // Fix the remaining ones.
    PCF8523::setPowerManagement(PCF8523::PowerManagement::DirectSwitchAndLowBatDetect);
    PCF8523::setClockOutFrequency(PCF8523::ClockFrequency::Disabled);
    // Make sure the alarm interrupt is enabled.
    PCF8523::enableInterrupt(PCF8523::Interrupt::Alarm);
    PCF8523::clearInterruptFlag(PCF8523::Interrupt::Alarm);
    // Set the alarm mode.
    PCF8523::setAlarmMode(PCF8523::AlarmMode::HourMinute);
    // Now reset the clock and alarm.
    PCF8523::setDateTime(DateTime(2017, 1, 1, 12, 0, 0));
    PCF8523::setAlarmMinute(0);
    PCF8523::setAlarmHour(0);
    PCF8523::setAlarmDay(1); // Not used.
    PCF8523::setAlarmDayOfWeek(0); // Not used.
}


}