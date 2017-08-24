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
#include "LRPCF8523.h"


#include <Wire.h>
#include <avr/pgmspace.h>


namespace lr {
namespace PCF8523 {


/// A struct to store all time related registers in one block.
///
struct DateTimeRegister {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t dayOfWeek;
    uint8_t month;
    uint8_t year;
};


/// The chip address in the I2C bus.
///
static const uint8_t cChipAddress = 0x68;

/// The mapping between interrupts and registers/bits
///
static const uint16_t cInterruptMapping[] PROGMEM = {
//    abcd : a = enable register, b = enable bit, c = flag register, d = flag bit
    0x0214, // Second
    0x0113, // Alarm
    0x0008, // Correction
    0x1217, // WatchDogTimerA
    0x1116, // CountdownTimerA
    0x1015, // CountdownTimerB
    0x2123, // BatterySwitchOver
    0x2022, // BatteryLow
};


/// The year base.
///
static uint16_t gYearBase;
 
   
/// Function to convert BCD format into binary format.
///
static inline uint8_t convertBcdToBin(const uint8_t bcd)
{
    return (bcd&0xf)+((bcd>>4)*10);
}

    
/// Function to convert binary to BCD format.
///
static inline uint8_t convertBinToBcd(const uint8_t bin)
{
    return (bin%10)+((bin/10)<<4);
}


uint8_t readRegister(Register reg)
{
    // Address the register
    Wire.beginTransmission(cChipAddress);
    Wire.write(static_cast<uint8_t>(reg));
    Wire.endTransmission();
    // Read one byte from the register.
    Wire.requestFrom(cChipAddress, 1u);
    const uint8_t data = Wire.read();
    return data;
}


void readRegister(Register reg, uint8_t *valueOut, uint8_t count)
{
    // Address the register
    Wire.beginTransmission(cChipAddress);
    Wire.write(static_cast<uint8_t>(reg));
    Wire.endTransmission();
    // Read the register values
    Wire.requestFrom(cChipAddress, count);
    for (uint8_t i = 0; i < count; ++i) {
        valueOut[i] = Wire.read();
    }
}


bool readFlag(Register reg, uint8_t bitMask)
{
    return (readRegister(reg) & bitMask) != 0;
}


void writeRegister(Register reg, uint8_t value)
{
    // Address the register and write the value
    Wire.beginTransmission(cChipAddress);
    Wire.write(static_cast<uint8_t>(reg));
    Wire.write(value);
    Wire.endTransmission();
}


void writeRegister(Register reg, const uint8_t *valueIn, uint8_t count)
{
    // Address the register and write the values
    Wire.beginTransmission(cChipAddress);
    Wire.write(static_cast<uint8_t>(reg));
    for (uint8_t i = 0; i < count; ++i) {
        Wire.write(valueIn[i]);
    }
    Wire.endTransmission();
}


void writeRegister(Register reg, uint8_t value, uint8_t mask)
{
    value &= mask; // Remove not maked bits.
    uint8_t data = readRegister(reg); // Read the old value.
    data &= (~mask); // Remove the masked bits.
    data |= value; // Add the new value.
    writeRegister(reg, data); // Write the combined value.
}


void setFlag(Register reg, uint8_t bitMask)
{
    uint8_t data = readRegister(reg); // Read the old value.
    data |= bitMask; // Set the flag.
    writeRegister(reg, data); // Write the combined value.
}


void clearFlag(Register reg, uint8_t bitMask)
{
    uint8_t data = readRegister(reg); // Read the old value.
    data &= (~bitMask); // Clear the flag
    writeRegister(reg, data); // Write the combined value.
}


void writeFlag(Register reg, uint8_t bitMask, bool enabled)
{
    if (enabled) {
        setFlag(reg, bitMask);
    } else {
        clearFlag(reg, bitMask);
    }
}

    
void begin(uint16_t yearBase)
{
    gYearBase = yearBase;
}

    
DateTime getDateTime()
{
    // Use the struct to read all registers in one batch.
    DateTimeRegister data;
    readRegister(Register::Seconds, reinterpret_cast<uint8_t*>(&data), sizeof(DateTimeRegister));
    // Convert these values into a date object.
    return DateTime::fromUncheckedValues(
        static_cast<uint16_t>(convertBcdToBin(data.year))+gYearBase,
        convertBcdToBin(data.month&0x1f),
        convertBcdToBin(data.day&0x3f),
        convertBcdToBin(data.hours&0x3f),
        convertBcdToBin(data.minutes&0x7f),
        convertBcdToBin(data.seconds&0x7f),
        data.dayOfWeek&0x7);
}

    
void setDateTime(const DateTime &dateTime)
{
    // Basic year check
    const uint16_t newYear = dateTime.getYear();
    if (newYear < gYearBase || newYear >= (gYearBase+100)) {
        return; // Ignore this call.
    }
    // Use a struct to write all registers in one batch.
    DateTimeRegister data;
    // Prepare all registers which will be written
    data.seconds = convertBinToBcd(dateTime.getSecond());
    data.minutes = convertBinToBcd(dateTime.getMinute());
    data.hours = convertBinToBcd(dateTime.getHour());
    data.day = convertBinToBcd(dateTime.getDay());
    data.dayOfWeek = dateTime.getDayOfWeek();
    data.month = convertBinToBcd(dateTime.getMonth());
    data.year = convertBinToBcd(dateTime.getYear()-gYearBase);
    // Write all registers.
    writeRegister(Register::Seconds, reinterpret_cast<uint8_t*>(&data), sizeof(DateTimeRegister));
}


uint8_t getAlarmMinute()
{
    return convertBcdToBin(readRegister(Register::MinuteAlarm) & 0b01111111);
}


uint8_t getAlarmHour()
{
    return convertBcdToBin(readRegister(Register::HourAlarm) & 0b00111111);
}


uint8_t getAlarmDay()
{
    return convertBcdToBin(readRegister(Register::DayAlarm) & 0b00111111);
}


uint8_t getAlarmDayOfWeek()
{
    return convertBcdToBin(readRegister(Register::WeekdayAlarm) & 0b00000111);  
}


void setAlarmMinute(uint8_t minute)
{
    writeRegister(Register::MinuteAlarm, convertBinToBcd(minute), 0b01111111);
}


void setAlarmHour(uint8_t hour)
{
    writeRegister(Register::HourAlarm, convertBinToBcd(hour), 0b00111111);
}


void setAlarmDay(uint8_t day)
{
    writeRegister(Register::DayAlarm, convertBinToBcd(day), 0b00111111);
}


void setAlarmDayOfWeek(uint8_t dayOfWeek)
{
    writeRegister(Register::WeekdayAlarm, convertBinToBcd(dayOfWeek), 0b00000111);
}


AlarmMode getAlarmMode()
{
    const uint8_t alarmRegisterCount = 4;
    uint8_t alarmRegister[alarmRegisterCount];
    readRegister(Register::MinuteAlarm, alarmRegister, alarmRegisterCount);
    uint8_t mode = 0;
    for (uint8_t i = 0; i < alarmRegisterCount; ++i) {
        if ((alarmRegister[i] & 0b10000000) == 0) {
            mode |= (1<<i);
        }
    }
    return static_cast<AlarmMode>(mode);
}


void setAlarmMode(AlarmMode alarmMode)
{
    const uint8_t alarmRegisterCount = 4;
    uint8_t alarmRegister[alarmRegisterCount];
    readRegister(Register::MinuteAlarm, alarmRegister, alarmRegisterCount);
    for (uint8_t i = 0; i < alarmRegisterCount; ++i) {
        if ((static_cast<uint8_t>(alarmMode) & (1<<i)) != 0) {
            // Enable this alarm by setting the highest bit to 0
            alarmRegister[i] &= 0b01111111;
        } else {
            // Disable this alarm by setting the highest bit to 1
            alarmRegister[i] |= 0b10000000;
        }
    }
    writeRegister(Register::MinuteAlarm, alarmRegister, alarmRegisterCount);
}


bool isAlarm()
{
    return readFlag(Control2::AF);
}


void clearAlarm()
{
    clearFlag(Control2::AF);
}


bool isBackupBatteryLow()
{
    return readFlag(Control3::BLF);
}


ClockFrequency getClockOutFrequency()
{
    return static_cast<ClockFrequency>(readRegister(Register::TimerAndClockOut)>>3);
}


void setClockOutFrequency(ClockFrequency freq)
{
    const uint8_t value = (static_cast<uint8_t>(freq) << 3);
    const uint8_t mask = static_cast<uint8_t>(TimerAndClockOut::COFmask);
    writeRegister(Register::TimerAndClockOut, value, mask);
}


PowerManagement getPowerManagement()
{
    return static_cast<PowerManagement>(readRegister(Register::Control3)>>5);
}


void setPowerManagement(PowerManagement powerManagement)
{
    const uint8_t value = (static_cast<uint8_t>(powerManagement) << 5);
    const uint8_t mask = static_cast<uint8_t>(Control3::PMmask);
    writeRegister(Register::Control3, value, mask);
}


bool isInterruptEnabled(Interrupt interrupt)
{
    const uint16_t entry = pgm_read_word(cInterruptMapping + static_cast<uint8_t>(interrupt));
    const Register enableRegister = static_cast<Register>(entry>>12);
    const uint8_t enableMask = (1<<((entry>>8)&0x0f));
    return (readRegister(enableRegister) & enableMask) != 0;
}


void enableInterrupt(Interrupt interrupt)
{
    const uint16_t entry = pgm_read_word(cInterruptMapping + static_cast<uint8_t>(interrupt));
    const Register enableRegister = static_cast<Register>(entry>>12);
    const uint8_t enableMask = (1<<((entry>>8)&0x0f));
    writeRegister(enableRegister, enableMask, enableMask);
}


void disableInterrupt(Interrupt interrupt)
{
    const uint16_t entry = pgm_read_word(cInterruptMapping + static_cast<uint8_t>(interrupt));
    const Register enableRegister = static_cast<Register>(entry>>12);
    const uint8_t enableMask = (1<<((entry>>8)&0x0f));
    writeRegister(enableRegister, static_cast<uint8_t>(0x00), enableMask);
}


bool isInterruptFlagSet(Interrupt interrupt)
{
    const uint16_t entry = pgm_read_word(cInterruptMapping + static_cast<uint8_t>(interrupt));
    const Register flagRegister = static_cast<Register>((entry>>4)&0x0f);
    const uint8_t flagMask = (1<<(entry&0x0f));
    return (readRegister(flagRegister) & flagMask) != 0;    
}


void clearInterruptFlag(Interrupt interrupt)
{
    const uint16_t entry = pgm_read_word(cInterruptMapping + static_cast<uint8_t>(interrupt));
    const Register flagRegister = static_cast<Register>((entry>>4)&0x0f);
    const uint8_t flagMask = (1<<(entry&0x0f));
    writeRegister(flagRegister, static_cast<uint8_t>(0x00), flagMask);
}


void reset()
{
    writeRegister(Register::Control1, 0x58);
}


void printAllRegisterValues()
{
    const uint8_t rtcRegisterCount = 0x14;
    uint8_t rtcRegister[rtcRegisterCount];
    PCF8523::readRegister(PCF8523::Register::Control1, rtcRegister, rtcRegisterCount);
    for (uint8_t i = 0; i < rtcRegisterCount; ++i) {
        if (i < 0x10) {
           Serial.print('0');
        }
        Serial.print(i, HEX);
        Serial.print(':');
        const uint8_t value = rtcRegister[i];
        for (uint8_t j = 0; j < 8; ++j) {
            Serial.print(((value&(1<<(7-j)))!=0)?'1':'0');
        }
        Serial.print(':');
        if (value < 0x10) {
           Serial.print('0');
        }
        Serial.println(value, HEX);
    }
}

    
}
}


