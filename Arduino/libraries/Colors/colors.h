#ifndef COLORS_H
#define COLORS_H

#include <PololuLedStrip.h>

#define BRIGHTNESS_DIVISOR 20.0

// Standard colors
rgb_color black = {0, 0, 0};
rgb_color gray = {10, 10, 10};
rgb_color white = {20, 20, 20};
rgb_color red = {20, 0, 0};
rgb_color orange = {20, 10, 0};
rgb_color yellow = {20, 20, 0};
rgb_color ltgreen = {10, 20, 0};
rgb_color green = {0, 20, 0};
rgb_color seafoam = {0, 20, 10};
rgb_color cyan = {0, 20, 20};
rgb_color ltblue = {0, 10, 20};
rgb_color blue = {0, 0, 20};
rgb_color purple = {10, 0, 20};
rgb_color magenta = {20, 0, 20};
rgb_color pink = {20, 0, 10};
rgb_color dkgray = {5, 5, 5};

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
