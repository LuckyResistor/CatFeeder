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
#include "SetAlarmView.h"


#include "Application.h"
#include "Display.h"
#include "Data.h"
#include "LRPCF8523.h"


using namespace lr;


void SetAlarmView::enter()
{
    Display::switchToView(Display::View::SetAlarm);
    _displayUpdateCounter = 0;
    _adjustIndex = 0;
}


void SetAlarmView::handleKeyPress(KeyPad::Key key)
{
    if (key == KeyPad::Plus) {
        switch (_adjustIndex) {
        case 0:
            ++Data::alarmHour;
            if (Data::alarmHour >= 24) {
                Data::alarmHour = 0;
            }
            break;
        case 1:
            ++Data::alarmMinute;
            if (Data::alarmMinute >= 60) {
                Data::alarmMinute = 0;
            }
            break;
        default:
            break;
        }
        _displayUpdateCounter = 0;
    } else if (key == KeyPad::Minus) {
        switch (_adjustIndex) {
        case 0:
            --Data::alarmHour;
            if (Data::alarmHour >= 24) {
                Data::alarmHour = 23;
            }
            break;
        case 1:
            --Data::alarmMinute;
            if (Data::alarmMinute >= 60) {
                Data::alarmMinute = 59;
            }
            break;
        default: break;
        }
        _displayUpdateCounter = 0;
    } else if (key == KeyPad::Select) {
        ++_adjustIndex;
        if (_adjustIndex >= 3) {
            // Adjust the alarm
            PCF8523::setAlarmMinute(Data::alarmMinute);
            PCF8523::setAlarmHour(Data::alarmHour);
            // Set the alarm mode.
            PCF8523::setAlarmMode(PCF8523::AlarmMode::HourMinute);
            // Make sure the alarm interrupt is enabled.
            PCF8523::enableInterrupt(PCF8523::Interrupt::Alarm);
            // Clear the interrupt flag if it got already set.
            PCF8523::clearAlarm();
            // Back to the status view
            Application::switchToState(Application::State::Status);
        }
        _displayUpdateCounter = 0;
    } else if (key == KeyPad::Exit) {
        Application::switchToState(Application::State::Status);
    }
}


void SetAlarmView::loop()
{
    if ((_displayUpdateCounter & 0x1f) == 0) {
        Display::updateSetAlarm(_adjustIndex, ((_displayUpdateCounter & 0x20) == 0));
    }
    ++_displayUpdateCounter;
}


