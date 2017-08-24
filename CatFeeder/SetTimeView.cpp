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
#include "SetTimeView.h"


#include "Application.h"
#include "LRPCF8523.h"
#include "Display.h"
#include "Data.h"


using namespace lr;


void SetTimeView::enter()
{
    Display::switchToView(Display::View::SetTime);
    _displayUpdateCounter = 0;
    _adjustIndex = 0;
}


void SetTimeView::handleKeyPress(KeyPad::Key key)
{
    if (key == KeyPad::Plus) {
        switch (_adjustIndex) {
            case 0: Data::now.setTime((Data::now.getHour()+1)%24, Data::now.getMinute(), 0); break;
            case 1: Data::now.setTime(Data::now.getHour(), (Data::now.getMinute()+1)%60, 0); break;
            case 2: Data::now.setDate(Data::now.getYear(), Data::now.getMonth(), Data::now.getDay()+1); break;
            case 3: Data::now.setDate(Data::now.getYear(), Data::now.getMonth()+1, Data::now.getDay()); break;
            case 4: Data::now.setDate(Data::now.getYear()+1, Data::now.getMonth(), Data::now.getDay()); break;
            default: break;
        }
        _displayUpdateCounter = 0;
    } else if (key == KeyPad::Minus) {
        switch (_adjustIndex) {
            case 0: Data::now.setTime((Data::now.getHour()-1)%24, Data::now.getMinute(), 0); break;
            case 1: Data::now.setTime(Data::now.getHour(), (Data::now.getMinute()-1)%60, 0); break;
            case 2: Data::now.setDate(Data::now.getYear(), Data::now.getMonth(), Data::now.getDay()-1); break;
            case 3: Data::now.setDate(Data::now.getYear(), Data::now.getMonth()-1, Data::now.getDay()); break;
            case 4: Data::now.setDate(Data::now.getYear()-1, Data::now.getMonth(), Data::now.getDay()); break;
            default: break;
        }
        _displayUpdateCounter = 0;
    } else if (key == KeyPad::Select) {
        ++_adjustIndex;
        if (_adjustIndex >= 6) {
            // Adjust the time
            PCF8523::setDateTime(Data::now);
            Application::switchToState(Application::State::Status);
        }
        _displayUpdateCounter = 0;
    } else if (key == KeyPad::Exit) {
        Application::switchToState(Application::State::Status);
    }
}


void SetTimeView::loop()
{
    if ((_displayUpdateCounter & 0x1f) == 0) {
        Display::updateSetTime(_adjustIndex, ((_displayUpdateCounter & 0x20) == 0));
    }
    ++_displayUpdateCounter;
}


