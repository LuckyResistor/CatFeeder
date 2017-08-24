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
#include "KeyPad.h"


#include "Display.h"

#include <string.h>

#include <Arduino.h>


namespace KeyPad {


/// The number of defined keys.
///
static const uint8_t cKeyCount = 4;

/// The key masks for all tested keys.
///
static const Key cKeyMasks[cKeyCount] = { Plus, Minus, Select, Exit }; 

/// The size of the key queue.
///
static const uint8_t cKeyQueueSize = 16;

/// The initial delay until repeat starts.
///
static const uint8_t cRepeatDelay = 50;

/// The speed of the repeat.
///
static const uint8_t cRepeatSpeed = 5;


/// The last bitmask of the key states.
///
static Key _currentPressedKey = None;

/// The number of checks since a key was pressed down.
///
static uint8_t _keyPressedTimeCount = 0;

/// Flag if we are already in repeat mode.
///
static bool _repeatMode = false;

/// Key queue for received keys.
///
static uint8_t _keyQueue[16];

/// The current size of the key queue.
///
static uint8_t _currentKeyQueueSize = 0;



/// Get the current pressed key.
///
Key currentPressedKey()
{
    const uint8_t mask = Display::readButtons();
    for (uint8_t i = 0; i < cKeyCount; ++i) {
        if ((mask & cKeyMasks[i]) != 0) {
            return cKeyMasks[i];
        }
    }
    return None;
}


/// Add a key press to the queue.
///
void addKeyPress(Key key)
{
    if (_currentKeyQueueSize < cKeyQueueSize) {
        _keyQueue[_currentKeyQueueSize] = key;
        ++_currentKeyQueueSize;
    } 
}


void begin()
{
    // Nothing to do for this interface.
}


void scanKeys()
{
    auto key = currentPressedKey();
    if (_currentPressedKey != key) {
        _currentPressedKey = key;
        _keyPressedTimeCount = 0;
        _repeatMode = false;
        if (key != None) {
            addKeyPress(key);
        }
    } else if (_currentPressedKey != None) {
        ++_keyPressedTimeCount;
        if (_repeatMode && _keyPressedTimeCount > cRepeatSpeed) {
            _keyPressedTimeCount = 0;
            addKeyPress(_currentPressedKey);
        } else if (_keyPressedTimeCount > cRepeatDelay) {
            _keyPressedTimeCount = 0;
            addKeyPress(_currentPressedKey);
            _repeatMode = true;
        }
    }
}


bool hasKeyPress()
{
    return (_currentKeyQueueSize > 0);
}


Key getKeyPress()
{
    if (_currentKeyQueueSize > 0) {
        auto key = _keyQueue[0];
        memmove(_keyQueue, _keyQueue+1, cKeyQueueSize-1);
        --_currentKeyQueueSize;
        return key;
    } else {
        return None;
    }
}


}