#pragma once

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <util/delay.h>
#include <util/twi.h>

#define DO_PRAGMA(x) _Pragma(#x)
#define TODO(x)      DO_PRAGMA(message("TODO - " #x))

/* Helper macros/functions */

#define SET_BIT(_mask, _bitno)    (_mask |= _BV(_bitno))
#define UNSET_BIT(_mask, _bitno)  (_mask &= ~_BV(_bitno))
#define TOGGLE_BIT(_mask, _bitno) (_mask ^= _BV(_bitno))
