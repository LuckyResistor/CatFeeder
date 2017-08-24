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

// Hardware:
// ---------------------------------------------------------------------------
// The hardware consist of the following components:
// - Arduino Uno rev. 3
// - Adafruit RGB LCD Shield. Used as detached shield with external buttons.
// - Adafruit Data Logging Shield with the PCF8523 RTC
// - Adafruit PowerBoost 500 with removed pull-up resistor on the EN input.
// - Adafruit TPL5110 breakout board. 
// - Switch E-Switch 100DP3T1B1M1QEH On-Off-On/On-Off-On
// - 1x 2A rated p-channel MOSFET for the RC motor power.
// - 2x BSS84 p-channel MOSFET to control low battery warning LED and
//   translate the interrupt output from the RTC.
// - Custom board to connect all boards.
//
// Connections:
// - Data Logger shield on top of Arduino Uno (using I2C for RTC)
// - LCD Display detached and power +5, GND, 2xI2C connected to Arduino Uno
// - The four buttons left/right/up and down connected as detached buttons.
// - Pin 2: Controls Servo Power
// - Pin 3: Connected to Switch, connects to GND if in programming mode.
// - Pin 4: Output for the done signal. Connected to the TPL5110
// - Pin 5: Input if the battery is low.
// - Pin 9: Servo control output. 
//


#include "Application.h"


// Functions
// ---------------------------------------------------------------------------

void setup() {
    Application::setup();
}


void loop() {
    Application::loop();
}


