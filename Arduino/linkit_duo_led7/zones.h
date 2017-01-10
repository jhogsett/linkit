#ifndef ZONES_H
#define ZONES_H

// todo: turn into a dependency class

#if defined(DISC93) || defined(DISC93_AND_STRIP)
#define NUM_ZONES 11
// all, 6 rings starting in center, first three rings, last three rings, rings 2-5, rings 1-5
byte zone_offsets[NUM_ZONES] = { 0,  0, 1,  9, 21, 37, 61,  0, 21,  1,  0};
byte zone_windows[NUM_ZONES] = { 93, 1, 9, 21, 37, 61, 93, 21, 93, 61, 61};
#elif defined(STRAND1)
#define NUM_ZONES 8
// all, two halves, center half, four quarters
byte zone_offsets[NUM_ZONES] = {  0,  0, 25, 12,  0, 12, 25, 38 };
byte zone_windows[NUM_ZONES] = { 50, 25, 50, 38, 12, 25, 38, 50 };
#elif defined(STRAND2)
#define NUM_ZONES 8
// all, two halves, center half, four quarters
byte zone_offsets[NUM_ZONES] = {   0,  0,  50, 25,  0, 25, 50,  75 };
byte zone_windows[NUM_ZONES] = { 100, 50, 100, 75, 25, 50, 75, 100 };
#elif defined(WEARABLE)
// all, two halves, center half, four quarters
#define NUM_ZONES 8
byte zone_offsets[NUM_ZONES] = { 0, 0, 4, 2, 0, 2, 4, 6 };
byte zone_windows[NUM_ZONES] = { 8, 4, 8, 6, 2, 4, 6, 8 };
#else
#define NUM_ZONES 8
// all, two halves, center half, four quarters
byte zone_offsets[NUM_ZONES] = {  0,  0, 36, 18,  0, 18, 36, 54};
byte zone_windows[NUM_ZONES] = { 72, 36, 72, 53, 18, 36, 54, 72};
#endif

#endif
