#ifndef COLORS_H
#define COLORS_H

// eanble to support palettes
// to do: disable palettes
#define USE_PALETTES

// make into a class; put colors in an array

// to do: consider higher divisor
#define BRIGHTNESS_DIVISOR 20.0
#define NUM_COLORS 16

#include <PololuLedStrip.h>

class Colors
{
  public:
  static rgb_color colors[NUM_COLORS];

  static const byte black = 0;
//  static const byte red = 1;
//  static const byte orange = 2;
//  static const byte yellow = 3;
//  static const byte green = 4;


//  pickup::::: figure out best way to store the colors in the class and use them

};

rgb_color Colors::colors[NUM_COLORS] = {
  { 0,  0,  0}, // black
  {20,  0,  0}, // red
  {20, 10,  0}, // orange
  {20, 20,  0}, // yellow
  { 0, 20,  0}, // green
  { 0,  0, 20}, // blue
  {10,  0, 20}, // purple
  { 0, 20, 20}, // cyan
  {20,  0, 20}, // magenta
  { 0, 10, 20}, // ltblue
  {10, 20,  0}, // ltgreen
  { 0, 20, 10}, // seafoam
  {20,  0, 10}, // pink
  { 5,  5,  5}, // dkgray
  {10, 10, 10}, // gray
  {20, 20, 20}  // white
};

// Standard colors
rgb_color black = {0, 0, 0};
rgb_color red = {20, 0, 0};
rgb_color orange = {20, 10, 0};
rgb_color yellow = {20, 20, 0};
rgb_color green = {0, 20, 0};
rgb_color blue = {0, 0, 20};
rgb_color purple = {10, 0, 20};
rgb_color cyan = {0, 20, 20};
rgb_color magenta = {20, 0, 20};
rgb_color ltblue = {0, 10, 20};
rgb_color ltgreen = {10, 20, 0};
rgb_color seafoam = {0, 20, 10};
rgb_color pink = {20, 0, 10};
rgb_color dkgray = {5, 5, 5};
rgb_color gray = {10, 10, 10};
rgb_color white = {20, 20, 20};


#ifdef USE_PALETTES
#define NPALETTE 16
#define NPRETTY_COLORS 12     // red group                    green group                        blue group                    white group
rgb_color palette[NPALETTE] = { red, orange, magenta, pink,   yellow, ltgreen, green, seafoam,   cyan, ltblue, blue, purple,   white, black, gray, dkgray };
rgb_color adjusted_palette[NPALETTE];

#define NGROUPS 3 // 4
#define GROUP_RED_START 0
#define GROUP_RED_MAX 4
#define GROUP_GREEN_START GROUP_RED_MAX
#define GROUP_GREEN_MAX 8
#define GROUP_BLUE_START GROUP_GREEN_MAX
#define GROUP_BLUE_MAX 12
#define GROUP_WHITE_START GROUP_BLUE_MAX
#define GROUP_WHITE_MAX NPALETTE

int group_start[4] = {GROUP_RED_START,GROUP_GREEN_START,GROUP_BLUE_START,GROUP_WHITE_START};
int group_max[4] = {GROUP_RED_MAX,GROUP_GREEN_MAX,GROUP_BLUE_MAX,GROUP_WHITE_MAX};
#endif

#endif
