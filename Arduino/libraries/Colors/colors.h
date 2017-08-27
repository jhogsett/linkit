#ifndef COLORS_H
#define COLORS_H

#include <PololuLedStrip.h>

#define USE_BYTE_ARRAYS

#define NUM_COLORS 24
#define NPRETTY_COLORS 18

#define NUM_PALETTE_COLORS NPRETTY_COLORS

// in pre-rendered colors, 20 = the max brightness, so at 100 brightness, it would multiply
// 20 * (255 / 20) to get 255
#define BRIGHTNESS_DIVISOR 20.0

#define RED       (*Colors::get_color(0))
#define ORANGE    (*Colors::get_color(1))
#define YELLOW    (*Colors::get_color(2))
#define GREEN     (*Colors::get_color(3))
#define BLUE      (*Colors::get_color(4))
#define PURPLE    (*Colors::get_color(5))
#define CYAN      (*Colors::get_color(6))
#define MAGENTA   (*Colors::get_color(7))
#define LTBLUE    (*Colors::get_color(8))
#define LTGREEN   (*Colors::get_color(9))
#define SEAFOAM   (*Colors::get_color(10))
#define PINK      (*Colors::get_color(11))
#define AMBER     (*Colors::get_color(12))
#define OLIVE     (*Colors::get_color(13))
#define SKYBLUE   (*Colors::get_color(14))
#define TURQUOISE (*Colors::get_color(15))
#define LAVENDER  (*Colors::get_color(16))
#define ROSE      (*Colors::get_color(17))
#define BLACK     (*Colors::get_color(18))
#define DKGRAY    (*Colors::get_color(19))
#define GRAY      (*Colors::get_color(20))
#define WHITE     (*Colors::get_color(21))
#define TUNGSTEN  (*Colors::get_color(22))
#define NEON      (*Colors::get_color(23))

// tungsten based on 20% brightness, 29,230,255:hsl became 133,70,12
// when unscaled, full strength is 255,135,23, downscaled to 0..20
// is 20,11,2

// neon looked right at 11,255,255:hsl, buffer 133,26,0, render 254,45,0
// scaled to 0..20 = 20,5,0

// to do: consider higher divisor
// consider: specifying colors with full RGB values as full brightness color

// const byte bytes_01[] PROGMEM = {40, 30, 20};
//
// const byte* const bytes_array[] PROGMEM = {
//   bytes_01
// };

#ifdef USE_BYTE_ARRAYS
const byte color_00[] PROGMEM = {20,  0,  0};
const byte color_01[] PROGMEM = {20, 10,  0};
const byte color_02[] PROGMEM = {20, 20,  0};
const byte color_03[] PROGMEM = { 0, 20,  0};
const byte color_04[] PROGMEM = { 0,  0, 20};
const byte color_05[] PROGMEM = {10,  0, 20};
const byte color_06[] PROGMEM = { 0, 20, 20};
const byte color_07[] PROGMEM = {20,  0, 20};
const byte color_08[] PROGMEM = { 0, 10, 20};
const byte color_09[] PROGMEM = {10, 20,  0};
const byte color_10[] PROGMEM = { 0, 20, 10};
const byte color_11[] PROGMEM = {20,  0, 10};
const byte color_12[] PROGMEM = {20, 15,  0};
const byte color_13[] PROGMEM = {15, 20,  0};
const byte color_14[] PROGMEM = { 0, 15, 20};
const byte color_15[] PROGMEM = { 0, 20, 15};
const byte color_16[] PROGMEM = {15,  0, 20};
const byte color_17[] PROGMEM = {20,  0, 15};

const byte color_18[] PROGMEM = { 0,  0,  0};
const byte color_19[] PROGMEM = { 5,  5,  5};
const byte color_20[] PROGMEM = {10, 10, 10};
const byte color_21[] PROGMEM = {20, 20, 20};
const byte color_22[] PROGMEM = {20, 11,  2};
const byte color_23[] PROGMEM = {20,  5,  0};

const byte* const bytes_array[] PROGMEM = {
  color_00, color_01, color_02, color_03, color_04, color_05,
  color_06, color_07, color_08, color_09, color_10, color_11,
  color_12, color_13, color_14, color_15, color_16, color_17,
  color_18, color_19, color_20, color_21, color_22, color_23
};

#else

const rgb_color color_0 PROGMEM  = {20,  0,  0}; // red
const rgb_color color_1 PROGMEM  = {20, 10,  0}; // orange
const rgb_color color_2 PROGMEM  = {20, 20,  0}; // yellow
const rgb_color color_3 PROGMEM  = { 0, 20,  0}; // green
const rgb_color color_4 PROGMEM  = { 0,  0, 20}; // blue
const rgb_color color_5 PROGMEM  = {10,  0, 20}; // purple
const rgb_color color_6 PROGMEM  = { 0, 20, 20}; // cyan
const rgb_color color_7 PROGMEM  = {20,  0, 20}; // magenta
const rgb_color color_8 PROGMEM  = { 0, 10, 20}; // ltblue
const rgb_color color_9 PROGMEM  = {10, 20,  0}; // ltgreen
const rgb_color color_10 PROGMEM = { 0, 20, 10}; // seafoam
const rgb_color color_11 PROGMEM = {20,  0, 10}; // pink
const rgb_color color_12 PROGMEM = {20, 15, 0};  // amber
const rgb_color color_13 PROGMEM = {15, 20, 0};  // olive
const rgb_color color_14 PROGMEM = {0, 15, 20};  // sky blue
const rgb_color color_15 PROGMEM = {0, 20, 15};  // turquoise
const rgb_color color_16 PROGMEM = {15, 0, 20};  // lavender
const rgb_color color_17 PROGMEM = {20, 0, 15};  // rose ros

const rgb_color color_18 PROGMEM = { 0,  0,  0}; // black
const rgb_color color_19 PROGMEM = { 5,  5,  5}; // dkgray
const rgb_color color_20 PROGMEM = {10, 10, 10}; // gray
const rgb_color color_21 PROGMEM = {20, 20, 20}; // white
const rgb_color color_22 PROGMEM = {20, 11, 2};  // tungsten
const rgb_color color_23 PROGMEM = {20, 5,  0};  // neon

const rgb_color* const color_array[] PROGMEM = {
  &color_0,  &color_1,  &color_2,  &color_3,  &color_4,  &color_5,  &color_6,  &color_7,
  &color_8,  &color_9,  &color_10, &color_11, &color_12, &color_13, &color_14, &color_15,
  &color_16, &color_17, &color_18, &color_19, &color_20, &color_21, &color_22, &color_23
};
#endif

class Colors
{
  public:
  enum color{
    red,         // pretty palette includes these
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

    black,       // full palette includes these too
    dkgray,
    gray,
    white,
    tungsten,
    neon
  };

  static const rgb_color * const get_color(byte c);

  static rgb_color random_palette_color();
  static void reset_palette();
  static void shuffle_palette();
  static rgb_color complimentary_color(rgb_color color);
  static void compliment_pairs();
  static void random_compliment_pairs();
  static void compliment_palette();
  static void rotate_palette(byte times, byte limit, bool down = true);
  static void reverse_palette();
  static rgb_color * get_palette();
  //static byte get_num_palette_colors();

  private:
  // this is pointed-to as the return value for get_color()
  static rgb_color return_color;
  static rgb_color palette[NUM_PALETTE_COLORS];
};

rgb_color Colors::return_color = {0,0,0};

rgb_color Colors::palette[NUM_PALETTE_COLORS];

#ifdef USE_BYTE_ARRAYS
const rgb_color * const Colors::get_color(byte c){
  void *p = (void*)pgm_read_word(&(bytes_array[c]));
  return_color.red =   pgm_read_byte(p + 0);
  return_color.green = pgm_read_byte(p + 1);
  return_color.blue =  pgm_read_byte(p + 2);
  return &return_color;
}

#else

const rgb_color * const Colors::get_color(byte c){
  void *p = (void*)pgm_read_word(&(color_array[c]));
  return_color.red =   pgm_read_byte(p + 0);
  return_color.green = pgm_read_byte(p + 1);
  return_color.blue =  pgm_read_byte(p + 2);
  return &return_color;
}
#endif

rgb_color Colors::random_palette_color(){
  return palette[random(NUM_PALETTE_COLORS)];
}

void Colors::reset_palette(){
  for(byte i = 0; i < NPRETTY_COLORS; i++){
    palette[i] = *Colors::get_color((Colors::color)i);
  }
}

// from https://forum.arduino.cc/index.php?topic=345964.0
void Colors::shuffle_palette()
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
rgb_color Colors::complimentary_color(rgb_color color){
  return (rgb_color){
    BRIGHTNESS_DIVISOR - color.red,
    BRIGHTNESS_DIVISOR - color.green,
    BRIGHTNESS_DIVISOR - color.blue
  };
}

// to do: compliment whole palette instead of in pairs

// create pairs of complimentary colors by making each odd color
// complimentary of the previous even color
void Colors::compliment_pairs()
{
  for(byte i = 0; i < NUM_PALETTE_COLORS; i += 2)
    palette[i+1] = complimentary_color(palette[i]);
}

// create a random palette of complimentary color pairs
void Colors::random_compliment_pairs()
{
  reset_palette();
  shuffle_palette();
  compliment_pairs();
}

void Colors::compliment_palette(){
  for(byte i = 0; i < NUM_PALETTE_COLORS; i ++)
    palette[i] = complimentary_color(palette[i]);
}

// times - number of rotations
// limit - how many color positions to rotate
// down - type = rotate higher number positions toward lower ones
void Colors::rotate_palette(byte times, byte limit, bool down){
  times = max(1, times);

  if(limit < 1){
    limit = NUM_PALETTE_COLORS ;
  }
  limit--;

  if(down == true){
    for(byte t = 0; t < times; t++){
      rgb_color carry_color = palette[0];
      for(byte i = 0; i < limit; i++){
        palette[i] = palette[i + 1];
      }
      palette[limit] = carry_color;
    }
  } else {
    for(byte t = 0; t < times; t++){
      rgb_color carry_color = palette[limit];
      for(byte i = limit; i > 0; i--){
        palette[i] = palette[i - 1];
      }
      palette[0] = carry_color;
    }
  }
}

void Colors::reverse_palette(){
  byte max = NUM_PALETTE_COLORS - 1;
  byte limit = NUM_PALETTE_COLORS / 2;
  for(byte i = 0; i < limit; i++){
    byte swap = max - i;
    rgb_color carry_color = palette[i];
    palette[i] = palette[swap];
    palette[swap] = carry_color;
  }
}

rgb_color * Colors::get_palette(){
  return palette;
}

//byte Colors::get_num_palette_colors(){
//}

#endif
