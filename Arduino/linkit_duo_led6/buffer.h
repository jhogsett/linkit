/*******************************************************************/

#include <PololuLedStrip.h>

PololuLedStrip<12> ledStrip;

#define DEFAULT_BRIGHTNESS_SCALE (DEFAULT_BRIGHTNESS_PERCENT / 100.0)

rgb_color colors[LED_COUNT];
rgb_color render[LED_COUNT];
//rgb_color backbuffer[LED_COUNT];

void display_buffer(rgb_color * pbuffer = render){
  ledStrip.write(pbuffer, ANIM_LED_COUNT);  
}

void erase(bool display = false){
  for(int i = 0; i < ANIM_LED_COUNT; i++){
    colors[i] = black;
    effects[i] = 0;
  }
  if(display){
    //ledStrip.write(colors, ANIM_LED_COUNT); 
    display_buffer(colors);
  }
}

void shift_buffer(rgb_color * buffer, bool include_effects = true, int max = ANIM_LED_COUNT){
  for(int i = max - 1; i >= 1; i--){
    buffer[i] = buffer[i-1];
    effects[i] = effects[i-1];
  }
}


void copy_buffer(rgb_color *source, rgb_color *destination, int count = ANIM_LED_COUNT){
  for(int i = 0; i < ANIM_LED_COUNT; i++){
    destination[i] = source[i];    
  }
}

// for limiting effects to preserve later pixels
int window = 0;

void push_color(rgb_color color, bool display = false, int effect = NO_EFFECT, rgb_color *buffer = colors, int max = window){
  if(max == 0){
    max = ANIM_LED_COUNT;
  }
  shift_buffer(buffer, true, max);
  
  buffer[0] = color;
  effects[0] = effect;
  
  if(display){
    display_buffer(buffer);
  }
}

void push_rgb_color(int red, int green, int blue){
  rgb_color color = (rgb_color){red, green, blue}; 
  color = unscale_color(color, DEFAULT_BRIGHTNESS_SCALE);
  push_color(color);
}

void push_hsl_color(int hue, int sat, int lit){
  rgb_color color = hsl_to_rgb(hue, sat, lit);
  push_rgb_color(color.red, color.green, color.blue);
}

void set_color(int pos, rgb_color color, bool display = false, int effect = NO_EFFECT, rgb_color *buffer = colors){
  buffer[pos] = color;

  if(effect != LEAVE_EFFECT){
    effects[pos] = effect;
  }
  
  if(display){
    display_buffer(buffer);
  }
}

void set_window(int width){
  if(width < 0){
    width = 0;
  }
  window = width;
}

// animate by shifting frame (future: shift in from back buffer)
void shift(int count, int maxx = ANIM_LED_COUNT){
  maxx = min(maxx, LED_COUNT);
  for(int i = 0; i < count; i++){
    render[i] = black;
  }
  for(int i = count; i < maxx; i++){
    render[i] = scale_color(colors[i - count], DEFAULT_BRIGHTNESS_SCALE);
  }

  display_buffer();
}

void finalize_shift(int count, int max){
  for(int i = 0; i < count; i++){
    push_color(black, false, NO_EFFECT, colors, max);
  }
}

