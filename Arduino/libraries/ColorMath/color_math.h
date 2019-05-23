#ifndef COLOR_MATH_H
#define COLOR_MATH_H

// pickup: make ColorMath the authority on brightness levels

#include <PololuLedStrip.h>
#include <colors.h>
#include <power_ease.h>

#define CROSSFADE_STEPS   20

#define SINE_INTS
//#define SINE_FLOATS

// #define SINE_24_STEPS

// #define SINE_36_STEPS
// #define SINE_48_STEPS
// #define SINE_60_STEPS

#define SINE_90_STEPS

#if defined(SINE_24_STEPS)
#define NUM_SINES    25
#define SINE_RANGE 24.0

#elif defined(SINE_36_STEPS)
#define NUM_SINES    37
#define SINE_RANGE 36.0

#elif defined(SINE_48_STEPS)
#define NUM_SINES    49
#define SINE_RANGE 48.0

#elif defined(SINE_60_STEPS)
#define NUM_SINES    61
#define SINE_RANGE 60.0

#elif defined(SINE_90_STEPS)
#define NUM_SINES    91
#define SINE_RANGE 90.0
#endif

#define COLOR_SCALE_FACTOR (255.0 / BRIGHTNESS_DIVISOR)

//#define USE_BYTE_CROSSFADE_STEPS

//smoother crossfade
#define USE_ALT_BYTE_CROSSFADE_STEPS

class ColorMath
{
  public:
  static void begin(bool swap_r_and_g);
  static rgb_color random_color();
  static rgb_color scale_color(rgb_color color, float scale);
  static rgb_color unscale_color(rgb_color color, float scale);
  static rgb_color hsl_to_rgb(int hue, int sat, int val);
  static rgb_color add_color(rgb_color color1, rgb_color color2);
  static rgb_color subtract_color(rgb_color color1, rgb_color color2);

#ifdef USE_BLEND
  static rgb_color blend_colors(rgb_color color1, rgb_color color2, float strength);
#endif

  static rgb_color crossfade_colors(byte step, rgb_color color1, rgb_color color2);
  static rgb_color correct_color(rgb_color color);
  static byte crossfade_steps(){ return CROSSFADE_STEPS; }
  // static rgb_color complimentary_color(rgb_color color);
  static bool equal(rgb_color color1, rgb_color color2);
  static float get_cosine(byte step);
  static float get_sine(byte step);
  static float crossfade_step_value(byte step);
  static byte crossfade_component_raw(byte step, byte component1, byte component2);

  private:
  static bool swap_r_and_g;

#if defined(USE_BYTE_CROSSFADE_STEPS) || defined(USE_ALT_BYTE_CROSSFADE_STEPS)
  static const byte PROGMEM crossfade[];
#else
  static const float PROGMEM crossfade[];
#endif

#if defined(SINE_FLOATS)
  static const float PROGMEM cosines[NUM_SINES];
  static const float PROGMEM sines[NUM_SINES];
#elif defined(SINE_INTS)
  static const byte PROGMEM cosines[NUM_SINES];
  static const byte PROGMEM sines[NUM_SINES];
#endif

#ifdef USE_BLEND
  static byte blend_component(byte component1, byte component2, float strength);
#endif

  static byte crossfade_component(byte step, byte component1, byte component2);

  static float read_cosine_array(byte step);
  static float read_sine_array(byte step);
};

bool ColorMath::swap_r_and_g;

#if defined(USE_ALT_BYTE_CROSSFADE_STEPS)
/*

max_step = 20
steps = (0..max_step)
steps.each do |step|
  position = step / (max_step * 1.0)
  print (((0.5 - 0.5 * Math.cos(Math::PI * position)) ** 0.5) * 255.0).floor
  puts
end
*/
const byte PROGMEM ColorMath::crossfade[] // CROSSFADE_STEPS + 1]
= {
    0,
    20,
    39,
    59,
    78,
    97,
    115,
    133,
    149,
    165,
    180,
    193,
    206,
    217,
    227,
    235,
    242,
    247,
    251,
    254,
    255
};
#elif defined(USE_BYTE_CROSSFADE_STEPS)
/* ruby:
max_step = 20
steps = (0..max_step)
steps.each do |step|
  position = step / (max_step * 1.0)
  puts (((0.5 - 0.5 * Math.cos(Math::PI * position)) ** 0.5) * 255.0).round(0)
end
*/
const byte PROGMEM ColorMath::crossfade[] // CROSSFADE_STEPS + 1]
= {
    0,
    20,
    40,
    60,
    79,
    98,
    116,
    133,
    150,
    166,
    180,
    194,
    206,
    217,
    227,
    236,
    243,
    248,
    252,
    254,
    255
};
#else
/* ruby:
  max_step = 20
  steps = (0..max_step)
  steps.each do |step|
    position = step / (max_step * 1.0)
    puts ((0.5 - 0.5 * Math.cos(Math::PI * position)) ** 0.5).round(15)
  end
*/
const float PROGMEM ColorMath::crossfade[] // CROSSFADE_STEPS + 1]
= {
    0.0,
    0.078459095727845,
    0.156434465040231,
    0.233445363855905,
    0.309016994374947,
    0.38268343236509,
    0.453990499739547,
    0.522498564715949,
    0.587785252292473,
    0.649448048330184,
    0.707106781186548,
    0.760405965600031,
    0.809016994374948,
    0.852640164354092,
    0.891006524188368,
    0.923879532511287,
    0.951056516295154,
    0.972369920397677,
    0.987688340595138,
    0.996917333733128,
    1.0
};
#endif

#if defined(SINE_FLOATS)

#ifdef SINE_24_STEPS
/* ruby code
  steps = 24
  cosines = (0..steps).map{|n| ((1.0 - Math.cos((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5).round(15)}
  sines   = (0..steps).map{|n| ((1.0 - Math.sin((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5).round(15)}
*/
const float PROGMEM ColorMath::cosines[NUM_SINES]
= {
    0.0,
    0.017037086855466,
    0.066987298107781,
    0.146446609406726,
    0.25,
    0.37059047744874,
    0.5,
    0.62940952255126,
    0.75,
    0.853553390593274,
    0.933012701892219,
    0.982962913144534,
    1.0,
    0.982962913144534,
    0.933012701892219,
    0.853553390593274,
    0.75,
    0.62940952255126,
    0.5,
    0.37059047744874,
    0.25,
    0.146446609406726,
    0.066987298107781,
    0.017037086855466,
    0.0
};

const float PROGMEM ColorMath::sines[NUM_SINES]
= {
    0.5,
    0.37059047744874,
    0.25,
    0.146446609406726,
    0.066987298107781,
    0.017037086855466,
    0.0,
    0.017037086855466,
    0.066987298107781,
    0.146446609406726,
    0.25,
    0.37059047744874,
    0.5,
    0.62940952255126,
    0.75,
    0.853553390593274,
    0.933012701892219,
    0.982962913144534,
    1.0,
    0.982962913144534,
    0.933012701892219,
    0.853553390593274,
    0.75,
    0.62940952255126,
    0.5
};

#elif defined(SINE_36_STEPS)
/* ruby code
  steps = 24
  cosines = (0..steps).map{|n| ((1.0 - Math.cos((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5).round(15)}
  sines = (0..steps).map{|n| ((1.0 - Math.sin((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5).round(15)}
only elements 0-12 are needed of cosines array to compute both sine and cosine values
*/

const float PROGMEM ColorMath::cosines[NUM_SINES]
= {
    0.0,
    0.007596123493896,
    0.030153689607046,
    0.066987298107781,
    0.116977778440511,
    0.17860619515673,
    0.25,
    0.328989928337166,
    0.413175911166535,
    0.5,
    0.586824088833465,
    0.671010071662834,
    0.75,
    0.82139380484327,
    0.883022221559489,
    0.933012701892219,
    0.969846310392954,
    0.992403876506104,
    1.0,
    0.992403876506104,
    0.969846310392954,
    0.933012701892219,
    0.883022221559489,
    0.82139380484327,
    0.75,
    0.671010071662835,
    0.586824088833465,
    0.5,
    0.413175911166535,
    0.328989928337165,
    0.25,
    0.17860619515673,
    0.116977778440511,
    0.066987298107781,
    0.030153689607046,
    0.007596123493896,
    0.0
};

const float PROGMEM ColorMath::sines[NUM_SINES]
= {
    0.5,
    0.413175911166535,
    0.328989928337166,
    0.25,
    0.17860619515673,
    0.116977778440511,
    0.066987298107781,
    0.030153689607046,
    0.007596123493896,
    0.0,
    0.007596123493896,
    0.030153689607046,
    0.066987298107781,
    0.116977778440511,
    0.17860619515673,
    0.25,
    0.328989928337166,
    0.413175911166535,
    0.5,
    0.586824088833465,
    0.671010071662834,
    0.75,
    0.82139380484327,
    0.883022221559489,
    0.933012701892219,
    0.969846310392954,
    0.992403876506104,
    1.0,
    0.992403876506104,
    0.969846310392954,
    0.933012701892219,
    0.883022221559489,
    0.82139380484327,
    0.75,
    0.671010071662835,
    0.586824088833465,
    0.5
};

#elif defined(SINE_48_STEPS)
/* ruby code
  steps = 48
  cosines = (0..steps).map{|n| ((1.0 - Math.cos((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5).round(15)}
  sines = (0..steps).map{|n| ((1.0 - Math.sin((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5).round(15)}
only elements 0-12 are needed of cosines array to compute both sine and cosine values
*/

const float PROGMEM ColorMath::cosines[NUM_SINES]
= {
    0.0,
    0.004277569313095,
    0.017037086855466,
    0.038060233744357,
    0.066987298107781,
    0.103323329854382,
    0.146446609406726,
    0.19561928549564,
    0.25,
    0.308658283817455,
    0.37059047744874,
    0.434736903889974,
    0.5,
    0.565263096110026,
    0.62940952255126,
    0.691341716182545,
    0.75,
    0.80438071450436,
    0.853553390593274,
    0.896676670145618,
    0.933012701892219,
    0.961939766255643,
    0.982962913144534,
    0.995722430686905,
    1.0,
    0.995722430686905,
    0.982962913144534,
    0.961939766255643,
    0.933012701892219,
    0.896676670145618,
    0.853553390593274,
    0.804380714504361,
    0.75,
    0.691341716182545,
    0.62940952255126,
    0.565263096110026,
    0.5,
    0.434736903889974,
    0.37059047744874,
    0.308658283817455,
    0.25,
    0.19561928549564,
    0.146446609406726,
    0.103323329854383,
    0.066987298107781,
    0.038060233744357,
    0.017037086855466,
    0.004277569313095,
    0.0
};

const float PROGMEM ColorMath::sines[NUM_SINES]
= {
    0.5,
    0.434736903889974,
    0.37059047744874,
    0.308658283817455,
    0.25,
    0.19561928549564,
    0.146446609406726,
    0.103323329854382,
    0.066987298107781,
    0.038060233744357,
    0.017037086855466,
    0.004277569313095,
    0.0,
    0.004277569313095,
    0.017037086855466,
    0.038060233744357,
    0.066987298107781,
    0.103323329854382,
    0.146446609406726,
    0.19561928549564,
    0.25,
    0.308658283817455,
    0.37059047744874,
    0.434736903889974,
    0.5,
    0.565263096110026,
    0.62940952255126,
    0.691341716182545,
    0.75,
    0.80438071450436,
    0.853553390593274,
    0.896676670145618,
    0.933012701892219,
    0.961939766255643,
    0.982962913144534,
    0.995722430686905,
    1.0,
    0.995722430686905,
    0.982962913144534,
    0.961939766255643,
    0.933012701892219,
    0.896676670145618,
    0.853553390593274,
    0.804380714504361,
    0.75,
    0.691341716182545,
    0.62940952255126,
    0.565263096110026,
    0.5
};
#endif

#elif defined(SINE_INTS)

#ifdef SINE_24_STEPS
/* ruby code
  steps = 24
  cosines = (0..steps).map{|n| (((1.0 - Math.cos((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5) * 255).floor}
  sines = (0..steps).map{|n| (((1.0 - Math.sin((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5)* 255).floor}
*/
const byte PROGMEM ColorMath::cosines[NUM_SINES]
= {
    0, 
    4, 
    17, 
    37, 
    63, 
    94, 
    127, 
    160, 
    191, 
    217, 
    237, 
    250, 
    255, 
    250, 
    237, 
    217, 
    191, 
    160, 
    127, 
    94, 
    63, 
    37, 
    17, 
    4, 
    0
};

const byte PROGMEM ColorMath::sines[NUM_SINES]
= {
    127, 
    94, 
    63, 
    37, 
    17, 
    4, 
    0, 
    4, 
    17, 
    37, 
    63, 
    94, 
    127, 
    160, 
    191, 
    217, 
    237, 
    250, 
    255, 
    250, 
    237, 
    217, 
    191, 
    160, 
    127
};

#elif defined(SINE_36_STEPS)
/* ruby code
  steps = 36
  cosines = (0..steps).map{|n| (((1.0 - Math.cos((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5) * 255).floor}
  sines = (0..steps).map{|n| (((1.0 - Math.sin((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5)* 255).floor}
*/

const byte PROGMEM ColorMath::cosines[NUM_SINES]
= {
    0,
    1,
    7,
    17,
    29,
    45,
    63,
    83,
    105,
    127,
    149,
    171,
    191,
    209,
    225,
    237,
    247,
    253,
    255,
    253,
    247,
    237,
    225,
    209,
    191,
    171,
    149,
    127,
    105,
    83,
    63,
    45,
    29,
    17,
    7,
    1,
    0
};

const byte PROGMEM ColorMath::sines[NUM_SINES]
= {
    127,
    105,
    83,
    63,
    45,
    29,
    17,
    7,
    1,
    0,
    1,
    7,
    17,
    29,
    45,
    63,
    83,
    105,
    127,
    149,
    171,
    191,
    209,
    225,
    237,
    247,
    253,
    255,
    253,
    247,
    237,
    225,
    209,
    191,
    171,
    149,
    127
};

#elif defined(SINE_48_STEPS)
/* ruby code
  steps = 48
  cosines = (0..steps).map{|n| (((1.0 - Math.cos((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5) * 255).floor}
  sines = (0..steps).map{|n| (((1.0 - Math.sin((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5)* 255).floor}
*/

const byte PROGMEM ColorMath::cosines[NUM_SINES]
= {
    0,
    1,
    4,
    9,
    17,
    26,
    37,
    49,
    63,
    78,
    94,
    110,
    127,
    144,
    160,
    176,
    191,
    205,
    217,
    228,
    237,
    245,
    250,
    253,
    255,
    253,
    250,
    245,
    237,
    228,
    217,
    205,
    191,
    176,
    160,
    144,
    127,
    110,
    94,
    78,
    63,
    49,
    37,
    26,
    17,
    9,
    4,
    1,
    0
};

const byte PROGMEM ColorMath::sines[NUM_SINES]
= {
    127,
    110,
    94,
    78,
    63,
    49,
    37,
    26,
    17,
    9,
    4,
    1,
    0,
    1,
    4,
    9,
    17,
    26,
    37,
    49,
    63,
    78,
    94,
    110,
    127,
    144,
    160,
    176,
    191,
    205,
    217,
    228,
    237,
    245,
    250,
    253,
    255,
    253,
    250,
    245,
    237,
    228,
    217,
    205,
    191,
    176,
    160,
    144,
    127
};

#elif defined(SINE_60_STEPS)
/* ruby code
  steps = 60
  cosines = (0..steps).map{|n| (((1.0 - Math.cos((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5) * 255).floor}
  sines = (0..steps).map{|n| (((1.0 - Math.sin((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5)* 255).floor}
*/

const byte PROGMEM ColorMath::cosines[NUM_SINES]
= {
0, 0, 2, 6, 11, 17, 24, 32, 42, 52, 63, 75, 88, 100, 114, 127, 140, 154, 166, 179, 191, 202, 212, 222, 230, 237, 243, 248, 252, 254, 255, 254, 252, 248, 243, 237, 230, 222, 212, 202, 191, 179, 166, 154, 140, 127, 114, 100, 88, 75, 63, 52, 42, 32, 24, 17, 11, 6, 2, 0, 0
};

const byte PROGMEM ColorMath::sines[NUM_SINES]
= {
127, 114, 100, 88, 75, 63, 52, 42, 32, 24, 17, 11, 6, 2, 0, 0, 0, 2, 6, 11, 17, 24, 32, 42, 52, 63, 75, 88, 100, 114, 127, 140, 154, 166, 179, 191, 202, 212, 222, 230, 237, 243, 248, 252, 254, 255, 254, 252, 248, 243, 237, 230, 222, 212, 202, 191, 179, 166, 154, 140, 127
};

#elif defined(SINE_90_STEPS)
/* ruby code
  steps = 90
  cosines = (0..steps).map{|n| (((1.0 - Math.cos((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5) * 255).floor}
  sines = (0..steps).map{|n| (((1.0 - Math.sin((Math::PI * 2) * (n / (steps * 1.0)))) * 0.5)* 255).floor}
*/

const byte PROGMEM ColorMath::cosines[NUM_SINES]
= {
0, 0, 1, 2, 4, 7, 11, 14, 19, 24, 29, 35, 42, 49, 56, 63, 71, 79, 88, 96, 105, 114, 123, 131, 140, 149, 158, 166, 175, 183, 191, 198, 205, 212, 219, 225, 230, 235, 240, 243, 247, 250, 252, 253, 254, 255, 254, 253, 252, 250, 247, 243, 240, 235, 230, 225, 219, 212, 205, 198, 191, 183, 175, 166, 158, 149, 140, 131, 123, 114, 105, 96, 88, 79, 71, 63, 56, 49, 42, 35, 29, 24, 19, 14, 11, 7, 4, 2, 1, 0, 0
};

const byte PROGMEM ColorMath::sines[NUM_SINES]
= {
127, 118, 109, 100, 92, 83, 75, 67, 59, 52, 45, 38, 32, 27, 21, 17, 12, 9, 6, 3, 1, 0, 0, 0, 0, 1, 3, 6, 9, 12, 17, 21, 27, 32, 38, 45, 52, 59, 67, 75, 83, 92, 100, 109, 118, 127, 136, 145, 154, 162, 171, 179, 187, 195, 202, 209, 216, 222, 227, 233, 237, 242, 245, 248, 251, 253, 254, 254, 254, 254, 253, 251, 248, 245, 242, 237, 233, 227, 222, 216, 209, 202, 195, 187, 179, 171, 162, 154, 145, 136, 127

// 180 degree phase shift
//127, 136, 145, 154, 162, 171, 179, 187, 195, 202, 209, 216, 222, 227, 233, 237, 242, 245, 248, 251, 253, 254, 254, 254, 254, 253, 251, 248, 245, 242, 237, 233, 227, 222, 216, 209, 202, 195, 187, 179, 171, 162, 154, 145, 136, 127, 118, 109, 100, 92, 83, 75, 67, 59, 52, 45, 38, 32, 27, 21, 17, 12, 9, 6, 3, 1, 0, 0, 0, 0, 1, 3, 6, 9, 12, 17, 21, 27, 32, 38, 45, 52, 59, 67, 75, 83, 92, 100, 109, 118, 127
};
#endif

#endif

rgb_color ColorMath::scale_color(rgb_color color, float scale){
  float factor = scale * COLOR_SCALE_FACTOR;
  return (rgb_color){
    color.red * factor,
    color.green * factor,
    color.blue * factor
  };
}

rgb_color ColorMath::unscale_color(rgb_color color, float scale){
  float factor = scale * COLOR_SCALE_FACTOR;
  return (rgb_color){
    color.red / factor,
    color.green / factor,
    color.blue / factor
  };
}

// hue: 0-359, sat: 0-255, val (lightness): 0-255
rgb_color ColorMath::hsl_to_rgb(int hue, int sat, int val) {
  int r, g, b, base;

  if (sat == 0) { // Achromatic color (gray).
    r = val;
    g = val;
    b = val;
  } else  {
    base = ((255 - sat) * val)>>8;
    switch(hue/60) {
      case 0:
        r = val;
        g = (((val-base)*hue)/60)+base;
        b = base;
        break;
      case 1:
        r = (((val-base)*(60-(hue%60)))/60)+base;
        g = val;
        b = base;
        break;
      case 2:
        r = base;
        g = val;
        b = (((val-base)*(hue%60))/60)+base;
        break;
      case 3:
        r = base;
        g = (((val-base)*(60-(hue%60)))/60)+base;
        b = val;
        break;
      case 4:
        r = (((val-base)*(hue%60))/60)+base;
        g = base;
        b = val;
        break;
      case 5:
        r = val;
        g = base;
        b = (((val-base)*(60-(hue%60)))/60)+base;
        break;
    }
  }
  return (rgb_color){r, g, b};
}

//rgb_color ColorMath::add_color(rgb_color color1, rgb_color color2){
//  rgb_color new_color;
//  new_color.red = min(255, color1.red + color2.red);
//  new_color.green = min(255, color1.green + color2.green);
//  new_color.blue = min(255, color1.blue + color2.blue);
//  return new_color;
//}
//
//rgb_color ColorMath::subtract_color(rgb_color color1, rgb_color color2){
//  rgb_color new_color;
//  new_color.red = max(0, color1.red - color2.red);
//  new_color.green = max(0, color1.green - color2.green);
//  new_color.blue = max(0, color1.blue - color2.blue);
//  return new_color;
//}

// some sets have red and green swapped, usually false for led strips
void ColorMath::begin(bool swap_r_and_g = true)
{
  ColorMath::swap_r_and_g = swap_r_and_g;
}

float ColorMath::crossfade_step_value(byte step)
{
#if defined(USE_BYTE_CROSSFADE_STEPS) || defined(USE_ALT_BYTE_CROSSFADE_STEPS)
  return pgm_read_byte(&crossfade[step]) / 255.0;
#else
  return pgm_read_float(&crossfade[step]);
#endif
}

byte ColorMath::crossfade_component_raw(byte step, byte component1, byte component2)
{
  int newc1 = component1 * crossfade_step_value(CROSSFADE_STEPS - step);
  int newc2 = component2 * crossfade_step_value(step);
  return newc1 + newc2;
}

// on subsequent steps, component1 must be set to the return value of the previous step
// the steps must go from zero, to and including CROSSFADE_STEPS
byte ColorMath::crossfade_component(byte step, byte component1, byte component2)
{
  if(step > 1)
  {
    byte prev_step = step - 1;
    int prevc2 = component2 * crossfade_step_value(prev_step);
    int restored_c1 = (component1 - prevc2) / crossfade_step_value(CROSSFADE_STEPS - prev_step);
    component1 = restored_c1;
  }

  return crossfade_component_raw(step, component1, component2);
}

//  color1 is the dominant color for strength (0.0 = all color1)
rgb_color ColorMath::crossfade_colors(byte step, rgb_color color1, rgb_color color2)
{
  rgb_color result;
  result.red = crossfade_component(step, color1.red, color2.red);
  result.green = crossfade_component(step, color1.green, color2.green);
  result.blue = crossfade_component(step, color1.blue, color2.blue);
  return result;
}

#ifdef USE_BLEND
byte ColorMath::blend_component(byte component1, byte component2, float strength)
{
  return (byte)((component1 * strength) + (component2 * (1.0 - strength)));
}

//  color1 is the dominant color for strength (1.0 = all color1)
rgb_color ColorMath::blend_colors(rgb_color color1, rgb_color color2, float strength = 0.5)
{
  rgb_color result;
  result.red = blend_component(color1.red, color2.red, strength);
  result.green = blend_component(color1.green, color2.green, strength);
  result.blue = blend_component(color1.blue, color2.blue, strength);
  return result;
}
#endif

rgb_color ColorMath::correct_color(rgb_color color)
{
  if(swap_r_and_g)
  {
    byte swap = color.red;
    color.red = color.green;
    color.green = swap;
  }
  return color;
}

// needed in renderer
rgb_color ColorMath::random_color()
{
  return *Colors::get_color(random(NPRETTY_COLORS));
}

bool ColorMath::equal(rgb_color color1, rgb_color color2)
{
  return color1.red == color2.red && color1.green == color2.green && color1.blue == color2.blue;
}

#if defined(SINE_FLOATS)
float ColorMath::read_cosine_array(byte step)
{
  return pgm_read_float(&cosines[step]);
}

float ColorMath::read_sine_array(byte step)
{
  return pgm_read_float(&sines[step]);
}
#elif defined(SINE_INTS)
float ColorMath::read_cosine_array(byte step)
{
  return pgm_read_byte(&cosines[step]) / 255.0;
}

float ColorMath::read_sine_array(byte step)
{
  return pgm_read_byte(&sines[step]) / 255.0;
}
#endif

float ColorMath::get_cosine(byte step)
{
  return read_cosine_array(step);
}

// steps 0-24
float ColorMath::get_sine(byte step)
{
  return read_sine_array(step);
}

#endif
