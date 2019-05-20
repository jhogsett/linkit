#ifndef ZONE_DEFS_H
#define ZONE_DEFS_H

#define FIRST_FINE_ZONE 1

// fine zones start at zone #1 and are the set of best zones for quick animations and effects

//#if defined(APOLLO_LIGHTS2) || defined(APOLLO_LIGHTS2_DEV)

#if defined(ZONES_CHEATER)

#define NUM_ZONES 11
#define FINE_ZONES 8 //                               pairs                         top & bottom  
const byte PROGMEM zone_offsets[NUM_ZONES]= {   0,    0, 2, 4, 6,  8, 10, 12, 14,   0,  8 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 16,    2, 4, 6, 8, 10, 12, 14, 16,   8, 16 };

#elif defined(ZONES_STRAND_50)
#define NUM_ZONES 6
#define FINE_ZONES 5 //                              5 zones of 10
const byte PROGMEM zone_offsets[NUM_ZONES]= {   0,    0, 10, 20, 30, 40 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 50,   10, 20, 30, 40, 50 };

#elif defined(ZONES_STRAND_100)
#define NUM_ZONES 11
#define FINE_ZONES 10 //                              10 zones of 10
const byte PROGMEM zone_offsets[NUM_ZONES]= {    0,    0, 10, 20, 30, 40, 50, 60, 70, 80,  90 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 100,   10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };

#elif defined(ZONES_ANNUNCIATOR)
#define NUM_ZONES 10
#define FINE_ZONES 3 //                              3 zones of 12    6 zones of 6
const byte PROGMEM zone_offsets[NUM_ZONES]= {   0,    0, 12, 24,       0, 6,  12, 18, 24, 30 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 36,   12, 24, 36,       6, 12, 18, 24, 30, 36 };

#elif defined(ZONES_ANNUNCIATOR2)
#define NUM_ZONES 13
#define FINE_ZONES 3 //                              3 zones of 18    9 zones of 6
const byte PROGMEM zone_offsets[NUM_ZONES]= {   0,    0, 18, 36,       0, 6,  12, 18, 24, 30, 36, 42, 48 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 54,   18, 36, 54,       6, 12, 18, 24, 30, 36, 42, 48, 54 };

#elif defined(ZONES_144_LEDS)
#define NUM_ZONES 12
#define FINE_ZONES 6 //                               6 zones of 24               2 halves  3 thirds
const byte PROGMEM zone_offsets[NUM_ZONES]= {  0,      0, 24, 48, 72,  96, 120,    0,  72,     0, 48,  96 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 144,   24, 48, 72, 96, 120, 144,   72, 144,    48, 96, 144 };

#elif defined(ZONES_OUTFIT)
#define NUM_ZONES 57
#define FINE_ZONES 4 //                               left & right coat & pants     coat   pants     50 zones of 3
const byte PROGMEM zone_offsets[NUM_ZONES]= {  0,      0, 39,  78, 114,             0,      78,      0, 3, 6,  9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 54, 57, 60, 63, 66, 69, 72, 75, 78, 81, 84, 87, 90, 93, 96,  99, 102, 105, 108, 111, 114, 117, 120, 123, 126, 129, 132, 135, 138, 141, 144, 147};
const byte PROGMEM zone_windows[NUM_ZONES] = { 150,   39, 78, 114, 150,             78,    150,      3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 54, 57, 60, 63, 66, 69, 72, 75, 78, 81, 84, 87, 90, 93, 96, 99, 102, 105, 108, 111, 114, 117, 120, 123, 126, 129, 132, 135, 138, 141, 144, 147, 150};

#elif defined(ZONES_90_LEDS)
#define NUM_ZONES 12
#define FINE_ZONES 6 //                              6 zones of 15             2 halves  3 thirds
const byte PROGMEM zone_offsets[NUM_ZONES]= {  0,     0, 15, 30, 45, 60, 75,    0, 45,     0, 30, 60 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 90,   15, 30, 45, 60, 75, 90,   45, 90,    30, 60, 90 };

#elif defined(ZONES_72_LEDS)
#define NUM_ZONES 8
#define FINE_ZONES 4 //                              4 zones of 18     halves     center half
const byte PROGMEM zone_offsets[NUM_ZONES] = {  0,    0, 18, 36, 54,    0, 36,    18 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 72,   18, 36, 54, 72,   36, 72,    53 };

#elif defined(ZONES_SPHERE)
#define NUM_ZONES 13
#define FINE_ZONES 10 //                             10 rows                                   bottom,top halves
const byte PROGMEM zone_offsets[NUM_ZONES] = {  0,   0, 1,  7, 18, 33, 50, 67, 82, 93,  99,     0,  50 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 100,  1, 7, 18, 33, 50, 67, 82, 93, 99, 100,    50, 100 };

#elif defined(ZONES_RADIUS8)
//#define NUM_ZONES 24
//#define FINE_ZONES 16 //                              16 rows                                                                  halves      quarters             middle half
//const byte PROGMEM zone_offsets[NUM_ZONES] = {   0,   0,  8, 14, 26, 38, 52, 68,  84, 100, 116, 132, 148, 162, 174, 186, 194,    0, 100,    50,  0,  50, 100,   150 };
//const byte PROGMEM zone_windows[NUM_ZONES] = { 200,   6, 14, 26, 38, 52, 68, 84, 100, 116, 132, 148, 162, 174, 186, 194, 200,  100, 200,   150, 50, 100, 150,   200  };

#define NUM_ZONES 8
#define FINE_ZONES 2 //                              halves      quarters            middle half
const byte PROGMEM zone_offsets[NUM_ZONES] = {   0,     0, 100,    50,  0,  50, 100,   150 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 200,   100, 200,   150, 50, 100, 150,   200  };

#elif defined(ZONES_DISC)
#define NUM_ZONES 11
#define FINE_ZONES 6 //                              6 rings starting in center   1st 3 rings,  last 3 rings,  rings 2-5,  rings 1-5
const byte PROGMEM zone_offsets[NUM_ZONES] = { 0,    0, 1,  9, 21, 37, 61,         0,           21,             1,          0 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 93,   1, 9, 21, 37, 61, 93,        21,           93,            61,         61 };

#elif defined(ZONES_HARDHAT) 
#define NUM_ZONES 6 
#define FINE_ZONES 4 //                              fine, bottom up                              
const byte PROGMEM zone_offsets[NUM_ZONES] = {  0,   0,  17, 31, 41 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 44,   17, 31, 41, 44 };

#elif defined(ZONES_GLASSES)
#define NUM_ZONES 8 
#define FINE_ZONES 4 // from the wearer perspective: right strip, right circle, left circle, left strip
                     //                              fine             circles only   L/R halves                                                
const byte PROGMEM zone_offsets[NUM_ZONES] = {  0,   0,  8, 32, 56,    8,            0, 32,      };
const byte PROGMEM zone_windows[NUM_ZONES] = { 64,   8, 32, 56, 64,   56,            32, 64,     };

#elif defined(ZONES_MINI_DISC)
#define NUM_ZONES 5
#define FINE_ZONES 3 //                             inner, middle, outer rings  inner + middle rings
const byte PROGMEM zone_offsets[NUM_ZONES] = {  0,  0, 1,  7,                   0 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 19,  1, 7, 19,                   7 };

#elif defined(ZONES_8_LEDS)
#define NUM_ZONES 7
#define FINE_ZONES 2 //                            l,r four, 4 2-led zones 
const byte PROGMEM zone_offsets[NUM_ZONES] = { 0,  0, 4,     0, 2, 4, 6 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 8,  4, 8,     2, 4, 6, 8 };

#elif defined(ZONES_3_X_19_LEDS)
#define NUM_ZONES 13
#define FINE_ZONES 3 //                              3 zones of 18  3X inner, middle, outer rings
const byte PROGMEM zone_offsets[NUM_ZONES]= {   0,    0, 19, 38,    0,  1,  7, 19, 20, 26, 38, 39, 45 };
const byte PROGMEM zone_windows[NUM_ZONES] = { 57,   19, 38, 57,    1,  7, 19, 20, 26, 38, 39, 45, 57 };

#endif




//#elif defined(GLASSES) || defined(WEARABLE_AND_GLASSES) 
//#define NUM_ZONES 12 
//#define FINE_ZONES 4 // from the wearer perspective: right strip, right circle, left circle, left strip
//                     //                              fine             circles only   L/R halves                                                
//const byte PROGMEM zone_offsets[NUM_ZONES] = {  0,   0,  8, 32, 56,    8,            0, 32,      };
//const byte PROGMEM zone_windows[NUM_ZONES] = { 64,   8, 32, 56, 64,   56,            32, 64,     };
//
//#elif defined(RADIUS8)
//#define NUM_ZONES 24
//#define FINE_ZONES 16 //                              16 rows                                                                  halves      quarters             middle half
//const byte PROGMEM zone_offsets[NUM_ZONES] = {   0,   0,  8, 14, 26, 38, 52, 68,  84, 100, 116, 132, 148, 162, 174, 186, 194,    0, 100,    50,  0,  50, 100,   150 };
//const byte PROGMEM zone_windows[NUM_ZONES] = { 200,   6, 14, 26, 38, 52, 68, 84, 100, 116, 132, 148, 162, 174, 186, 194, 200,  100, 200,   150, 50, 100, 150,   200  };
//
//#elif defined(DISC93) || defined(DISC93_AND_STRIP) || defined(WEARABLE_AND_DISC93) || defined(DISC93_DOUBLE)
//#define NUM_ZONES 11
//#define FINE_ZONES 6 //                              6 rings starting in center   1st 3 rings,  last 3 rings,  rings 2-5,  rings 1-5
//const byte PROGMEM zone_offsets[NUM_ZONES] = { 0,    0, 1,  9, 21, 37, 61,         0,           21,             1,          0 };
//const byte PROGMEM zone_windows[NUM_ZONES] = { 93,   1, 9, 21, 37, 61, 93,        21,           93,            61,         61 };
//
//#elif defined(SPHERE)
//
//#elif defined(DUAL_STRIP)
//#define NUM_ZONES 16
//#define FINE_ZONES 8 //                              8 eighths                                 halves     quarters
//const byte PROGMEM zone_offsets[NUM_ZONES] = {   0,   0, 18, 36, 54, 72,  90, 108, 126,         0,  72,     36,  0, 36,  72, 108 };
//const byte PROGMEM zone_windows[NUM_ZONES] = { 144,  18, 36, 54, 72, 90, 108, 126, 144,        72, 144,    108, 36, 72, 108, 144 };
//
//#elif defined(WEARABLE_AND_HARDHAT) 
//#define NUM_ZONES 6 
//#define FINE_ZONES 4 //                              fine, bottom up                              
//const byte PROGMEM zone_offsets[NUM_ZONES] = {  0,   0,  17, 31, 41 };
//const byte PROGMEM zone_windows[NUM_ZONES] = { 44,   17, 31, 41, 44 };





//
//// TODO rearrange these zones
//
//#elif defined(DISC93_AND_STRAND2)
//#define NUM_ZONES 11
//#define FINE_ZONES 0
//// all 100, 6 rings starting in center, first three rings, last three rings, rings 2-5, rings 1-5
//byte zone_offsets[NUM_ZONES] = { 0,  0, 1,  9, 21, 37, 61,  0, 21,  1,  0};
//byte zone_windows[NUM_ZONES] = { 100, 1, 9, 21, 37, 61, 93, 21, 93, 61, 61};
//
//#elif defined(STRAND1)
//#define NUM_ZONES 8
//#define FINE_ZONES 4
//// all, two halves, center half, four quarters
//byte zone_offsets[NUM_ZONES] = {  0,  0, 25, 12,  0, 12, 25, 38 };
//byte zone_windows[NUM_ZONES] = { 50, 25, 50, 38, 12, 25, 38, 50 };
//
//#elif defined(STRAND2)
//#define NUM_ZONES 8
//#define FINE_ZONES 4
//// all, two halves, center half, four quarters
//byte zone_offsets[NUM_ZONES] = {   0,  0,  50, 25,  0, 25, 50,  75 };
//byte zone_windows[NUM_ZONES] = { 100, 50, 100, 75, 25, 50, 75, 100 };
//
//#elif defined(WEARABLE) || defined(WEARABLE2)
//// all, two halves, center half, four quarters
//#define NUM_ZONES 8
//#define FINE_ZONES 4
//byte zone_offsets[NUM_ZONES] = { 0, 0, 4, 2, 0, 2, 4, 6 };
//byte zone_windows[NUM_ZONES] = { 8, 4, 8, 6, 2, 4, 6, 8 };
//
//#else defined(MINI_DISC_19)
//// all, inner ring, middle ring, outer ring, inner two rings
//#define NUM_ZONES 5
//#define FINE_ZONES 0
//byte zone_offsets[NUM_ZONES] = {  0, 0, 1, 7,  0 };
//byte zone_windows[NUM_ZONES] = { 19, 1, 7, 19, 7 };
//#endif

#endif
