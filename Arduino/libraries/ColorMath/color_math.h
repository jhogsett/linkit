#ifndef COLOR_MATH_H
#define COLOR_MATH_H

// eanble to support palettes
// to do: disable palettes
#define USE_PALETTES

#include <PololuLedStrip.h>
#include <colors.h>

class ColorMath
{
  public:
  static void begin(bool swap_r_and_g);
  static void set_brightness(byte brightness_percent);
  static rgb_color random_color();
  static rgb_color scale_color(rgb_color color, float scale);
  static rgb_color unscale_color(rgb_color color, float scale);
  static rgb_color hsl_to_rgb(int hue, int sat, int val);
  static rgb_color add_color(rgb_color color1, rgb_color color2);
  static rgb_color subtract_color(rgb_color color1, rgb_color color2);
  static rgb_color blend_colors(rgb_color color1, rgb_color color2, float strength);
  static rgb_color crossfade_colors(int step, int times, rgb_color color1, rgb_color color2);
  static rgb_color correct_color(rgb_color color);

  private:
  static bool swap_r_and_g;
  static byte blend_component(byte component1, byte component2, float strength);
  static byte crossfade_component(int step, int times, byte component1, byte component2);
};

bool ColorMath::swap_r_and_g;

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

#ifdef USE_PALETTES
void ColorMath::set_brightness(byte brightness_percent){
  float percent = brightness_percent / 100.0;
  for(byte i = 0; i < NPALETTE; i++){
    adjusted_palette[i] = scale_color(palette[i], percent);
  }
}
#endif

rgb_color ColorMath::add_color(rgb_color color1, rgb_color color2){
  rgb_color new_color;
  new_color.red = min(255, color1.red + color2.red);
  new_color.green = min(255, color1.green + color2.green);
  new_color.blue = min(255, color1.blue + color2.blue);
  return new_color;
}

rgb_color ColorMath::subtract_color(rgb_color color1, rgb_color color2){
  rgb_color new_color;
  new_color.red = max(0, color1.red - color2.red);
  new_color.green = max(0, color1.green - color2.green);
  new_color.blue = max(0, color1.blue - color2.blue);
  return new_color;
}

// some sets have red and green swapped, usually false for led strips
void ColorMath::begin(bool swap_r_and_g = true){
  ColorMath::swap_r_and_g = swap_r_and_g;

#ifdef USE_PALETTES
//  if(swap_r_and_g == true){
//    for(byte i = 0; i < NPALETTE; i++){
//      unsigned char value = palette[i].red;
//      palette[i].red = palette[i].green;
//      palette[i].green = value;
//    }
//  }
//  memcpy(adjusted_palette, palette, sizeof(palette));
#endif
}

byte ColorMath::crossfade_component(int step, int times, byte component1, byte component2){
  float strength = 1.0 / times;
  if(step == 0)
    return (byte)((component2 * (1.0 - (strength * (times - step)))) + (component1 * (strength * (times - step))));
  else
    return (byte)((component2 * (1.0 - (strength * (times - step)))) + ((component1 - (component2 * (1.0 - (strength * ((times - step) + 1))))) * (strength * (times - step))));
}

//  color1 is the dominant color for strength (1.0 = all color1)
rgb_color ColorMath::crossfade_colors(int step, int times, rgb_color color1, rgb_color color2){
  rgb_color result;
  result.red = crossfade_component(step, times, color1.red, color2.red);
  result.green = crossfade_component(step, times, color1.green, color2.green);
  result.blue = crossfade_component(step, times, color1.blue, color2.blue);
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

#ifdef USE_PALETTES
// needed in renderer
rgb_color ColorMath::random_color(){
  return palette[random(NPRETTY_COLORS)];
}

#endif

#endif
