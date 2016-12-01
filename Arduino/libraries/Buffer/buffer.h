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

class Buffer
{
  public:
#ifdef EXISTENCE_ENABLED
  void begin(PololuLedStripBase **ledStrips, int default_brightness, float fade_rate, int safety_led_count, int visible_led_count, Render *renderer, rgb_color *buffer, rgb_color *render, char *effects, int *existence);
#else
  void begin(PololuLedStripBase **ledStrips, int default_brightness, float fade_rate, int safety_led_count, int visible_led_count, Render *renderer, rgb_color *buffer, rgb_color *render, char *effects);
#endif

  void display_buffer(rgb_color * pbuffer);
  void erase(bool display);
  void push_color(rgb_color color, bool display, int effect, int max);
  void push_rgb_color(int red, int green, int blue);
  void push_hsl_color(int hue, int sat, int lit);
  void shift(int count, int maxx, bool fast_render);
  void finalize_shift(int count, int max);
  void set_color(int pos, rgb_color color, bool display, int effect);
  void set_window(int width);
  void fade(float rate);
  void fade_fast();
  int get_window();
  int set_display(int display);

  private:
  PololuLedStripBase **ledStrips;
  int current_strip;
  rgb_color *buffer;
  static rgb_color *render;
  char *effects;
  float default_brightness_scale;
  Render *renderer;
  int safety_led_count;
  int visible_led_count;
  float fade_rate;

#ifdef EXISTENCE_ENABLED
  int *existence;
#endif

  int window = 0;
  void shift_buffer(rgb_color * buffer, int max);
};

rgb_color *Buffer::render;

#ifdef EXISTENCE_ENABLED
void Buffer::begin(PololuLedStripBase **ledStrips, int default_brightness, float fade_rate, int safety_led_count, int visible_led_count, Render *renderer, rgb_color *buffer, rgb_color *render, char *effects, int *existence){
#else
void Buffer::begin(PololuLedStripBase **ledStrips, int default_brightness, float fade_rate, int safety_led_count, int visible_led_count, Render *renderer, rgb_color *buffer, rgb_color *render, char *effects){
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
  for(int i = 0; i < visible_led_count; i++){
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
  unsigned char *p;
  p = (unsigned char *)buffer;
  for(int i = 0; i < visible_led_count * 3; i++){
    *(p + i) *= rate;
  }
}

void Buffer::fade_fast(){
  unsigned char *p;
  p = (unsigned char *)buffer;
  for(int i = 0; i < visible_led_count * 3; i++){
    *(p + i) = *(p + i) >> 1;
  }
}

void Buffer::shift_buffer(rgb_color * buffer, int max){
  for(int i = max - 1; i >= 1; i--){
    buffer[i] = buffer[i-1];
    effects[i] = effects[i-1];

#ifdef EXISTENCE_ENABLED
    existence[i] = existence[i-1];
#endif

  }
}
  
#ifdef EXISTENCE_ENABLED
void Buffer::push_color(rgb_color color, bool display = false, int effect = NO_EFFECT, int max = 0, int id = NO_ID){
#else
void Buffer::push_color(rgb_color color, bool display = false, int effect = NO_EFFECT, int max = 0){
#endif
  max = (max == 0) ? window : max;

  shift_buffer(buffer, max);

  buffer[0] = color;
  effects[0] = effect;

#ifdef EXISTENCE_ENABLED
  existence[0] = id;
#endif

  if(display){
    display_buffer(buffer);
  }
}
  
void Buffer::push_rgb_color(int red, int green, int blue){
  rgb_color color = (rgb_color){red, green, blue};
  color = ColorMath::unscale_color(color, default_brightness_scale);
  push_color(color);
}

void Buffer::push_hsl_color(int hue, int sat, int lit){
  rgb_color color = ColorMath::hsl_to_rgb(hue, sat, lit);
  push_rgb_color(color.red, color.green, color.blue);
}
  
#ifdef EXISTENCE_ENABLED
void Buffer::set_color(int pos, rgb_color color, bool display = false, int effect = NO_EFFECT, int id = NO_ID){
#else
void Buffer::set_color(int pos, rgb_color color, bool display = false, int effect = NO_EFFECT){
#endif
  buffer[pos] = color;
  
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
  
void Buffer::set_window(int window){
  this->window = (window < 1 || window > visible_led_count) ? visible_led_count : window;
}

int Buffer::get_window(){
  return window;
}

int Buffer::set_display(int display){
  current_strip = display;
}

// animate by shifting frame (future: shift in from back buffer)
void Buffer::shift(int count, int maxx, bool fast_render = true){
  maxx = min(maxx, safety_led_count);
  for(int i = 0; i < count; i++){
    render[i] = black;
  }
  for(int i = count; i < maxx; i++){
    if(fast_render)
      render[i] = renderer->fast_render(buffer[i - count], effects[i - count]);
    else
      render[i] = renderer->render(buffer[i - count], effects[i - count]);
  }

  display_buffer();
}

void Buffer::finalize_shift(int count, int max){
  for(int i = 0; i < count; i++){
    push_color(black, false, NO_EFFECT, max);
  }
}

#endif

//void draw(rgb_color color, int pos, int id){
//  colors[pos] = ColorMath::add_color(colors[pos], color);
//
//  int mirror = MAX_LED - pos;
//  colors[mirror] = ColorMath::add_color(colors[mirror], color);
//
//  existence[pos] |= bitmask[id];
//}
//
//void undraw(rgb_color color, int pos, int id){
//  existence[pos] &= ~bitmask[id];
//}
