#ifndef ZONES_H
#define ZONES_H

// todo: turn into a dependency class

#if defined(DUAL_STRIP)
#define NUM_ZONES 16
#define FINE_ZONES 8
// all, two halves, center half, four quarters, eight eighths
byte zone_offsets[NUM_ZONES] = {   0,  0,  72,  36,  0, 36,  72, 108,  0, 18, 36, 54, 72,  90, 108, 126 };
byte zone_windows[NUM_ZONES] = { 144, 72, 144, 108, 36, 72, 108, 144, 18, 36, 54, 72, 90, 108, 126, 144 };

#elif defined(GLASSES) || defined(WEARABLE_AND_GLASSES)
#define NUM_ZONES 12 
#define FINE_ZONES 8
// from the wearer perspective
// all, right strip, right circle, left circle, left strip, center circles only, right 1/2, left 1/2, right strip, right circle, left circle, left strip (repeated)
byte zone_offsets[NUM_ZONES] = {  0, 0, 8,  32, 56,  8,  0, 32, 0,  8, 32, 56 };
byte zone_windows[NUM_ZONES] = { 64, 8, 32, 56, 64, 56, 32, 64, 8, 32, 56, 64 };

#elif defined(DISC93) || defined(DISC93_AND_STRIP) || defined(WEARABLE_AND_DISC93) || defined(DISC93_DOUBLE)
#define NUM_ZONES 17
#define FINE_ZONES 10
// all, 6 rings starting in center, first three rings, last three rings, rings 2-5, rings 1-5
byte zone_offsets[NUM_ZONES] = { 0,  0, 1,  9, 21, 37, 61,  0, 21,  1,  0, 0, 1,  9, 21, 37, 61};
byte zone_windows[NUM_ZONES] = { 93, 1, 9, 21, 37, 61, 93, 21, 93, 61, 61, 1, 9, 21, 37, 61, 93};

#elif defined(DISC93_AND_STRAND2)
#define NUM_ZONES 11
#define FINE_ZONES 0
// all 100, 6 rings starting in center, first three rings, last three rings, rings 2-5, rings 1-5
byte zone_offsets[NUM_ZONES] = { 0,  0, 1,  9, 21, 37, 61,  0, 21,  1,  0};
byte zone_windows[NUM_ZONES] = { 100, 1, 9, 21, 37, 61, 93, 21, 93, 61, 61};

#elif defined(STRAND1)
#define NUM_ZONES 8
#define FINE_ZONES 4
// all, two halves, center half, four quarters
byte zone_offsets[NUM_ZONES] = {  0,  0, 25, 12,  0, 12, 25, 38 };
byte zone_windows[NUM_ZONES] = { 50, 25, 50, 38, 12, 25, 38, 50 };

#elif defined(STRAND2)
#define NUM_ZONES 8
#define FINE_ZONES 4
// all, two halves, center half, four quarters
byte zone_offsets[NUM_ZONES] = {   0,  0,  50, 25,  0, 25, 50,  75 };
byte zone_windows[NUM_ZONES] = { 100, 50, 100, 75, 25, 50, 75, 100 };

#elif defined(WEARABLE) || defined(WEARABLE2)
// all, two halves, center half, four quarters
#define NUM_ZONES 8
#define FINE_ZONES 4
byte zone_offsets[NUM_ZONES] = { 0, 0, 4, 2, 0, 2, 4, 6 };
byte zone_windows[NUM_ZONES] = { 8, 4, 8, 6, 2, 4, 6, 8 };

#elif defined(APOLLO_LIGHTS2) || defined(APOLLO_LIGHTS2_DEV)
// all, two halves, three thirds, 6 sixths
#define NUM_ZONES 12
#define FINE_ZONES 6
byte zone_offsets[NUM_ZONES] = {  0,  0, 45,  0, 30, 60,  0, 15, 30, 45, 60, 75 };
byte zone_windows[NUM_ZONES] = { 90, 45, 90, 30, 60, 90, 15, 30, 45, 60, 75, 90};

#elif defined(RADIUS8)
// all, two halves, middle half, four quarters, 16 rows (8 through 24)
#define NUM_ZONES 25
#define FINE_ZONES 8
byte zone_offsets[NUM_ZONES] = {   0,   0, 100,  50,  0,  50, 100, 150, 0,  8, 14, 26, 38, 52, 68,  84, 100, 116, 132, 148, 162, 174, 186, 194 };
byte zone_windows[NUM_ZONES] = { 200, 100, 200, 150, 50, 100, 150, 200, 6, 14, 26, 38, 52, 68, 84, 100, 116, 132, 148, 162, 174, 186, 194, 200 };

#elif defined(MINI_DISC_19)
// all, inner ring, middle ring, outer ring, inner two rings
#define NUM_ZONES 5
#define FINE_ZONES 0
byte zone_offsets[NUM_ZONES] = {  0, 0, 1, 7,  0 };
byte zone_windows[NUM_ZONES] = { 19, 1, 7, 19, 7 };

#else
#define NUM_ZONES 8
#define FINE_ZONES 4
// all, two halves, center half, four quarters 
byte zone_offsets[NUM_ZONES] = {  0,  0, 36, 18,  0, 18, 36, 54};
byte zone_windows[NUM_ZONES] = { 72, 36, 72, 53, 18, 36, 54, 72};
#endif

#endif

