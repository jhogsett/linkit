#ifndef EVENTS_H
#define EVENTS_H

// can an event be manually fired from a macro?
//   might want to broadcast something
#define EVENT_MANUAL          0

#define EVENT_BLINK_OFF       1
#define EVENT_BREATHE_OFF     2
#define EVENT_NO_EMPTY_FOUND  3
#define EVENT_NO_FILLED_FOUND 4

// for this, might want a way to cancel default behavior
// maybe as simple as: the event does whatever is in the macro and simply skips doing the reset
//  and the macro can use rst if it needs to for default behavior
#define EVENT_RESET 5

// for this, might want to know what zone the rotation was completed for
#define EVENT_ROTATION 6

// offset and/or window changed?

// for this, might want to know what the new brightness level is
//  but that can be queried
#define EVENT_LEVEL 7

// insertion? (color pushed)

// on pause? on continue?

// can more than one macro be called on an event?
//  could handle with macro that calls other macros

// palette changed?

// black level changed?

// blink rate changed? breathe rate changed?

// could be handled as listeners

#endif

