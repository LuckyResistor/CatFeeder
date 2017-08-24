#pragma once
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


/// The application logic
///
namespace Application {


/// The current state of the application.
///
enum class State {
    Welcome     = 0xff, ///< The welcome message.
    Status      = 0x00, ///< The status screen with the time and alarm
    Menu        = 0x01, ///< The menu screen.
    SetTime     = 0x02, ///< The screen to set the time.
    SetAlarm    = 0x03, ///< The screen to set the alarm.
    FeederTest  = 0x04, ///< The screen to test the feeder mechanism.
    KeyTest     = 0x05, ///< The screen to test the key pads.
    FixHw       = 0x06, ///< The screen to fix any hardware configuration.
    Alarm       = 0x07, ///< The screen if an alarm is raised with display on.
};


/// Setup the application.
///
void setup();

/// The main loop of the application.
///
void loop();

/// Switch the application to another state.
///
/// This can be called from any view, but it has to be the last
/// call in the loop() method, just before the loop ends.
///
void switchToState(State state);


}

