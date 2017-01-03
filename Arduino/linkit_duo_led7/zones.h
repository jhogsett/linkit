#ifndef ZONES_H
#define ZONES_H

// todo: turn into a dependency class

#if defined(DISC93)
#define NUM_ZONES 10
// all, 6 rings starting in center, first three rings, last three rings, rings 2-5
byte zone_offsets[NUM_ZONES] = { 0,  0, 1,  9, 21, 37, 61,  0, 21,  1};
byte zone_windows[NUM_ZONES] = { 93, 1, 9, 21, 37, 61, 93, 21, 93, 61};
#elif defined(STRAND2)
#define NUM_ZONES 8
// all, two halves, center half, four quarters
byte zone_offsets[NUM_ZONES] = {   0,  0,  50, 25,  0, 25, 50,  75 };
byte zone_windows[NUM_ZONES] = { 100, 50, 100, 75, 25, 50, 75, 100 };
#else
#define NUM_ZONES 8
// all, two halves, center half, four quarters
byte zone_offsets[NUM_ZONES] = {  0,  0, 36, 18,  0, 18, 36, 54};
byte zone_windows[NUM_ZONES] = { 72, 36, 72, 53, 18, 36, 54, 72};
#endif

#endif
