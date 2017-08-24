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
#include "StatusView.h"


#include "Application.h"
#include "LRPCF8523.h"
#include "Display.h"
#include "Data.h"
#include "Hardware.h"


using namespace lr;


namespace {
const uint16_t cDisplayUpdateWithWarning = 25; // Every ~500ms
const uint16_t cDisplayUpdateNoWarning = 500; // Every 10s
}


void StatusView::enter()
{
    // Reset the (not initialised) display counter.
    _displayUpdateCounter = 0;
    _blinkState = false;
    // Read the current alarm time from the RTC.
    Data::alarmHour = PCF8523::getAlarmHour();
    Data::alarmMinute = PCF8523::getAlarmMinute();
}


void StatusView::handleKeyPress(KeyPad::Key key)
{
    if (key == KeyPad::Select) {
        Application::switchToState(Application::State::Menu);
    }
}


void StatusView::loop()
{
    bool displayRefresh = false;
    // Check for warnings every ~500ms
    if ((_displayUpdateCounter & 0x001f) == 0) {
        // Check for an alarm.
        if (PCF8523::isAlarm()) {
            // First, clear the alarm.
            PCF8523::clearAlarm();
            // Check if the time matches.
            Data::now = PCF8523::getDateTime();
            if (Data::now.getHour() == Data::alarmHour && Data::now.getMinute() == Data::alarmMinute) {
                // Switch to the alarm state.
                Application::switchToState(Application::State::Alarm);
                return;
            }            
        }
        // Check the battery levels.
        if (PCF8523::isBackupBatteryLow()) {
            Data::warning = Data::Warning::RtcBatteryLow;
        } else if (Hardware::isBatteryLow()) {
            Data::warning = Data::Warning::BatteryLow;
        } else {
            Data::warning = Data::Warning::None;
        }
        if (Data::warning != Data::Warning::None) {
            displayRefresh = true;
        }
    }
    // Update the time from RTC every ~10s
    if ((_displayUpdateCounter & 0x01ff) == 0) {
        // Get the current date/time.
        Data::now = PCF8523::getDateTime();
        displayRefresh = true;
    }
    // Refresh the display if necessary.
    if (displayRefresh) {
        Display::updateStatus(_blinkState);
        _blinkState = !_blinkState;
    }
    ++_displayUpdateCounter;
}
