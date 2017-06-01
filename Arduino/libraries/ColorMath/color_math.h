#ifndef COLOR_MATH_H
#define COLOR_MATH_H

// pickup: make ColorMath the authority on brightness levels

#include <PololuLedStrip.h>
#include <colors.h>
#include <power_ease.h>

#define CROSSFADE_STEPS 20

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
  static rgb_color blend_colors(rgb_color color1, rgb_color color2, float strength);
  static rgb_color crossfade_colors(byte step, rgb_color color1, rgb_color color2);
  static rgb_color correct_color(rgb_color color);
  static byte crossfade_steps();
  static rgb_color simple_scale_color(rgb_color color, float scale);
  static rgb_color simple_unscale_color(rgb_color color, float scale);
  static rgb_color complimentary_color(rgb_color color);
  static bool equal(rgb_color color1, rgb_color color2);

  private:
  static bool swap_r_and_g;
  static const float PROGMEM crossfade[];

  static byte blend_component(byte component1, byte component2, float strength);
  static byte crossfade_component(byte step, byte component1, byte component2);
};

bool ColorMath::swap_r_and_g;

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

rgb_color ColorMath::scale_color(rgb_color color, float scale){
  return (rgb_color){
    ((color.red / BRIGHTNESS_DIVISOR) * 255) * scale,
    ((color.green / BRIGHTNESS_DIVISOR) * 255) * scale,
    ((color.blue / BRIGHTNESS_DIVISOR) *255) * scale
  };
}

rgb_color ColorMath::unscale_color(rgb_color color, float scale){
  return (rgb_color){
    ((color.red / scale) / 255) * BRIGHTNESS_DIVISOR,
    ((color.green / scale) / 255) * BRIGHTNESS_DIVISOR,
    ((color.blue / scale) / 255) * BRIGHTNESS_DIVISOR,
  };
}

rgb_color ColorMath::simple_scale_color(rgb_color color, float scale){
  return (rgb_color){
    color.red * scale,
    color.green * scale,
    color.blue * scale
  };
}

rgb_color ColorMath::simple_unscale_color(rgb_color color, float scale){
  return (rgb_color){
    color.red / scale,
    color.green / scale,
    color.blue / scale
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
void ColorMath::begin(bool swap_r_and_g = true){
  ColorMath::swap_r_and_g = swap_r_and_g;
}

byte ColorMath::crossfade_steps(){
  return CROSSFADE_STEPS;
}

// on subsequent steps, component1 must be set to the return value of the previous step
// the steps must go from zero, to and including CROSSFADE_STEPS
byte ColorMath::crossfade_component(byte step, byte component1, byte component2){
  if(step > 1){
    byte prev_step = step - 1;
    int prevc2 = component2 * pgm_read_float(&crossfade[prev_step]);
    int restored_c1 = (component1 - prevc2) / pgm_read_float(&crossfade[CROSSFADE_STEPS - prev_step]);
    component1 = restored_c1;
  }

  int newc1 = component1 * pgm_read_float(&crossfade[CROSSFADE_STEPS - step]);
  int newc2 = component2 * pgm_read_float(&crossfade[step]);
  return newc1 + newc2;
}

//  color1 is the dominant color for strength (0.0 = all color1)
rgb_color ColorMath::crossfade_colors(byte step, rgb_color color1, rgb_color color2){
  rgb_color result;
  result.red = crossfade_component(step, color1.red, color2.red);
  result.green = crossfade_component(step, color1.green, color2.green);
  result.blue = crossfade_component(step, color1.blue, color2.blue);
  return result;
}

byte ColorMath::blend_component(byte component1, byte component2, float strength){
  return (byte)((component1 * strength) + (component2 * (1.0 - strength)));
}

//  color1 is the dominant color for strength (1.0 = all color1)
rgb_color ColorMath::blend_colors(rgb_color color1, rgb_color color2, float strength = 0.5){
  rgb_color result;
  result.red = blend_component(color1.red, color2.red, strength);
  result.green = blend_component(color1.green, color2.green, strength);
  result.blue = blend_component(color1.blue, color2.blue, strength);
  return result;
}

rgb_color ColorMath::correct_color(rgb_color color){
  if(swap_r_and_g){
    byte swap = color.red;
    color.red = color.green;
    color.green = swap;
  }
    return color;
}

// needed in renderer
rgb_color ColorMath::random_color(){
  return *Colors::get_color((Colors::color)random(NPRETTY_COLORS));
}

bool ColorMath::equal(rgb_color color1, rgb_color color2){
  if(color1.red == color2.red && color1.green == color2.green && color1.blue == color2.blue){
    return true;
  }
  return false;
}

#endif
