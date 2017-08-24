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


#include "LRDateTime.h"


/// Global data used by all application modules.
///
namespace Data {


/// Warnings to display.
///
enum class Warning : uint8_t {
    None,
    RtcBatteryLow,
    BatteryLow
};


/// The current date time.
///
extern lr::DateTime now;

/// The alarm hour.
///
extern uint8_t alarmHour;

/// The alarm minute.
///
extern uint8_t alarmMinute;

/// Warnings to display.
///
extern Warning warning;


}