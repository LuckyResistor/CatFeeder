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
#include "Application.h"


#include "AlarmView.h"
#include "Clock.h"
#include "Configuration.h"
#include "Data.h"
#include "Display.h"
#include "Hardware.h"
#include "FeederTestView.h"
#include "FixHwView.h"
#include "KeyPad.h"
#include "KeyTestView.h"
#include "LRPCF8523.h"
#include "MenuView.h"
#include "SetAlarmView.h"
#include "SetTimeView.h"
#include "StatusView.h"

#include <Wire.h>


namespace Application {


using namespace lr;


/// The initial state after the welcome screen.
///
static const State cInitialState = State::Status; 

/// An array with all views for the application states.
///
static const View* cStateViews[] = {
    new StatusView(),
    new MenuView(),
    new SetTimeView(),
    new SetAlarmView(),
    new FeederTestView(),
    new KeyTestView(),
    new FixHwView(),
    new AlarmView(),
};

/// The loop delay in milliseconds.
///
static const uint8_t cLoopDelay = 20;

/// The maximum idle time before the application switches back to 
/// the status view.
///
static const uint16_t cMaximumIdleCount = 1500;

/// The current state of the application.
///
State _state = State::Welcome;

/// The current displayed view of the application.
///
View *_currentView = nullptr;

/// The idle counter.
///
uint16_t _idleCounter = 0;

/// Flag if power save mode is active.
///
bool _isPowerSaveActive = false;

/// Flag if we are in programming mode.
///
bool _isProgrammingMode = false;


/// Setup everything for the programming mode.
///
void setupProgrammingMode()
{
    LR_DEBUG_PRINTLN(F("Programming mode."));
    // Initialise components
    Display::begin();
    KeyPad::begin();
    
    // Display the welcome message.
    Display::switchToView(Display::View::Welcome);
    // Wait before showing the main screen.
    delay(2000);

    // Check the clock configuration.
    if (Clock::isCorrect()) {
        // Switch to the initial state.
        switchToState(cInitialState);
    } else {
        // Display the hardware problem view.
        switchToState(State::FixHw);
    }
}


/// Wakeup to activate the feeder.
///
void wakeupAndFeed()
{
    LR_DEBUG_PRINTLN(F("Wakeup to activate feeder."));
    // Initialise components
    Display::begin();
    KeyPad::begin();
    // Display the welcome message.
    Display::switchToView(Display::View::Welcome);
    delay(2000);
    // Manually display the alarm view.
    Display::switchToView(Display::View::Alarm);
    Hardware::dropFood();
    Display::updateAlarm();
    // Wait and shutdown.
    delay(2000);
}


/// Setup everything for a timer event.
///
void setupTimerEvent() {
    LR_DEBUG_PRINTLN(F("Timer event."));
    // Read the current time from the RTC.
    Data::now = PCF8523::getDateTime();
    LR_DEBUG_PRINT(F("Current date/time: "));
    LR_DEBUG_PRINTLN(Data::now.toString(DateTime::Format::Long));   
    // Check if the device was woken by an alarm.
    if (PCF8523::isAlarm()) {
        // First, clear the alarm.
        PCF8523::clearAlarm();
        LR_DEBUG_PRINTLN(F("Alarm flag was set."));
        // Read the current alarm time from the RTC.
        Data::alarmHour = PCF8523::getAlarmHour();
        Data::alarmMinute = PCF8523::getAlarmMinute();
        LR_DEBUG_PRINT(F("Alarm is set to: "));
        LR_DEBUG_PRINT(Data::alarmHour);
        LR_DEBUG_PRINT(':');
        LR_DEBUG_PRINTLN(Data::alarmMinute);
        // Check if the time matches (or if this was some old unanswered alarm).
        if (Data::now.getHour() == Data::alarmHour && Data::now.getMinute() == Data::alarmMinute) {
            // Wakeup fully to activate the feeder.
            wakeupAndFeed();
        } else {
            LR_DEBUG_PRINTLN(F("Alarm does not match current time."));
        }
    } else {
        LR_DEBUG_PRINTLN(F("Regular wakeup."));
        // This is just one of the regular timed wake-ups.
        // Shutdown as fast as possible.
    }
    // Shutdown the power of the device.
    // This call never returns.
    Hardware::sendDoneSignal();
}


void setup()
{
#ifdef LR_DEBUG_ENABLED
    Serial.begin(115200);
#endif
    // Initialise the hardware.
    Hardware::begin();
    Wire.begin();
    // Initialise the RTC library.
    PCF8523::begin(2000); // Use year 2000 as base for the time.
    // Check the mode
    if (Hardware::isProgrammingMode()) {
        setupProgrammingMode();
        _isProgrammingMode = true;
    } else {
        setupTimerEvent();
    }
}


/// The main loop for the fully powered mode.
///
void loopFullPower()
{
    // Check key presses.
    KeyPad::scanKeys();
    if (KeyPad::hasKeyPress()) {
        auto key = KeyPad::getKeyPress();
        if (_isPowerSaveActive) {
            _isPowerSaveActive = false;
            Display::setPowerSave(false);
        } else {
            _currentView->handleKeyPress(key);
        }
        _idleCounter = 0;
    }
    // Execute the loop block of the current view.
    _currentView->loop();
    // Count while the application is idle.
    if (!_isPowerSaveActive) {
        ++_idleCounter;
    }
    if (_idleCounter >= cMaximumIdleCount) {
        _idleCounter = 0;
        if (_state == State::FixHw) {
            // Ignore idle for this important message.
        } else if (_state != State::Status) {
            switchToState(State::Status);
        } else {
            Display::setPowerSave(true);
            _isPowerSaveActive = true;
        }
    }
    delay(cLoopDelay);
}


void loop()
{
    if (_isProgrammingMode) {
        loopFullPower();
    } else {
        // We should never enter the loop if not in programming mode.
        // Shutdown the power of the device.
        // This call never returns.
        Hardware::sendDoneSignal();
    }
}


void switchToState(State state)
{
    if (_state != state) {
        _state = state;
        _idleCounter = 0;
        _isPowerSaveActive = false;
        _currentView = cStateViews[static_cast<uint8_t>(state)];
        _currentView->enter();
    }
}


}

