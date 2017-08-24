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


/// Access to the keys on the device.
///
/// This implements a simple key handling mechanism for single key 
/// presses. If two keys are pressed simultaneously, always the key with the
/// higher priority is used.
///
/// Pressed keys are added to a queue, so the code can get all key presses
/// even after a short delay.
///
/// There is a repeat function enabled, so after ~1s, the pressed key 
/// is automatically repeated to allow simple interfaces to adjust time/date
/// and similar things.
///
/// /note This uses internally the LCD driver to access the button bitmask.
///
namespace KeyPad {


/// The key.
///
enum Key : uint8_t {
    None = 0,
    Plus = (1<<4),
    Minus = (1<<3),
    Select = (1<<2),
    Exit = (1<<1)   
};


/// Initialise this component.
///
void begin();

/// Check the keys.
///
/// Call this every 20-50ms.
///
void scanKeys();

/// Check if there is a key press in the queue.
///
bool hasKeyPress();

/// Get the next key from the queue.
///
Key getKeyPress();


}



