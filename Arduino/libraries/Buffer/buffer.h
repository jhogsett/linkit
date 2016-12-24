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

//  buffer takes a pointer to render buffer and a single display buffer

class Buffer
{
  public:
#ifdef EXISTENCE_ENABLED
  void begin(PololuLedStripBase **ledStrips, byte default_brightness, float fade_rate, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color **buffers, rgb_color *render, byte **effects_buffers, byte *existence);
#else
  void begin(PololuLedStripBase **ledStrips, byte default_brightness, float fade_rate, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color **buffers, rgb_color *render, byte **effects_buffers);
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
  void cross_fade(int step);
  int get_window();
  void set_display(byte display);
  void set_buffer(byte buffer);
  rgb_color * get_buffer();
  byte * get_effects_buffer();
  byte get_current_display();

  // todo: is there an alternative to storing all these pointers?
  private:
  PololuLedStripBase **ledStrips;             // 4
  byte current_display;                         // 1
  rgb_color **buffers;                          // 4
  static rgb_color *render;                   // 4
  byte **effects_buffers;                              // 4
  float default_brightness_scale;             // 4
  Render *renderer;                           // 4
  byte safety_led_count;                      // 1
  byte visible_led_count;                     // 1
  float fade_rate;                            // 4

#ifdef EXISTENCE_ENABLED
  byte *existence;
#endif

  byte window = 0;
  void shift_buffer(rgb_color * buffer, byte * effects, byte max);
};

rgb_color *Buffer::render;

#ifdef EXISTENCE_ENABLED
void Buffer::begin(PololuLedStripBase **ledStrips, byte default_brightness, float fade_rate, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color **buffers, rgb_color *render, byte **effects_buffers, byte *existence){
#else
void Buffer::begin(PololuLedStripBase **ledStrips, byte default_brightness, float fade_rate, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color **buffers, rgb_color *render, byte **effects_buffers){
#endif

  this->ledStrips = ledStrips;
  this->current_display = 0;
  this->buffers = buffers;
  this->render = render;
  this->default_brightness_scale = default_brightness / 100.0;
  this->effects_buffers = effects_buffers;
  this->renderer = renderer;
  this->safety_led_count = safety_led_count;
  this->visible_led_count = visible_led_count;
  this->fade_rate = fade_rate;
  this->window = this->visible_led_count;
#ifdef EXISTENCE_ENABLED
  this->existence = existence;
#endif
}

// always write from the render buffer to a pin,
// the render buffer having been rendered from a specific display buffer already
void Buffer::display_buffer(rgb_color * pbuffer = render){
  ledStrips[current_display]->write(pbuffer, visible_led_count);
}
  
void Buffer::erase(bool display = false)
{
  // to do: restrict to current zone
  for(byte i = 0; i < visible_led_count; i++){
    buffers[current_display][i] = black;
    effects_buffers[current_display][i] = NO_EFFECT;

#ifdef EXISTENCE_ENABLED
    existence[i] = NO_ID;
#endif
  }

  if(display){
    display_buffer(buffers[current_display]);
  }
}

// to do: restrict to current zone
void Buffer::fade(float rate = 0.0){
  rate = (rate == 0.0) ? fade_rate : rate;
  byte *p = (byte *)buffers[current_display];
  for(int i = 0; i < visible_led_count * 3; i++){
    *(p + i) *= rate;
  }
}

// to do: restrict to current zone
void Buffer::fade_fast(){
  byte *p;
  p = (byte *)buffers[current_display];
  for(byte i = 0; i < visible_led_count * 3; i++){
    *(p + i) = *(p + i) >> 1;
  }
}

// to do: restrict to current zone
void Buffer::cross_fade(int step){
  for(int i = 0; i < visible_led_count; i++){
    rgb_color *pb = buffers[current_display] + i;
    rgb_color *pr = render + i;
    rgb_color rendered_color = renderer->render(*pb, effects_buffers[current_display][i]);
    *pr = ColorMath::crossfade_colors(step, *pr, rendered_color);
   }
}

// to do: set minimum position based on current zone
void Buffer::shift_buffer(rgb_color * buffer, byte * effects, byte max){
  for(byte i = max - 1; i >= 1; i--){
    buffer[i] = buffer[i-1];
    effects[i] = effects[i-1];

#ifdef EXISTENCE_ENABLED
    existence[i] = existence[i-1];
#endif

  }
}
  
#ifdef EXISTENCE_ENABLED
void Buffer::push_color(rgb_color color, bool display = false, byte effect = NO_EFFECT, byte max = 0, byte id = NO_ID)
#else
void Buffer::push_color(rgb_color color, bool display = false, byte effect = NO_EFFECT, byte max = 0)
#endif
{
  // to do: set default window by current zone
  max = (max == 0) ? window : max;

  shift_buffer(buffers[current_display], effects_buffers[current_display], max);

  // to do: offset by zone
  buffers[current_display][0] = ColorMath::correct_color(color);
  effects_buffers[current_display][0] = effect;

#ifdef EXISTENCE_ENABLED
  existence[0] = id;
#endif

  if(display){
    display_buffer(buffers[current_display]);
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
void Buffer::set_color(byte pos, rgb_color color, bool display = false, byte effect = NO_EFFECT, byte id = NO_ID)
#else
void Buffer::set_color(byte pos, rgb_color color, bool display = false, byte effect = NO_EFFECT)
#endif
{
  buffers[current_display][pos] = ColorMath::correct_color(color);
  
  if(effect != LEAVE_EFFECT){
    effects_buffers[current_display][pos] = effect;
  }

#ifdef EXISTENCE_ENABLED
  if(id != LEAVE_ID){
    existence[pos] = id;
  }
#endif

  if(display){
    display_buffer(buffers[current_display]);
  }
}
  
void Buffer::set_window(byte window){
  this->window = (window < 1 || window > visible_led_count) ? visible_led_count : window;
}

int Buffer::get_window(){
  return window;
}

void Buffer::set_display(byte display){
  current_display = display;
}

byte Buffer::get_current_display(){
  return current_display;
}

rgb_color * Buffer::get_buffer(){
  return buffers[current_display];
}

byte * Buffer::get_effects_buffer(){
  return effects_buffers[current_display];
}

// to do: operate on a specific display buffer
// to do: restrict to current zone
// animate by shifting frame (future: shift in from back buffer)
void Buffer::shift(byte count, byte maxx, bool fast_render = true){
  maxx = min(maxx, safety_led_count);
  for(byte i = 0; i < count; i++){
    render[i] = black;
  }
  for(byte i = count; i < maxx; i++){
    if(fast_render)
      render[i] = renderer->fast_render(buffers[current_display][i - count], effects_buffers[current_display][i - count]);
    else
      render[i] = renderer->render(buffers[current_display][i - count], effects_buffers[current_display][i - count]);
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
