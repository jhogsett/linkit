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
  void begin(PololuLedStripBase **ledStrips, byte default_brightness, float fade_rate, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color **buffers, rgb_color *render, byte **effects_buffers, byte num_zones, byte *zone_offsets, byte *zone_windows, byte *existence);
#else
  void begin(PololuLedStripBase **ledStrips, byte default_brightness, float fade_rate, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color **buffers, rgb_color *render, byte **effects_buffers, byte num_zones, byte *zone_offsets, byte *zone_windows);
#endif

  void display_buffer(rgb_color * pbuffer);
  void erase(bool display);
  void push_color(rgb_color color, bool display, byte effect, byte max, byte start);
  void push_rgb_color(byte red, byte green, byte blue);
  void push_hsl_color(int hue, int sat, int lit);
  void push_carry_color();
  void shift(byte count, byte maxx, bool fast_render);
  void finalize_shift(byte count, byte max);
  void set_color(byte pos, rgb_color color, bool display, byte effect);
  void set_window_override(byte window);
  void fade(float rate);
  void fade_fast();
  void cross_fade(int step);
  int get_window();
  void set_display(byte display);
  void set_buffer(byte buffer);
  rgb_color * get_buffer();
  byte * get_effects_buffer();
  byte get_current_display();
  rgb_color * get_render_buffer();
  void set_offset_override(byte offset);
  byte get_offset();
  byte get_width();
  void set_zone(byte zone);
  void reset();
  void set_reverse(bool reverse);
  void rotate();
  byte get_display();

  static const rgb_color black;

  // todo: is there an alternative to storing all these pointers?
  private:
  PololuLedStripBase **ledStrips;
  byte current_display;
  rgb_color **buffers;
  static rgb_color *render;
  byte **effects_buffers;
  float default_brightness_scale;
  Render *renderer;
  byte safety_led_count;
  byte visible_led_count;
  float fade_rate;
  rgb_color carry_color;
  byte carry_effect;

#ifdef EXISTENCE_ENABLED
  byte *existence;
#endif

  byte window_override = 0;
  byte offset_override = 0;

  byte num_zones;
  byte *zone_offsets;
  byte *zone_windows;
  byte current_zone = 0;
  bool reverse = false;

  void shift_buffer(rgb_color * buffer, byte * effects, byte max, byte start, bool reverse);
};

const rgb_color Buffer::black = {0,0,0};

rgb_color *Buffer::render;

#ifdef EXISTENCE_ENABLED
void Buffer::begin(PololuLedStripBase **ledStrips, byte default_brightness, float fade_rate, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color **buffers, rgb_color *render, byte **effects_buffers, byte num_zones, byte *zone_offsets, byte *zone_windows, byte *existence){
#else
void Buffer::begin(PololuLedStripBase **ledStrips, byte default_brightness, float fade_rate, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color **buffers, rgb_color *render, byte **effects_buffers, byte num_zones, byte *zone_offsets, byte *zone_windows){
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
  this->window_override = 0;
  this->offset_override = 0;
#ifdef EXISTENCE_ENABLED
  this->existence = existence;
#endif

  this->num_zones = num_zones;
  this->zone_offsets = zone_offsets;
  this->zone_windows = zone_windows;
  this->reverse = false;
  this->carry_color = black;
  this->carry_effect = NO_EFFECT;
}

byte Buffer::get_display(){
  return this->current_display;
}

void Buffer::reset(){
  this->set_zone(0);
  this->set_display(0);
  this->set_reverse(false);
}

// always write from the render buffer to a pin,
// the render buffer having been rendered from a specific display buffer already
void Buffer::display_buffer(rgb_color * pbuffer){
  ledStrips[current_display]->write(pbuffer, visible_led_count);
}
  
void Buffer::erase(bool display = false)
{
  for(byte i = get_offset(); i < get_window(); i++){
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

void Buffer::fade(float rate = 0.0){
  rate = (rate == 0.0) ? fade_rate : rate;
  byte *p = (byte *)buffers[current_display];
  for(int i = get_offset(); i < get_window() * 3; i++){
    *(p + i) *= rate;
  }
}

void Buffer::fade_fast(){
  byte *p;
  p = (byte *)buffers[current_display];
  for(byte i = get_offset(); i < get_window() * 3; i++){
    *(p + i) = *(p + i) >> 1;
  }
}

void Buffer::cross_fade(int step){
  for(int i = get_offset(); i < get_window(); i++){
    rgb_color *pb = buffers[current_display] + i;
    rgb_color *pr = render + i;
    rgb_color rendered_color = renderer->render(*pb, effects_buffers[current_display][i]);
    *pr = ColorMath::crossfade_colors(step, *pr, rendered_color);
   }
}

void Buffer::rotate(){
  rgb_color * buffer = buffers[current_display];
  byte * effects = effects_buffers[current_display];
  byte max = get_window();
  byte start = get_offset();

  rgb_color carry_color;
  byte carry_effect;
  if(reverse){
    carry_color = buffer[start];
    carry_effect = effects[start];
  } else {
    carry_color = buffer[max - 1];
    carry_effect = effects[max - 1];
  }

  shift_buffer(buffer, effects, max, start, this->reverse);

  if(reverse){
    buffer[max - 1] = carry_color;
    effects[max - 1] = carry_effect;
  } else {
    buffer[start] = carry_color;
    effects[start] = carry_effect;
  }
}

void Buffer::shift_buffer(rgb_color * buffer, byte * effects, byte max, byte start, bool reverse = false){
  if(reverse){
    carry_color = buffer[start];
    carry_effect = effects[start];

    for(byte i = start; i < (max - 1); i++){
      buffer[i] = buffer[i+1];
      effects[i] = effects[i+1];

#ifdef EXISTENCE_ENABLED
      existence[i] = existence[i+1];
#endif
    }
  } else {
    carry_color = buffer[max - 1];
    carry_effect = effects[max - 1];

    for(byte i = max - 1; i >= (start + 1); i--){
      buffer[i] = buffer[i-1];
      effects[i] = effects[i-1];

#ifdef EXISTENCE_ENABLED
      existence[i] = existence[i-1];
#endif
    }
  }
}

#ifdef EXISTENCE_ENABLED
void Buffer::push_color(rgb_color color, bool display = false, byte effect = NO_EFFECT, byte max = 0, byte id = NO_ID, byte start = 0)
#else
void Buffer::push_color(rgb_color color, bool display = false, byte effect = NO_EFFECT, byte max = 0, byte start = 0)
#endif
{
  max = (max == 0) ? get_window() : max;
  start = (start == 0) ? get_offset() : start;

  shift_buffer(buffers[current_display], effects_buffers[current_display], max, start, this->reverse);

  if(this->reverse){
    buffers[current_display][max-1] = ColorMath::correct_color(color);
    effects_buffers[current_display][max-1] = effect;
#ifdef EXISTENCE_ENABLED
    existence[max-1] = id;
#endif
  } else {
    buffers[current_display][start] = ColorMath::correct_color(color);
    effects_buffers[current_display][start] = effect;
#ifdef EXISTENCE_ENABLED
    existence[start] = id;
#endif
  }

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

void Buffer::push_carry_color(){
  push_color(carry_color, false, carry_effect);
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
  
void Buffer::set_window_override(byte window){
  this->window_override = max(1, min(visible_led_count + 1, window));
}

int Buffer::get_window(){
  if(this->window_override != 0){
    return this->window_override;
  } else {
    return this->zone_windows[this->current_zone];
  }
}

// to do: consider using -1 to indicate "auto" offset
void Buffer::set_offset_override(byte offset){
  this->offset_override = max(0, min(this->get_window() - 1, offset));
}

byte Buffer::get_offset(){
  if(this->offset_override != 0){
    return this->offset_override;
  } else {
    return this->zone_offsets[this->current_zone];
  }
}

byte Buffer::get_width(){
  return this->get_window() - this->get_offset();
}

void Buffer::set_zone(byte zone){
  this->current_zone = max(0, min(this->num_zones - 1, zone));
  this->window_override = 0;
  this->offset_override = 0;
}

void Buffer::set_display(byte display){
  this->current_display = display;
}

byte Buffer::get_current_display(){
  return this->current_display;
}

rgb_color * Buffer::get_buffer(){
  return this->buffers[current_display];
}

byte * Buffer::get_effects_buffer(){
  return this->effects_buffers[current_display];
}

rgb_color * Buffer::get_render_buffer(){
  return this->render;
}

void Buffer::set_reverse(bool reverse = true){
  this->reverse = reverse;
}

// to do: support either orientation
// to do: restrict to current zone
// animate by shifting frame (rendering frame
// buffer in different render buffer positions)
void Buffer::shift(byte count, byte maxx, bool fast_render = true){

  // to do: restrict to visible led count?
  maxx = min(maxx, safety_led_count);

  // to do: start off offset
  for(byte i = 0; i < count; i++){
    render[i] = black;
  }
  // to do: add offset
  for(byte i = count; i < maxx; i++){
    if(fast_render)
      render[i] = renderer->fast_render(buffers[current_display][i - count], effects_buffers[current_display][i - count]);
    else
      render[i] = renderer->render(buffers[current_display][i - count], effects_buffers[current_display][i - count]);
  }

  display_buffer(this->render);
}

void Buffer::finalize_shift(byte count, byte max){

  // to do: restrict to current zone
  for(byte i = 0; i < count; i++){
    push_color(black, false, NO_EFFECT, max);
  }
}

#endif
