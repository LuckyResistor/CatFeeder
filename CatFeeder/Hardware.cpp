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
#include "Hardware.h"


#include <Servo.h>
#include <Arduino.h>


namespace Hardware {


/// The pin where the servo is attached.
///
const uint8_t cServoPin = 9;

/// The pin where the servo power switch is attached.
///
/// The MOSFET is pulled up, so keeping the pin in High-Z mode is desired until used.
///
const uint8_t cServoPowerPin = 2;

/// Programming mode pin.
///
const uint8_t cProgrammingModePin = 3;

/// Done signal pin.
///
const uint8_t cDoneSignalPin = 4;

/// Battery low pin
///
const uint8_t cBatteryLowPin = 5;


/// Start position of the servo (cup can be filled).
///
const uint8_t cServoStartPos = 100;

/// End position of the servo (food is dropped).
///
const uint8_t cServoEndPos = 15; // >0!

/// Back position to shake the cup.
///
const uint8_t cServoBackPos = 25;

/// Slow motion time (ms)
///
const uint16_t cServoSlowMotionDuration = 2500;

/// The instance of the servo library.
///
Servo gServo;


void begin()
{
    pinMode(cBatteryLowPin, INPUT);
    pinMode(cProgrammingModePin, INPUT_PULLUP);
    digitalWrite(cDoneSignalPin, LOW);
    pinMode(cDoneSignalPin, OUTPUT);
}


void dropFood()
{
    // Enable power to the servo.
    pinMode(cServoPowerPin, OUTPUT);
    digitalWrite(cServoPowerPin, LOW);
    // Wait until the servo is ready.
    delay(200); 
    // Control the servo / drop the food.
    gServo.attach(cServoPin);
    
    // Start with a slow motion to get the right sound.
    const uint16_t motionDelay = (cServoSlowMotionDuration / (cServoStartPos-cServoEndPos));
    for (uint8_t pos = cServoStartPos; pos >= cServoEndPos; --pos) {
        gServo.write(pos);
        delay(motionDelay);
    }
    // Wait for the food to drop.
    delay(300);
    // Do a shake motion to make sure the cut gets empty
    for (uint8_t i = 0; i < 3; ++i) {
        gServo.write(cServoBackPos);
        delay(100);
        gServo.write(cServoEndPos);
        delay(100);
    }
    delay(500);
    // Move the cup back into the initial position.
    gServo.write(cServoStartPos);
    delay(2000);
    gServo.detach();
    // Cut the power from the servo.
    pinMode(cServoPowerPin, INPUT);    
}


bool isBatteryLow()
{
    return digitalRead(cBatteryLowPin);
}


bool isProgrammingMode()
{
    return !digitalRead(cProgrammingModePin);
}


void sendDoneSignal()
{
    while (true) {
        digitalWrite(cDoneSignalPin, HIGH);
        delay(10);
        digitalWrite(cDoneSignalPin, LOW);
        delay(100);
    }
}


}


