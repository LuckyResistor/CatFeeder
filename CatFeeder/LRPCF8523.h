#pragma once
//
// Lucky Resistor's PCF8523 Driver
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


#include <Arduino.h>

#include "LRDateTime.h"


namespace lr {

/// This is a simple driver library for the NXP PCF8523 chip.
///
/// It has very efficient getDateTime() and setDateTime() methods.
/// All other methods are implemented with comfort in mind, not
/// speed. If you need speed, please consider to implement your
/// own code for programming the chip.
///
namespace PCF8523 {


/// The alarm mode.
///
enum class AlarmMode : uint8_t {
    Off = 0b0000, ///< The alarm is disabled.
    Minute = 0b0001, ///< The minute has to match.
    HourMinute = 0b0011, ///< The minute and hour have to match.
    Hour = 0b0010, ///< The hour has to match. 
    Day = 0b0100, ///< The day has to match.
    DayOfWeek = 0b1000, ///< The day of week has to match.
    HourAndDayOfWeek = 0b1010, ///< The hour and day of week has to match.
    HourMinuteAndDayOfWeek = 0b1110, ///< The hour, minute and day of week has to match.
    All = 0b1111, ///< All registers have to match (makes no sense).
};

/// The clock out frequencies
///
enum class ClockFrequency : uint8_t {
    F32768Hz  = 0b000, ///< Clock out 32kHz
    F16384Hz  = 0b001, ///< Clock out 16kHz
    F8192Hz   = 0b010, ///< Clock out 8kHz
    F4096Hz   = 0b011, ///< Clock out 4kHz
    F1024Hz   = 0b100, ///< Clock out 1kHz
    F32Hz     = 0b101, ///< Clock out 32Hz
    F1Hz      = 0b110, ///< Clock out 1Hz
    Disabled  = 0b111, ///< Clock out disabled.
};

/// The power management configuration
///
/// See specification for the meaning of these values.
///
enum class PowerManagement : uint8_t {
    /// 
    StandartModeAndLowBatDetect = 0b000,
    DirectSwitchAndLowBatDetect = 0b001,
    OnlyOneSupplyAndLowBatDetect = 0b010,
    StandartMode = 0b100,
    DirectSwitch = 0b101,
    OnlyOneSupply = 0b111
};

/// The interrupts
///
enum class Interrupt : uint8_t {
    Second = 0, ///< The second interrupt.
    Alarm, ///< The alarm interrupt.
    Correction, ///< The correction interrupt.
    WatchDogTimerA, ///< The watchdog timer A interrupt.
    CountdownTimerA, ///< The countdown timer A interrupt.
    CountdownTimerB, ///< The countdown timer B interrupt.
    BatterySwitchOver, ///< The battery switch over interrupt.
    BatteryLow, ///< The battery low interrupt.
};

    
/// Initialize the real time clock driver.
///
/// @param yearBase The year base which is used for the RTC.
///    The RTC stores the year only with two digits, plus one
///    additional bit for the next century. If you set the
///    year base to 2000, the RTC will hold the correct time
///    for 200 years, starting from 2000-01-01 00:00:00.
///
void begin(uint16_t yearBase = 2000);

/// Get the current date/time.
///
DateTime getDateTime();

/// Set the date/time.
///
void setDateTime(const DateTime &dateTime);

/// Get the minute of the alarm.
///
uint8_t getAlarmMinute();

/// Get the hour of the alarm.
///
uint8_t getAlarmHour();

/// Get the day of the alarm.
///
uint8_t getAlarmDay();

/// Get the day of week of the alarm.
/// 
/// 0=sun, 1=mon ... 6=fri
///
uint8_t getAlarmDayOfWeek();

/// Set the hour of the alarm.
///
void setAlarmMinute(uint8_t minute);

/// Set the hour of the alarm.
///
void setAlarmHour(uint8_t hour);

/// Set the day of the alarm.
///
void setAlarmDay(uint8_t day);

/// Set the day of week of the alarm.
///
void setAlarmDayOfWeek(uint8_t dayOfWeek);

/// Get the current alarm mode.
///
AlarmMode getAlarmMode();

/// Set the alarm mode.
///
/// This controls which alarm registers are checked to generate an alarm.
///
void setAlarmMode(AlarmMode alarmMode);

/// Check if an alarm was generated.
///
bool isAlarm();

/// Clear the alarm flag.
///
void clearAlarm();

/// Check if the backup battery is low.
///
bool isBackupBatteryLow();

/// Get the current set clock out frequency.
///
ClockFrequency getClockOutFrequency();

/// Set the clock out frequency
///
void setClockOutFrequency(ClockFrequency freq);

/// Get the current power management configuration.
///
PowerManagement getPowerManagement();

/// Set the power management configuration.
///
void setPowerManagement(PowerManagement powerManagement);

/// Check if an interrupt is enabled.
///
bool isInterruptEnabled(Interrupt interrupt);

/// Enable an interrupt
///
void enableInterrupt(Interrupt interrupt);

/// Disable an Interrupt
///
void disableInterrupt(Interrupt interrupt);

/// Check if an interrupt flag was set.
///
/// /note This does not work the same way for all interrupts, 
/// please check the data sheet for details. 
///
bool isInterruptFlagSet(Interrupt interrupt);

/// Clear an interrupt flag.
///
/// /note This does not work the same way for all interrupts, 
/// please check the data sheet for details. 
///
void clearInterruptFlag(Interrupt interrupt);


/// Do a software reset
///
void reset();

/// Write all register values to the serial console.
///
void printAllRegisterValues();


// Low Level Functions
// ---------------------------------------------------------------------------

/// All registers available in the chip.
///
enum class Register : uint8_t {
    Control1 = 0x00,
    Control2 = 0x01,
    Control3 = 0x02,
    Seconds = 0x03,
    Minutes = 0x04,
    Hours = 0x05,
    Days = 0x06,
    Weekdays = 0x07,
    Months = 0x08,
    Years = 0x09,
    MinuteAlarm = 0x0a,
    HourAlarm = 0x0b,
    DayAlarm = 0x0c,
    WeekdayAlarm = 0x0d,
    Offset = 0x0e,
    TimerAndClockOut = 0x0f,
    TimerAFrequency = 0x10,
    TimerAValue = 0x11,
    TimerBFrequency = 0x12,
    TimerBValue = 0x13
};

/// All flags for control register 1
///
enum class Control1 : uint8_t {
    CIE = (1<<0),
    AIE = (1<<1),
    SIE = (1<<2),
    H12_24 = (1<<3),
    SR = (1<<4),
    STOP = (1<<5),
    CAP_SEL = (1<<7)
};

/// All flags for control register 2
///
enum class Control2 : uint8_t {
    CTBIE = (1<<0),
    CTAIE = (1<<1),
    WTAIE = (1<<2),
    AF = (1<<3),
    SF = (1<<4),
    CTBF = (1<<5),
    CTAF = (1<<6),
    WTAF = (1<<7)
};

/// All flags for control register 3
///
enum class Control3 : uint8_t {
    BLIE = (1<<0),
    BSIE = (1<<1),
    BLF = (1<<2),
    BSF = (1<<3),
    PMmask = 0b11100000
};

/// All flags for timer and clock out register.
///
enum class TimerAndClockOut : uint8_t {
    TBC = (1<<0),
    TACmask = 0b00000110,
    COFmask = 0b00111000,
    TBM = (1<<6),
    TAM = (1<<7)
};


/// Read a single register from the chip.
///
/// @param reg The register to read.
/// @return The value from the register.
///
uint8_t readRegister(Register reg);

/// Read multiple registers from the chip.
///
/// @param reg The first register to read.
/// @param valueOut An array of bytes to write the register values to.
/// @param count The number of registers to read.
///
void readRegister(Register reg, uint8_t *valueOut, uint8_t count);

/// Read a flag from a register.
///
/// @param reg The register for the flag.
/// @param bitMask The bit mask for the flag.
/// @return true if the flag is set.
///
bool readFlag(Register reg, uint8_t bitMask);

/// Write a single register to the chip.
///
/// @param reg The register to write into.
/// @param value The new value.
///
void writeRegister(Register reg, uint8_t value);

/// Write a multiple register values to the chip.
///
/// @param reg The start register for the write.
/// @param valueIn A pointer to the array of values to write.
/// @param count The number of registers to write.
///
void writeRegister(Register reg, const uint8_t *valueIn, uint8_t count);

/// Write a few bits in a single register.
/// 
/// This reads the register first, masks the result and writes the register.
///
/// @param reg The register to write into.
/// @param value The new value.
/// @param mask The mask. Each 1 bit in the mask set is written.
///
void writeRegister(Register reg, uint8_t value, uint8_t mask);

/// Write a flag to a register.
///
/// This will only do a write to the chip if the flag changes.
///
/// @param reg The register to change.
/// @param bitMask The bit mask for the flag.
/// @param enabled If the flag should be set `true` or cleared `false`.
///
void writeFlag(Register reg, uint8_t bitMask, bool enabled);

/// Set a flag in a register.
///
/// @param reg The register to change.
/// @param bitMask The bit mask for the flag.
///
void setFlag(Register reg, uint8_t bitMask);

/// Clear a flag in a register.
///
/// @param reg The register to change.
/// @param bitMask The bit mask for the flag.
///
void clearFlag(Register reg, uint8_t bitMask);


// Helper methods to simplify the code.
inline bool readFlag(Control1 flag) { return readFlag(Register::Control1, static_cast<uint8_t>(flag)); }
inline bool readFlag(Control2 flag) { return readFlag(Register::Control2, static_cast<uint8_t>(flag)); }
inline bool readFlag(Control3 flag) { return readFlag(Register::Control3, static_cast<uint8_t>(flag)); }
inline bool setFlag(Control1 flag) { setFlag(Register::Control1, static_cast<uint8_t>(flag)); }
inline bool setFlag(Control2 flag) { setFlag(Register::Control2, static_cast<uint8_t>(flag)); }
inline bool setFlag(Control3 flag) { setFlag(Register::Control3, static_cast<uint8_t>(flag)); }
inline bool clearFlag(Control1 flag) { clearFlag(Register::Control1, static_cast<uint8_t>(flag)); }
inline bool clearFlag(Control2 flag) { clearFlag(Register::Control2, static_cast<uint8_t>(flag)); }
inline bool clearFlag(Control3 flag) { clearFlag(Register::Control3, static_cast<uint8_t>(flag)); }
inline bool writeFlag(Control1 flag, bool enabled) { writeFlag(Register::Control1, static_cast<uint8_t>(flag), enabled); }
inline bool writeFlag(Control2 flag, bool enabled) { writeFlag(Register::Control2, static_cast<uint8_t>(flag), enabled); }
inline bool writeFlag(Control3 flag, bool enabled) { writeFlag(Register::Control3, static_cast<uint8_t>(flag), enabled); }


}
}


