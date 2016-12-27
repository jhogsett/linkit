#ifndef ZONES_H
#define ZONES_H

// todo: turn into a dependency class

#if defined(DISC93)
#define NUM_ZONES 9
byte zone_offsets[NUM_ZONES] = { 0,  0, 1,  9, 21, 37, 61,  0, 21};
byte zone_windows[NUM_ZONES] = { 93, 1, 9, 21, 37, 61, 93, 21, 93};
#else
#define NUM_ZONES 7
byte zone_offsets[NUM_ZONES] = {  0,  0, 36,  0, 18, 36, 54};
byte zone_windows[NUM_ZONES] = { 72, 36, 72, 18, 36, 54, 72};
#endif

#endif
