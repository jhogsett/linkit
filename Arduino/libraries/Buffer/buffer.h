#ifndef BUFFER_H
#define BUFFER_H

#include <PololuLedStrip.h>
#include <render.h>

#define LEAVE_EFFECT -1
#define NO_EFFECT 0

#ifdef EXISTENCE_ENABLED
#define LEAVE_ID -1
#define NO_ID 0
#endif

// add current zone
// add point to zones array

class Buffer
{
  public:
#ifdef EXISTENCE_ENABLED
  void begin(PololuLedStripBase **ledStrips, byte default_brightness, float fade_rate, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color *buffer, rgb_color *render, byte *effects, byte *existence);
#else
  void begin(PololuLedStripBase **ledStrips, byte default_brightness, float fade_rate, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color *buffer, rgb_color *render, byte *effects);
#endif

  void display_buffer(rgb_color * pbuffer);
  void erase(bool display);
  void push_color(rgb_color color, bool display, byte effect, byte max);
  void push_rgb_color(byte red, byte green, byte blue);
  void push_hsl_color(int hue, int sat, int lit);
  void shift(byte count, byte maxx, bool fast_render);
  void finalize_shift(byte count, byte max);
  void set_color(byte pos, rgb_color color, bool display, byte effect);
  void set_window(byte width);
  void fade(float rate);
  void fade_fast();
  int get_window();
  void set_display(byte display);

  private:
  PololuLedStripBase **ledStrips;
  byte current_strip;
  rgb_color *buffer;
  static rgb_color *render;
  byte *effects;
  float default_brightness_scale;
  Render *renderer;
  byte safety_led_count;
  byte visible_led_count;
  float fade_rate;

#ifdef EXISTENCE_ENABLED
  byte *existence;
#endif

  byte window = 0;
  void shift_buffer(rgb_color * buffer, byte max);
};

rgb_color *Buffer::render;

#ifdef EXISTENCE_ENABLED
void Buffer::begin(PololuLedStripBase **ledStrips, byte default_brightness, float fade_rate, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color *buffer, rgb_color *render, byte *effects, byte *existence){
#else
void Buffer::begin(PololuLedStripBase **ledStrips, byte default_brightness, float fade_rate, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color *buffer, rgb_color *render, byte *effects){
#endif

  this->ledStrips = ledStrips;
  this->current_strip = 0;
  this->buffer = buffer;
  this->render = render;
  this->default_brightness_scale = default_brightness / 100.0;
  this->effects = effects;

#ifdef EXISTENCE_ENABLED
  this->existence = existence;
#endif

  this->renderer = renderer;
  this->safety_led_count = safety_led_count;
  this->visible_led_count = visible_led_count;
  this->fade_rate = fade_rate;

  this->window = this->visible_led_count;
}

void Buffer::display_buffer(rgb_color * pbuffer = render){
  ledStrips[current_strip]->write(pbuffer, visible_led_count);
}
  
void Buffer::erase(bool display = false){

  // to do: restrict to current zone
  for(byte i = 0; i < visible_led_count; i++){
    buffer[i] = black;
    effects[i] = NO_EFFECT;

#ifdef EXISTENCE_ENABLED
    existence[i] = NO_ID;
#endif
  }

  if(display){
    display_buffer(buffer);
  }
}

void Buffer::fade(float rate = 0.0){
  rate = (rate == 0.0) ? fade_rate : rate;
  byte *p;
  p = (byte *)buffer;
  // to do: restrict to current zone
  for(int i = 0; i < visible_led_count * 3; i++){
    *(p + i) *= rate;
  }
}

void Buffer::fade_fast(){
  byte *p;
  p = (byte *)buffer;
  // to do: restrict to current zone
  for(byte i = 0; i < visible_led_count * 3; i++){
    *(p + i) = *(p + i) >> 1;
  }
}

void Buffer::shift_buffer(rgb_color * buffer, byte max){

  // to do: set minimum position based on current zone
  for(byte i = max - 1; i >= 1; i--){
    buffer[i] = buffer[i-1];
    effects[i] = effects[i-1];

#ifdef EXISTENCE_ENABLED
    existence[i] = existence[i-1];
#endif

  }
}
  
#ifdef EXISTENCE_ENABLED
void Buffer::push_color(rgb_color color, bool display = false, byte effect = NO_EFFECT, byte max = 0, byte id = NO_ID){
#else
void Buffer::push_color(rgb_color color, bool display = false, byte effect = NO_EFFECT, byte max = 0){
#endif

  // to do: set default window by current zone
  max = (max == 0) ? window : max;

  shift_buffer(buffer, max);

  // to do: offset by zone
  buffer[0] = ColorMath::correct_color(color);
  effects[0] = effect;

#ifdef EXISTENCE_ENABLED
  existence[0] = id;
#endif

  if(display){
    display_buffer(buffer);
  }
}
  
void Buffer::push_rgb_color(byte red, byte green, byte blue){
  rgb_color color = (rgb_color){red, green, blue};
  color = ColorMath::unscale_color(color, default_brightness_scale);
  push_color(color);
}

void Buffer::push_hsl_color(int hue, int sat, int lit){
  rgb_color color = ColorMath::hsl_to_rgb(hue, sat, lit);
  push_rgb_color(color.red, color.green, color.blue);
}
  
#ifdef EXISTENCE_ENABLED
void Buffer::set_color(byte pos, rgb_color color, bool display = false, byte effect = NO_EFFECT, byte id = NO_ID){
#else
void Buffer::set_color(byte pos, rgb_color color, bool display = false, byte effect = NO_EFFECT){
#endif
  buffer[pos] = ColorMath::correct_color(color);
  
  if(effect != LEAVE_EFFECT){
    effects[pos] = effect;
  }

#ifdef EXISTENCE_ENABLED
  if(id != LEAVE_ID){
    existence[pos] = id;
  }
#endif

  if(display){
    display_buffer(buffer);
  }
}
  
void Buffer::set_window(byte window){
  this->window = (window < 1 || window > visible_led_count) ? visible_led_count : window;
}

int Buffer::get_window(){
  return window;
}

void Buffer::set_display(byte display){
  current_strip = display;
}

    // to do: restrict to current zone
// animate by shifting frame (future: shift in from back buffer)
void Buffer::shift(byte count, byte maxx, bool fast_render = true){
  maxx = min(maxx, safety_led_count);
  for(byte i = 0; i < count; i++){
    render[i] = black;
  }
  for(byte i = count; i < maxx; i++){
    if(fast_render)
      render[i] = renderer->fast_render(buffer[i - count], effects[i - count]);
    else
      render[i] = renderer->render(buffer[i - count], effects[i - count]);
  }

  display_buffer();
}

void Buffer::finalize_shift(byte count, byte max){

  // to do: restrict to current zone
  for(byte i = 0; i < count; i++){
    push_color(black, false, NO_EFFECT, max);
  }
}

#endif
