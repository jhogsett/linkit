#ifndef BUFFER_H
#define BUFFER_H

#include <PololuLedStrip.h>
#include <render.h>
#include <zones.h>

#define LEAVE_EFFECT -1
#define NO_EFFECT 0

#define OVERRIDE_OFF -1

class Buffer
{
  public:
  void begin(PololuLedStripBase **ledStrips, byte default_brightness, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color **buffers, rgb_color *render, byte **effects_buffers, Zones * zones);

  void display_buffer(rgb_color * pbuffer);
  void render_display();
  void erase(bool display);
  void push_color(rgb_color color, byte times, bool display, byte effect, byte max, byte start);
  void push_rgb_color(byte red, byte green, byte blue);
  void push_hsl_color(int hue, int sat, int lit);
  void push_carry_color();
  void shift(byte count, byte maxx, bool fast_render);
  void finalize_shift(byte count, byte max);
  // void set_color(byte pos, rgb_color color, bool display, byte effect);
  void fade(float rate);
  void fade_fast();
  void cross_fade(byte step);
  byte get_window();
  void set_display(byte display);
  void set_buffer(byte buffer);
  rgb_color * get_buffer();
  byte * get_effects_buffer();
  byte get_current_display();
  rgb_color * get_render_buffer();
  void set_offset_override(byte offset, bool fixup);
  void set_window_override(byte window, bool fixup);
  byte get_offset();
  byte get_width();
  void set_zone(byte zone);
  void reset();
  void set_reverse(bool reverse);
  bool get_reverse();
  void rotate();
  byte get_display();
  byte get_zones();
  void set_black_level(rgb_color black_level);
  void reset_black_level();
  rgb_color black;
  int get_default_brightness();

  // todo: is there an alternative to storing all these pointers?
  private:
  PololuLedStripBase **ledStrips;
  byte current_display;
  rgb_color **buffers;
  static rgb_color *render;
  byte **effects_buffers;
  float default_brightness_scale;
  Render *renderer;
  byte visible_led_count;
  rgb_color carry_color;
  byte carry_effect;

  int window_override = OVERRIDE_OFF;
  int offset_override = OVERRIDE_OFF;

  Zones *zones;
  byte current_zone = 0;
  bool reverse = false;

  void shift_buffer(rgb_color * buffer, byte * effects, byte max, byte start, bool reverse);
};

rgb_color *Buffer::render;

void Buffer::begin(PololuLedStripBase **ledStrips, byte default_brightness, byte safety_led_count, byte visible_led_count, Render *renderer, rgb_color **buffers, rgb_color *render, byte **effects_buffers, Zones * zones){
  this->black = Render::black;
  this->ledStrips = ledStrips;
  this->current_display = 0;
  this->buffers = buffers;
  this->render = render;
  this->default_brightness_scale = default_brightness / 100.0;
  this->effects_buffers = effects_buffers;
  this->renderer = renderer;
  this->visible_led_count = visible_led_count;
  this->window_override = OVERRIDE_OFF;
  this->offset_override = OVERRIDE_OFF;
  this->zones = zones;
  this->reverse = false;
  this->carry_color = black;
  this->carry_effect = NO_EFFECT;
}

byte Buffer::get_display(){
  return this->current_display;
}

void Buffer::reset(){
  this->set_zone(0);
  this->set_reverse(false);
}

// always write from the render buffer to a pin,
// the render buffer having been rendered from a specific display buffer already
void Buffer::display_buffer(rgb_color * pbuffer){
  ledStrips[current_display]->write(pbuffer, visible_led_count);
}

// todo might want choice of slow or fast render
void Buffer::render_display(){
  renderer->render_buffer(render, this->buffers[current_display], visible_led_count, this->effects_buffers[current_display]);
  display_buffer(render);
}

void Buffer::erase(bool display = false)
{
  byte offset = get_offset();
  byte window = get_window();

  rgb_color * buf = buffers[current_display];
  byte * effects = effects_buffers[current_display];

  for(byte i = offset; i < window; i++){
    buf[i] = black;
    effects[i] = NO_EFFECT;
  }

  if(display){
    render_display();
  }
}

void Buffer::cross_fade(byte step){
  byte offset = get_offset();
  byte window = get_window();
  rgb_color *buffer = buffers[current_display];
  byte* effects = effects_buffers[current_display];

  for(byte i = offset; i < window; i++){
    rgb_color *pb = buffer + i;
    rgb_color *pr = render + i;
    rgb_color rendered_color = renderer->render(pb, effects[i]);
    *pr = ColorMath::crossfade_colors(step, *pr, rendered_color);
   }
}

void Buffer::rotate(){
  rgb_color * buffer = buffers[current_display];
  byte * effects = effects_buffers[current_display];
  byte max = get_window();
  byte start = get_offset();

  shift_buffer(buffer, effects, max, start, this->reverse);

  if(reverse){
    buffer[max - 1] = this->carry_color;
    effects[max - 1] = this->carry_effect;
  } else {
    buffer[start] = this->carry_color;
    effects[start] = this->carry_effect;
  }
}

void Buffer::shift_buffer(rgb_color * buffer, byte * effects, byte max, byte start, bool reverse = false){
  if(reverse){
    this->carry_color = buffer[start];
    this->carry_effect = effects[start];

    rgb_color * buf = buffer + start;
    byte * eff = effects + start;
    max -= 1;

    for(byte i = start; i < max; i++){
      *buf = *(buf + 1);
      *eff = *(eff + 1);
      buf++;
      eff++;
    }
  } else {
    this->carry_color = buffer[max - 1];
    this->carry_effect = effects[max - 1];

    max -= 1;
    start += 1;
    rgb_color * buf = buffer + max;
    byte * eff = effects + max;

    for(byte i = max; i >= start; i--){
      *buf = *(buf - 1);
      *eff = *(eff - 1);
      buf--;
      eff--;
    }
  }
}

// todo: only need to shift buffer multiple times if displaying (otherwise, shift the whole amount)
void Buffer::push_color(rgb_color color, byte times = 1, bool display = false, byte effect = NO_EFFECT, byte max = 0, byte start = 0)
{
  rgb_color * buffer = buffers[current_display];
  byte * effects = effects_buffers[current_display];

  max = (max == 0) ? get_window() : max;
  start = (start == 0) ? get_offset() : start;
  times = max(1, times);

  color = ColorMath ::correct_color(color);

  rgb_color * buf;
  byte * eff;
  if(this->reverse){
    buf = &buffer[max-1];
    eff = &effects[max-1];
  } else {
    buf = &buffer[start];
    eff = &effects[start];
  }

  for(byte i = 0; i < times; i++){
    shift_buffer(buffer, effects, max, start, this->reverse);
    *buf = color;
    *eff = effect;
    
    if(display){
      render_display();
    }
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
  push_color(carry_color, 1, false, carry_effect);
}

// todo: is this used?
//void Buffer::set_color(byte pos, rgb_color color, bool display = false, byte effect = NO_EFFECT)
//{
//  rgb_color * buffer = buffers[current_display];
//  byte * effects = effects_buffers[current_display];
//
//  buffer[pos] = ColorMath::correct_color(color);
//
//  if(effect != LEAVE_EFFECT){
//    effects[pos] = effect;
//  }
//
//  if(display){
//    render_display();
//  }
//}

// when setting override, should it fix the override to be normal?
// 2:win:4:off leaves window override at 2
void Buffer::set_window_override(byte window, bool fixup = false){
  if(window < 0){
    this->window_override = OVERRIDE_OFF;
  } else {
    this->window_override = max(1, min(visible_led_count + 1, window));

    // fix up offset override if necessary
    if(fixup && this->offset_override != OVERRIDE_OFF){
      this->offset_override = min(this->offset_override, this->window_override - 1);
    }
  }
}

byte Buffer::get_window(){
  if(this->window_override != OVERRIDE_OFF){
    return this->window_override;
  } else {
    return this->zones->zone_window(this->current_zone);
  }
}

void Buffer::set_offset_override(byte offset, bool fixup = false){
  if(offset < 0){
    this->offset_override = OVERRIDE_OFF;
  } else {
    this->offset_override = max(0, min(visible_led_count, offset));

    // fix up window override if necessary
    if(fixup && this->window_override != OVERRIDE_OFF){
      this->window_override = max(this->window_override, this->offset_override + 1);
    }
  }
}

byte Buffer::get_offset(){
  if(this->offset_override != OVERRIDE_OFF){
    return this->offset_override;
  } else {
    return this->zones->zone_offset(this->current_zone);
  }
}

byte Buffer::get_width(){
  return this->get_window() - this->get_offset();
}

void Buffer::set_zone(byte zone){
  this->current_zone = max(0, min(this->get_zones() - 1, zone));
  this->window_override = OVERRIDE_OFF;
  this->offset_override = OVERRIDE_OFF;
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

bool Buffer::get_reverse(){
  return this->reverse;
}

byte Buffer::get_zones(){
  return this->zones->get_num_zones();
}

void Buffer::set_black_level(rgb_color black_level){
  this->black = black_level;
}

void Buffer::reset_black_level(){
  this->black = Render::black;
}

int Buffer::get_default_brightness(){
  return (int)(this->default_brightness_scale * 100.0);
}

// to do: support either orientation
// to do: restrict to current zone
// animate by shifting frame (rendering frame
// buffer in different render buffer positions)
void Buffer::shift(byte count, byte maxx, bool fast_render = true){
  rgb_color * buffer = buffers[current_display];
  byte * effects = effects_buffers[current_display];

  // to do: restrict to visible led count?

  // this is used repeatedly during a shift animation to render the

  // to do: start off offset
  // to do: would it be faster to set each individual byte?
  for(byte i = 0; i < count; i++){
    this->render[i] = black;
  }

  // to do: add offset
  for(byte i = count; i < maxx; i++){
    if(fast_render)
      this->render[i] = renderer->fast_render(buffer[i - count], effects[i - count]);
    else
      this->render[i] = renderer->render(&buffer[i - count], effects[i - count]);
  }

  display_buffer(this->render);
}

void Buffer::finalize_shift(byte count, byte max){

  // to do: restrict to current zone
//  for(byte i = 0; i < count; i++){
//    push_color(black, 1, false, NO_EFFECT, max);
//  }
  push_color(black, count, false, NO_EFFECT, max);
}

#endif
