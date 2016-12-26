#ifndef COLORS_H
#define COLORS_H

#define USE_PROGMEM

// eanble to support palettes
// to do: disable palettes
#define USE_PALETTES

#define RED     (*Colors::get_color(Colors::red))
#define ORANGE  (*Colors::get_color(Colors::orange))
#define YELLOW  (*Colors::get_color(Colors::yellow))
#define GREEN   (*Colors::get_color(Colors::green))
#define BLUE    (*Colors::get_color(Colors::blue))
#define PURPLE  (*Colors::get_color(Colors::purple))
#define CYAN    (*Colors::get_color(Colors::cyan))
#define MAGENTA (*Colors::get_color(Colors::magenta))
#define LTBLUE  (*Colors::get_color(Colors::ltblue))
#define LTGREEN (*Colors::get_color(Colors::ltgreen))
#define SEAFOAM (*Colors::get_color(Colors::seafoam))
#define PINK    (*Colors::get_color(Colors::pink))
#define BLACK   (*Colors::get_color(Colors::black))
#define DKGRAY  (*Colors::get_color(Colors::dkgray))
#define GRAY    (*Colors::get_color(Colors::gray))
#define WHITE   (*Colors::get_color(Colors::white))

// to do: consider higher divisor
#define BRIGHTNESS_DIVISOR 20.0
#define NUM_COLORS 16
#define NPRETTY_COLORS 12

#include <PololuLedStrip.h>

#ifdef PROGMEM
// todo: do I need []?
const rgb_color color_0[] PROGMEM = {20,  0,  0};
const rgb_color color_1[] PROGMEM = {20, 10,  0};
const rgb_color color_2[] PROGMEM = {20, 20,  0};
const rgb_color color_3[] PROGMEM = { 0, 20,  0};
const rgb_color color_4[] PROGMEM = { 0,  0, 20};
const rgb_color color_5[] PROGMEM = {10,  0, 20};
const rgb_color color_6[] PROGMEM = { 0, 20, 20};
const rgb_color color_7[] PROGMEM = {20,  0, 20};
const rgb_color color_8[] PROGMEM = { 0, 10, 20};
const rgb_color color_9[] PROGMEM = {10, 20,  0};
const rgb_color color_10[] PROGMEM = { 0, 20, 10};
const rgb_color color_11[] PROGMEM = {20,  0, 10};
const rgb_color color_12[] PROGMEM = { 0,  0,  0};
const rgb_color color_13[] PROGMEM = { 5,  5,  5};
const rgb_color color_14[] PROGMEM = {10, 10, 10};
const rgb_color color_15[] PROGMEM = {20, 20, 20};

const rgb_color* const color_array[] PROGMEM = {
  color_0, color_1,  color_2,  color_3,  color_4,  color_5,  color_6,  color_7,
  color_8, color_9, color_10, color_11, color_12, color_13, color_14, color_15
};

#else

const rgb_color red = {20, 0, 0};
const rgb_color orange = {20, 10, 0};
const rgb_color yellow = {20, 20, 0};
const rgb_color green = {0, 20, 0};
const rgb_color blue = {0, 0, 20};
const rgb_color purple = {10, 0, 20};
const rgb_color cyan = {0, 20, 20};
const rgb_color magenta = {20, 0, 20};
const rgb_color ltblue = {0, 10, 20};
const rgb_color ltgreen = {10, 20, 0};
const rgb_color seafoam = {0, 20, 10};
const rgb_color pink = {20, 0, 10};
const rgb_color black = {0, 0, 0};
const rgb_color dkgray = {5, 5, 5};
const rgb_color gray = {10, 10, 10};
const rgb_color white = {20, 20, 20};

const rgb_color* color_array[] = {
  &black,   &red,    &orange,  &yellow,  &green, &blue,   &purple, &cyan,
  &magenta, &ltblue, &ltgreen, &seafoam, &pink,  &dkgray, &gray,   &white
};
#endif

class Colors
{
  public:
  enum color{
    red,
    orange,
    yellow,
    green,
    blue,
    purple,
    cyan,
    magenta,
    ltblue,
    ltgreen,
    seafoam,
    pink,
    black,
    dkgray,
    gray,
    white
  };

  static const rgb_color * const get_color(color c);

  private:
#ifdef PROGMEM
  static rgb_color static_color;
#endif
};

#ifdef PROGMEM
rgb_color Colors::static_color = {0,0,0};

const rgb_color * const Colors::get_color(color c){
  void *p = (void*)pgm_read_word(&(color_array[c]));
  static_color.red =   pgm_read_byte(p + 0);
  static_color.green = pgm_read_byte(p + 1);
  static_color.blue =  pgm_read_byte(p + 2);
  return &static_color;
}
#else
const rgb_color * const Colors::get_color(color c){
  return color_array[c];
}
#endif

//#ifdef USE_PALETTES
//#define NPALETTE 16
//#define NPRETTY_COLORS 12     // red group                    green group                        blue group                    white group
//rgb_color palette[NPALETTE]; // = { red, orange, magenta, pink,   yellow, ltgreen, green, seafoam,   cyan, ltblue, blue, purple,   white, black, gray, dkgray };
//rgb_color adjusted_palette[NPALETTE];

//#define NGROUPS 3 // 4
//#define GROUP_RED_START 0
//#define GROUP_RED_MAX 4
//#define GROUP_GREEN_START GROUP_RED_MAX
//#define GROUP_GREEN_MAX 8
//#define GROUP_BLUE_START GROUP_GREEN_MAX
//#define GROUP_BLUE_MAX 12
//#define GROUP_WHITE_START GROUP_BLUE_MAX
//#define GROUP_WHITE_MAX NPALETTE
//
//int group_start[4] = {GROUP_RED_START,GROUP_GREEN_START,GROUP_BLUE_START,GROUP_WHITE_START};
//int group_max[4] = {GROUP_RED_MAX,GROUP_GREEN_MAX,GROUP_BLUE_MAX,GROUP_WHITE_MAX};
//#endif

#endif
