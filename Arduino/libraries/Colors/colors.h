#ifndef COLORS_H
#define COLORS_H

#include <PololuLedStrip.h>

#define NUM_COLORS 23
#define NPRETTY_COLORS 18

#define NUM_PALETTE_COLORS NPRETTY_COLORS
rgb_color palette[NUM_PALETTE_COLORS];

// in pre-rendered colors, 20 = the max brightness, so at 100 brightness, it would multiply
// 20 * (255 / 20) to get 255
#define BRIGHTNESS_DIVISOR 20.0

#define RED       (*Colors::get_color(Colors::red))
#define ORANGE    (*Colors::get_color(Colors::orange))
#define YELLOW    (*Colors::get_color(Colors::yellow))
#define GREEN     (*Colors::get_color(Colors::green))
#define BLUE      (*Colors::get_color(Colors::blue))
#define PURPLE    (*Colors::get_color(Colors::purple))
#define CYAN      (*Colors::get_color(Colors::cyan))
#define MAGENTA   (*Colors::get_color(Colors::magenta))
#define LTBLUE    (*Colors::get_color(Colors::ltblue))
#define LTGREEN   (*Colors::get_color(Colors::ltgreen))
#define SEAFOAM   (*Colors::get_color(Colors::seafoam))
#define PINK      (*Colors::get_color(Colors::pink))
#define AMBER     (*Colors::get_color(Colors::amber))
#define OLIVE     (*Colors::get_color(Colors::olive))
#define SKYBLUE   (*Colors::get_color(Colors::skyblue))
#define TURQUOISE (*Colors::get_color(Colors::turquoise))
#define LAVENDER  (*Colors::get_color(Colors::lavender))
#define ROSE      (*Colors::get_color(Colors::rose))
#define BLACK     (*Colors::get_color(Colors::black))
#define DKGRAY    (*Colors::get_color(Colors::dkgray))
#define GRAY      (*Colors::get_color(Colors::gray))
#define WHITE     (*Colors::get_color(Colors::white))
#define TUNGSTEN  (*Colors::get_color(Colors::tungsten))

// tungsten based on 20% brightness, 29,230,255:hsl became 133,70,12
// when unscaled, full strength is 255,135,23, downscaled to 0..20
// is 20,11,2

// to do: consider higher divisor
// consider: specifying colors with full RGB values as full brightness color

const rgb_color color_0 PROGMEM = {20,  0,  0}; // red
const rgb_color color_1 PROGMEM = {20, 10,  0}; // orange    0,10,20=ltblue
const rgb_color color_2 PROGMEM = {20, 20,  0}; // yellow
const rgb_color color_3 PROGMEM = { 0, 20,  0}; // green
const rgb_color color_4 PROGMEM = { 0,  0, 20}; // blue
const rgb_color color_5 PROGMEM = {10,  0, 20}; // purple  10,20,0=ltgreen
const rgb_color color_6 PROGMEM = { 0, 20, 20}; // cyan
const rgb_color color_7 PROGMEM = {20,  0, 20}; // magenta
const rgb_color color_8 PROGMEM = { 0, 10, 20}; // ltblue
const rgb_color color_9 PROGMEM = {10, 20,  0}; // ltgreen
const rgb_color color_10 PROGMEM = { 0, 20, 10}; // seafoam  20,0,10=pink
const rgb_color color_11 PROGMEM = {20,  0, 10}; // pink
const rgb_color color_12 PROGMEM = {20, 15, 0}; // amber amb    = 0,5,20 ? close=ltblue
const rgb_color color_13 PROGMEM = {15, 20, 0}; // olive olv    = 5,0,20 ? close=purple
const rgb_color color_14 PROGMEM = {0, 15, 20}; // sky blue sky = 20,5,0 ? close=orange
const rgb_color color_15 PROGMEM = {0, 20, 15}; // turquoise tur = 20,0,5 ? close=pink
const rgb_color color_16 PROGMEM = {15, 0, 20}; // lavender lav = 5,20,0 ? close=ltgreen
const rgb_color color_17 PROGMEM = {20, 0, 15}; // rose ros = 0,20,5 ? close=seafoam

const rgb_color color_18 PROGMEM = { 0,  0,  0}; // black
const rgb_color color_19 PROGMEM = { 5,  5,  5}; // dkgray
const rgb_color color_20 PROGMEM = {10, 10, 10}; // gray
const rgb_color color_21 PROGMEM = {20, 20, 20}; // white
const rgb_color color_22 PROGMEM = {20, 11, 2}; // tungsten

const rgb_color* const color_array[] PROGMEM = {
  &color_0,  &color_1,  &color_2,  &color_3,  &color_4,  &color_5,  &color_6,  &color_7,
  &color_8,  &color_9,  &color_10, &color_11, &color_12, &color_13, &color_14, &color_15,
  &color_16, &color_17, &color_18, &color_19, &color_20, &color_21, &color_22
};

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
    amber,
    olive,
    skyblue,
    turquoise,
    lavender,
    rose,
    black,
    dkgray,
    gray,
    white,
    tungsten
  };

  static const rgb_color * const get_color(color c);

  private:
  // this is pointed-to as the return value for get_color()
  static rgb_color return_color;
};

rgb_color Colors::return_color = {0,0,0};

const rgb_color * const Colors::get_color(color c){
  void *p = (void*)pgm_read_word(&(color_array[c]));
  return_color.red =   pgm_read_byte(p + 0);
  return_color.green = pgm_read_byte(p + 1);
  return_color.blue =  pgm_read_byte(p + 2);
  return &return_color;
}

void reset_palette(){
  for(byte i = 0; i < NPRETTY_COLORS; i++){
    palette[i] = *Colors::get_color((Colors::color)i);
  }
}

// from https://forum.arduino.cc/index.php?topic=345964.0
void shuffle_palette()
{
  byte last = 0;
  rgb_color temp = palette[last];
  for (byte i=0; i < NUM_PALETTE_COLORS; i++)
  {
    byte index = random(NUM_PALETTE_COLORS);
    palette[last] = palette[index];
    last = index;
  }
  palette[last] = temp;
}

// create a complimentary color by inverting the values
// within the 0 - BRIGHTNESS_DIVISOR range, since that
// represents the entire pre-rendered color definition
rgb_color complimentary_color(rgb_color color){
  return (rgb_color){
    BRIGHTNESS_DIVISOR - color.red,
    BRIGHTNESS_DIVISOR - color.green,
    BRIGHTNESS_DIVISOR - color.blue
  };
}

// to do: compliment whole palette instead of in pairs

// create pairs of complimentary colors by making each odd color
// complimentary of the previous even color
void compliment_palette()
{
  for(byte i = 0; i < NUM_PALETTE_COLORS; i += 2)
    ::palette[i+1] = complimentary_color(::palette[i]);
}

// create a random palette of complimentary color pairs
void complimentary_palette()
{
  ::reset_palette();
  ::shuffle_palette();
  ::compliment_palette();
}

#endif
