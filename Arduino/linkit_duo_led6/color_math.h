class ColorMath
{
  public:
  void setup_colors(bool swap);
  void set_brightness(int brightness_percent);
  rgb_color random_color();
  rgb_color scale_color(rgb_color color, float scale);
  rgb_color unscale_color(rgb_color color, float scale);
  rgb_color hsl_to_rgb(int hue, int sat, int val);
  rgb_color add_color(rgb_color color1, rgb_color color2);
  rgb_color subtract_color(rgb_color color1, rgb_color color2);
  rgb_color blend_colors(rgb_color color1, rgb_color color2);
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

void ColorMath::set_brightness(int brightness_percent = DEFAULT_BRIGHTNESS_PERCENT){
  float percent = brightness_percent / 100.0;
  for(int i = 0; i < NPALETTE; i++){
    adjusted_palette[i] = scale_color(palette[i], percent);
  }
}

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

void ColorMath::setup_colors(bool swap = true){
  if(swap == true){
    for(int i = 0; i < NPALETTE; i++){
      unsigned char value = palette[i].red;
      palette[i].red = palette[i].green;  
      palette[i].green = value;
    }
  }
  memcpy(adjusted_palette, palette, sizeof(palette));
}

rgb_color ColorMath::blend_colors(rgb_color color1, rgb_color color2){
  rgb_color result;
  result.red = (color1.red + color2.red) / 2;
  result.green = (color1.green + color2.green) / 2;
  result.blue = (color1.blue + color2.blue) / 2;
  return result;
}

rgb_color ColorMath::random_color(){
  return palette[random(NPRETTY_COLORS)];
}

