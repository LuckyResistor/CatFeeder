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
#include "KeyTestView.h"


#include "Display.h"
#include "Data.h"


void KeyTestView::enter()
{
    Display::switchToView(Display::View::KeyTest);
}


void KeyTestView::handleKeyPress(KeyPad::Key key)
{
    switch(key) {
        case KeyPad::Plus: Display::updateKeyTest('+'); break;               
        case KeyPad::Minus: Display::updateKeyTest('-'); break;               
        case KeyPad::Select: Display::updateKeyTest('S'); break;               
        case KeyPad::Exit: Display::updateKeyTest('E'); break;
        default: break;
    }
}


