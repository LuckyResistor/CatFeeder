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
#include "MenuView.h"


#include "Application.h"
#include "Display.h"
#include "Data.h"


void MenuView::enter()
{
    _currentMenuIndex = 0;
    Display::switchToView(Display::View::Menu);
}


void MenuView::handleKeyPress(KeyPad::Key key)
{
    if (key == KeyPad::Plus) {
        _currentMenuIndex = ((_currentMenuIndex - 1) & 0x3);
        Display::updateMenu(_currentMenuIndex);
    } else if (key == KeyPad::Minus) {
        _currentMenuIndex = ((_currentMenuIndex + 1) & 0x3);
        Display::updateMenu(_currentMenuIndex);
    } else if (key == KeyPad::Select) {
        switch (_currentMenuIndex) {
        case 0: 
            Application::switchToState(Application::State::SetAlarm);
            break;
        case 1: 
            Application::switchToState(Application::State::SetTime);
            break;
        case 2: 
            Application::switchToState(Application::State::FeederTest);
            break;
        case 3: 
            Application::switchToState(Application::State::KeyTest);
            break;
        default:
            break;
        }
    } else if (key == KeyPad::Exit) {
        Application::switchToState(Application::State::Status);
    }
}


