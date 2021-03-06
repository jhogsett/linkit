#ifndef BUFFER_H
#define BUFFER_H

#include <PololuLedStrip.h>
#include <render.h>
#include <zones.h>

#define DEFAULT_EFFECT 127
#define NO_EFFECT 0
#define OVERRIDE_OFF -1

#define DRAW_MODE_WRITE 0
#define DRAW_MODE_PLUS  1
#define DRAW_MODE_MIX   2

class Buffer
{
  public:
  void begin(PololuLedStripBase **ledStrips, byte default_brightness, byte visible_led_count, Render *renderer, rgb_color **buffers, rgb_color *render, byte **effects_buffers, Zones * zones);

  void display_buffer(rgb_color * pbuffer);
  void render_display();
  void fast_erase(bool erase_effects = false);
  void erase(bool display);
  void push_color(rgb_color color, int times, bool display, byte effect, byte max, byte start, bool color_correction);
  void push_rgb_color(byte red, byte green, byte blue);
  void push_hsl_color(int hue, int sat, int lit);
  void push_carry_color();
  void shift(byte count, byte maxx, bool fast_render);
  void finalize_shift(byte count, byte max);
  // void set_color(byte pos, rgb_color color, bool display, byte effect);
  void fade(float rate);
  void fade_fast();
  byte get_window();

#ifdef USE_MULTIPLE_DISPLAYS
  void set_display(byte display);
  byte get_current_display();
#endif

  void set_buffer(byte buffer);
  rgb_color * get_buffer();
  byte * get_effects_buffer();
  rgb_color * get_render_buffer();
  void set_offset_override(int offset, bool fixup);
  void set_window_override(int window, bool fixup);
  byte get_offset();
  byte get_width();
  void set_zone(byte zone);
  void reset();
  void set_reverse(bool reverse);
  bool get_reverse();
  void rotate();
  // byte get_display();
  byte get_zones();
  void set_black_level(rgb_color black_level);
  void reset_black_level();
  rgb_color black;
  int get_default_brightness();
  void set_default_effect(byte effect);
  void set_draw_mode(byte mode = DRAW_MODE_WRITE);
  byte get_draw_mode();
  void uniform_cross_fade(int frame_delay);

#ifdef ROLLING_CROSSFADE      
  void rolling_cross_fade(int frame_delay);
#endif

  // todo: is there an alternative to storing all these pointers?
  private:

  static rgb_color *render;

  PololuLedStripBase **ledStrips;
  byte current_display;
  rgb_color **buffers;
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

  byte draw_mode = DRAW_MODE_WRITE;

  byte default_effect = NO_EFFECT;

  void shift_buffer(rgb_color * buffer, byte * effects, byte max, byte start, bool reverse);

#ifdef ROLLING_CROSSFADE      
  void do_rolling_position(byte led_pos, byte cfstep);
  void do_rolling_step(int step, byte max_led, byte max_step, byte offset);
#endif

  void cross_fade_step(byte step);
};

rgb_color *Buffer::render;

void Buffer::begin(PololuLedStripBase **ledStrips, byte default_brightness, byte visible_led_count, Render *renderer, rgb_color **buffers, rgb_color *render, byte **effects_buffers, Zones * zones){
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
  this->draw_mode = DRAW_MODE_WRITE;
}

// byte Buffer::get_display()
// {
//   return this->current_display;
// }

void Buffer::reset()
{
  this->set_zone(0);
  this->set_reverse(false);
}

// always write from the render buffer to a pin,
// the render buffer having been rendered from a specific display buffer already
void Buffer::display_buffer(rgb_color * pbuffer)
{
  ledStrips[current_display]->write(pbuffer, visible_led_count);
}

// todo might want choice of slow or fast render
void Buffer::render_display()
{
  renderer->render_buffer(render, this->buffers[current_display], visible_led_count, this->effects_buffers[current_display], true);
  display_buffer(render);
}

void Buffer::fast_erase(bool erase_effects)
{
  void * buf = (void*)buffers[current_display];
  memset(buf, 0, visible_led_count * 3);

  if(erase_effects)
  {
    void * effects = (void*)effects_buffers[current_display];
    memset(effects, 0, visible_led_count);
  }
}

void Buffer::erase(bool display = false)
{
  byte offset = get_offset();
  byte window = get_window();
  rgb_color * buf = buffers[current_display];
  byte * effects = effects_buffers[current_display];

  rgb_color erase_color = ColorMath ::correct_color(black);

  if(offset == window)
  {
    buf[offset] = erase_color;
    effects[offset] = NO_EFFECT;
  }
  else
  {
    for(byte i = offset; i < window; i++)
    {
      buf[i] = erase_color;
      effects[i] = NO_EFFECT;
    }
  }

  if(display)
    render_display();
}

void Buffer::cross_fade_step(byte step)
{
  byte offset = get_offset();
  byte window = get_window();
  rgb_color *buffer = buffers[current_display];
  byte* effects = effects_buffers[current_display];

  for(byte i = offset; i < window; i++){
    rgb_color *pb = buffer + i;
    rgb_color *pr = render + i;

#ifdef USE_COLOR_CACHE
    rgb_color rendered_color = renderer->render(pb, effects[i], 0.0, NULL, true);
#else
    rgb_color rendered_color = renderer->render(pb, effects[i], 0.0, NULL);
#endif

    // rgb_color rendered_color = renderer->get_default(*pb);
    *pr = ColorMath::crossfade_colors(step, *pr, rendered_color);
   }
}

void Buffer::uniform_cross_fade(int frame_delay)
{
  byte steps = ColorMath::crossfade_steps();
  for(byte step = 0; step <= steps; step++)
  {
    cross_fade_step(step);
    display_buffer(render);
    delay(frame_delay);
  }
}

#ifdef ROLLING_CROSSFADE      
void Buffer::do_rolling_position(byte led_pos, byte cfstep)
{
  rgb_color *pb = buffers[current_display] + led_pos;
  rgb_color *pr = render + led_pos;
  byte *effect = effects_buffers[current_display] + led_pos;

  rgb_color rendered_color = renderer->render(pb, *effect, 0.0, NULL);
  *pr = ColorMath::crossfade_colors(cfstep, *pr, rendered_color);
}

void Buffer::do_rolling_step(int step, byte max_led, byte max_step, byte offset)
{
  byte last_led_pos = min(step, max_led);
  bool ended = false;
  for(int led_pos = 0; led_pos <= last_led_pos; led_pos++){
    byte cross_fade_step = min(step - led_pos, max_step);
    if(!ended){
      do_rolling_position(led_pos + offset, cross_fade_step);
    }
    if(cross_fade_step == CROSSFADE_STEPS){
      ended = true;
    } 
  }
}

void Buffer::rolling_cross_fade(int frame_delay)
{
  byte offset = get_offset();
  byte window = get_window();
  byte num_leds = window - offset;
  byte max_led = num_leds - 1;
  byte max_step = CROSSFADE_STEPS - 1;
  int max_loops = max_led + max_step + 1;

  for(int step = 0; step < max_loops; step++){
    do_rolling_step(step, max_led, max_step, offset);
    display_buffer(render);
    delay(frame_delay);
  }
}
#endif

void Buffer::rotate()
{
  rgb_color * buffer = buffers[current_display];
  byte * effects = effects_buffers[current_display];
  byte max = get_window();
  byte start = get_offset();

  shift_buffer(buffer, effects, max, start, this->reverse);

  if(this->reverse)
  {
    buffer[max - 1] = this->carry_color;
    effects[max - 1] = this->carry_effect;
  }
  else
  {
    buffer[start] = this->carry_color;
    effects[start] = this->carry_effect;
  }
}

void Buffer::shift_buffer(rgb_color * buffer, byte * effects, byte max, byte start, bool reverse = false)
{
  if(reverse)
  {
    this->carry_color = buffer[start];
    this->carry_effect = effects[start];

    max -= 1;
    rgb_color * buf = buffer + start;
    byte * eff = effects + start;

    for(byte i = start; i < max; i++)
    {
      *buf = *(buf + 1);
      *eff = *(eff + 1);
      buf++;
      eff++;
    }
  }
  else
  {
    this->carry_color = buffer[max - 1];
    this->carry_effect = effects[max - 1];

    max -= 1;
    rgb_color * buf = buffer + max;
    byte * eff = effects + max;
    start += 1;

    for(byte i = max; i >= start; i--){
      *buf = *(buf - 1);
      *eff = *(eff - 1);
      buf--;
      eff--;
    }
  }
}

// todo: only need to shift buffer multiple times if displaying (otherwise, shift the whole amount)
void Buffer::push_color(rgb_color color, int times = 1, bool display = false, byte effect = DEFAULT_EFFECT, byte max = 0, byte start = 0, bool color_correction = true)
{
  rgb_color * buffer = buffers[current_display];
  byte * effects = effects_buffers[current_display];

  if(effect == DEFAULT_EFFECT)
    effect = this->default_effect;

  max = (max == 0) ? get_window() : max;
  start = (start == 0) ? get_offset() : start;
  times = max(1, times);

  if(color_correction)
    color = ColorMath ::correct_color(color);

  rgb_color * buf;
  byte * eff;
  if(this->reverse)
  {
    buf = &buffer[max-1];
    eff = &effects[max-1];
  }
  else
  {
    buf = &buffer[start];
    eff = &effects[start];
  }

  for(byte i = 0; i < times; i++)
  {
    shift_buffer(buffer, effects, max, start, this->reverse);

    switch(draw_mode){
      case DRAW_MODE_WRITE:
      default:
        *buf = color;
        break;
      case DRAW_MODE_PLUS:
        buf-> red += color.red;
        buf-> green += color.green;
        buf-> blue += color.blue;
        break;
      case DRAW_MODE_MIX:
        buf-> red = (buf->red + color.red) / 2;
        buf-> green = (buf->green + color.green) / 2;
        buf-> blue = (buf->blue + color.blue) / 2;
        break;
    }

    *eff = effect;
    if(display)
      render_display();
  }
}

void Buffer::push_rgb_color(byte red, byte green, byte blue)
{
  rgb_color color = (rgb_color){red, green, blue};
  color = ColorMath::unscale_color(color, default_brightness_scale);
  push_color(color);
}

void Buffer::push_hsl_color(int hue, int sat, int lit)
{
  rgb_color color = ColorMath::hsl_to_rgb(hue, sat, lit);
  push_rgb_color(color.red, color.green, color.blue);
}

void Buffer::push_carry_color()
{
  push_color(carry_color, 1, false, carry_effect);
}

// when setting override, should it fix the override to be normal?
// 2:win:4:off leaves window override at 2
void Buffer::set_window_override(int window, bool fixup = false)
{
  if(window < 0)
    this->window_override = OVERRIDE_OFF;
  else
  {
    this->window_override = max(1, min(visible_led_count, window));

    // fix up offset override if necessary
    if(fixup && this->offset_override != OVERRIDE_OFF){
      this->offset_override = min(this->offset_override, this->window_override - 1);
    }
  }
}

byte Buffer::get_window()
{
  if(this->window_override != OVERRIDE_OFF)
    return this->window_override;
  else
    return this->zones->zone_window(this->current_zone);
}

void Buffer::set_offset_override(int offset, bool fixup = false)
{
  if(offset < 0)
    this->offset_override = OVERRIDE_OFF;
  else
  {
    this->offset_override = max(0, min(visible_led_count-1, offset));

    // fix up window override if necessary
    if(fixup && this->window_override != OVERRIDE_OFF)
      this->window_override = max(this->window_override, this->offset_override + 1);
  }
}

byte Buffer::get_offset()
{
  if(this->offset_override != OVERRIDE_OFF)
    return this->offset_override;
  else
    return this->zones->zone_offset(this->current_zone);
}

byte Buffer::get_width()
{
  return this->get_window() - this->get_offset();
}

void Buffer::set_zone(byte zone)
{
  this->current_zone = max(0, min(this->get_zones() - 1, zone));
  this->window_override = OVERRIDE_OFF;
  this->offset_override = OVERRIDE_OFF;
}

#ifdef USE_MULTIPLE_DISPLAYS
void Buffer::set_display(byte display)
{
  this->current_display = display;
}

byte Buffer::get_current_display()
{
  return this->current_display;
}
#endif

rgb_color * Buffer::get_buffer()
{
  return this->buffers[current_display];
}

byte * Buffer::get_effects_buffer()
{
  return this->effects_buffers[current_display];
}

rgb_color * Buffer::get_render_buffer()
{
  return this->render;
}

void Buffer::set_reverse(bool reverse = true)
{
  this->reverse = reverse;
}

bool Buffer::get_reverse()
{
  return this->reverse;
}

byte Buffer::get_zones()
{
  return this->zones->get_num_zones();
}

void Buffer::set_black_level(rgb_color black_level)
{
  this->black = black_level;
}

void Buffer::reset_black_level()
{
  this->black = Render::black;
}

int Buffer::get_default_brightness()
{
  return (int)(this->default_brightness_scale * 100.0);
}

void Buffer::set_draw_mode(byte draw_mode){
  this->draw_mode = draw_mode;
}

byte Buffer::get_draw_mode(){
  return this->draw_mode;
}

void Buffer::set_default_effect(byte effect){
  this->default_effect = effect;
}


//// to do: support either orientation
//// to do: restrict to current zone
//// animate by shifting frame (rendering frame
//// buffer in different render buffer positions)
//void Buffer::shift(byte count, byte maxx, bool fast_render = true)
//{
//  rgb_color * buffer = buffers[current_display];
//  byte * effects = effects_buffers[current_display];
//
//  // to do: restrict to visible led count?
//
//  // this is used repeatedly during a shift animation to render the
//
//  // to do: start off offset
//  // to do: would it be faster to set each individual byte?
//  for(byte i = 0; i < count; i++)
//    this->render[i] = black;
//
//  // to do: add offset
//  for(byte i = count; i < maxx; i++)
//    if(fast_render)
//      this->render[i] = renderer->fast_render(buffer[i - count], effects[i - count]);
//    else
//      this->render[i] = renderer->render(&buffer[i - count], effects[i - count]);
//
//  display_buffer(this->render);
//}
//
//void Buffer::finalize_shift(byte count, byte max)
//{
//  // to do: restrict to current zone
////  for(byte i = 0; i < count; i++){
////    push_color(black, 1, false, NO_EFFECT, max);
////  }
//  push_color(black, count, false, NO_EFFECT, max);
//}

#endif
