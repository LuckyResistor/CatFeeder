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


#include <stdint.h>


/// The component for the display of the device.
///
namespace Display {


/// The current view of the display.
///
enum class View {
    Off, ///< The display is disabled.
    Welcome, ///< The view with the welcome message.
    Status, ///< The view to display the current status.
    Menu, ///< The menu to configure the device.
    SetTime, ///< The view to adjust the time.
    SetAlarm, ///< The view to adjust the alarm.
    FeederTest, ///< The view to test the feeder mechanism.
    KeyTest, ///< The view to test the key pad.
    FixHw, ///< The view with the message about hardware problem.
    Alarm, ///< The alarm view.
};


/// Initialise this component.
///
void begin();

/// Switch to a given view.
///
void switchToView(View view);

/// Save some power.
///
void setPowerSave(bool enabled);

/// Update/show the status display.
///
void updateStatus(bool blinkState);

/// Update the displayed menu.
///
void updateMenu(uint8_t menuIndex);

/// Update feeder test display.
///
void updateFeederTest(bool running);

/// Show a new key in the key test display.
///
void updateKeyTest(char c);

/// Show the reset progress in the display.
///
void updateFixHw();

/// Show the alarm is done.
///
void updateAlarm();

/// Update the set time view.
///
void updateSetTime(uint8_t adjustIndex, bool blinkState);

/// Update the set alarm view.
///
void updateSetAlarm(uint8_t adjustIndex, bool blinkState);


/// Read the buttons on the display.
///
/// Used by the `KeyPad` interface.
///
uint8_t readButtons();


}