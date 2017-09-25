#ifndef COLORS_H
#define COLORS_H

#include <PololuLedStrip.h>

#define NUM_COLORS 25
#define NPRETTY_COLORS 18

#define NUM_PALETTE_COLORS NPRETTY_COLORS

// in pre-rendered colors, 20 = the max brightness, so at 100 brightness, it would multiply
// 20 * (255 / 20) to get 255
#define BRIGHTNESS_DIVISOR 20.0

// tungsten based on 20% brightness, 29,230,255:hsl became 133,70,12
// when unscaled, full strength is 255,135,23, downscaled to 0..20
// is 20,11,2

// neon looked right at 11,255,255:hsl, buffer 133,26,0, render 254,45,0
// scaled to 0..20 = 20,5,0

// sodium based on looked right at 136,68,24, scaled to 20,10,4

const byte color_00[] PROGMEM = {20,  0,  0};  // red
const byte color_01[] PROGMEM = {20, 10,  0};  // org
const byte color_02[] PROGMEM = {20, 20,  0};  // yel
const byte color_03[] PROGMEM = { 0, 20,  0};  // grn
const byte color_04[] PROGMEM = { 0,  0, 20};  // blu
const byte color_05[] PROGMEM = {10,  0, 20};  // pur
const byte color_06[] PROGMEM = { 0, 20, 20};  // cyn
const byte color_07[] PROGMEM = {20,  0, 20};  // mag
const byte color_08[] PROGMEM = { 0, 10, 20};  // lbl
const byte color_09[] PROGMEM = {10, 20,  0};  // lgr
const byte color_10[] PROGMEM = { 0, 20, 10};  // sea
const byte color_11[] PROGMEM = {20,  0, 10};  // pnk
const byte color_12[] PROGMEM = {20, 15,  0};  // amb
const byte color_13[] PROGMEM = {15, 20,  0};  // olv
const byte color_14[] PROGMEM = { 0, 15, 20};  // sky
const byte color_15[] PROGMEM = { 0, 20, 15};  // tur
const byte color_16[] PROGMEM = {15,  0, 20};  // lav
const byte color_17[] PROGMEM = {20,  0, 15};  // ros
const byte color_18[] PROGMEM = { 5,  5,  5};  // dgr
const byte color_19[] PROGMEM = {10, 10, 10};  // gry
const byte color_20[] PROGMEM = {20, 20, 20};  // wht
const byte color_21[] PROGMEM = {20, 11,  2};  // tun
const byte color_22[] PROGMEM = {20,  5,  0};  // neo
const byte color_23[] PROGMEM = {20, 10,  4};  // sod
const byte color_24[] PROGMEM = { 0,  0,  0};  // blk

const byte* const bytes_array[] PROGMEM = {
  color_00, color_01, color_02, color_03, color_04, color_05,
  color_06, color_07, color_08, color_09, color_10, color_11,
  color_12, color_13, color_14, color_15, color_16, color_17,
  color_18, color_19, color_20, color_21, color_22, color_23,
  color_24
};

class Colors
{
  public:
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

  private:
  // this is pointed-to as the return value for get_color()
  static rgb_color return_color;
  static rgb_color palette[NUM_PALETTE_COLORS];
};

rgb_color Colors::return_color = {0,0,0};
rgb_color Colors::palette[NUM_PALETTE_COLORS];

const rgb_color * const Colors::get_color(byte c)
{
  void *p = (void*)pgm_read_word(&(bytes_array[c]));
  return_color.red =   pgm_read_byte(p + 0);
  return_color.green = pgm_read_byte(p + 1);
  return_color.blue =  pgm_read_byte(p + 2);
  return &return_color;
}

rgb_color Colors::random_palette_color(){
  return palette[random(NUM_PALETTE_COLORS)];
}

void Colors::reset_palette()
{
  for(byte i = 0; i < NPRETTY_COLORS; i++)
    palette[i] = *Colors::get_color(i);
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
rgb_color Colors::complimentary_color(rgb_color color)
{
  return (rgb_color)
  {
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
  {
    rgb_color *pal = &palette[i];
    *(pal + 1) = complimentary_color(*pal);
  }
}

// create a random palette of complimentary color pairs
void Colors::random_compliment_pairs()
{
  reset_palette();
  shuffle_palette();
  compliment_pairs();
}

void Colors::compliment_palette()
{
  for(byte i = 0; i < NUM_PALETTE_COLORS; i++)
  {
    rgb_color *pal = &palette[i];
    *pal = complimentary_color(*pal);
  }
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

  if(down == true)
  {
    for(byte t = 0; t < times; t++)
    {
      rgb_color carry_color = palette[0];
      for(byte i = 0; i < limit; i++)
      {
        rgb_color *pal = &palette[i];
        *pal = *(pal + 1);
      }
      palette[limit] = carry_color;
    }
  }
  else
  {
    for(byte t = 0; t < times; t++)
    {
      rgb_color carry_color = palette[limit];
      for(byte i = limit; i > 0; i--)
      {
        rgb_color *pal = &palette[i];
        *pal = *(pal - 1);
      }
      palette[0] = carry_color;
    }
  }
}

// todo: dry
void Colors::reverse_palette()
{
  byte max = NUM_PALETTE_COLORS - 1;
  byte limit = NUM_PALETTE_COLORS / 2;
  for(byte i = 0; i < limit; i++)
  {
    rgb_color *pal = &palette[i];
    rgb_color *swap = &palette[max - i];
    rgb_color carry_color = *pal;
    *pal = *swap;
    *swap = carry_color;
  }
}

rgb_color * Colors::get_palette()
{
  return palette;
}

#endif
