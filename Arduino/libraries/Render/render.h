#ifndef RENDER_H
#define RENDER_H

#include <effects_processor.h>

#define NO_BLINKC

#define NO_EFFECT 0

#define DYNAMIC_COLOR 0x80
#define NOT_DYNAMIC_COLOR 0x7f

class Render
{
  public:
  void begin(BlinkEffects *blink_effects, BreatheEffects *breathe_effects, FadeEffects * fade_effects, byte default_brightness, byte minimum_brightness);
  rgb_color render(rgb_color * color, byte effect, byte * cache = NULL, float breathe_scale = 0.0);
  rgb_color fast_render(rgb_color color, byte _effect);
  void render_buffer(rgb_color *dest_buffer, rgb_color *src_buffer, byte count, byte *effects);
  void set_default_brightness(byte brightness);
  void set_minimum_brightness(byte brightness);
  byte get_minimum_brightness();

  static rgb_color black;

  private:
  BlinkEffects *blink_effects;
  BreatheEffects *breathe_effects;
  FadeEffects *fade_effects;
  byte default_brightness;
  float default_brightness_scale;
  float minimum_brightness_scale;

  rgb_color get_blink(rgb_color color, rgb_color render_color, byte effect, byte * cache = NULL);
  rgb_color get_breathe(rgb_color color, float scale);
  rgb_color get_static();
  rgb_color get_default(rgb_color, byte effect = NO_EFFECT);
  rgb_color get_fade(rgb_color * color, byte effect);
};

rgb_color Render::black = {0, 0, 0};

void Render::begin(BlinkEffects *blink_effects, BreatheEffects *breathe_effects, FadeEffects *fade_effects, byte default_brightness, byte minimum_brightness)
{
    this->blink_effects = blink_effects;
    this->breathe_effects = breathe_effects;
    this->fade_effects = fade_effects;
    this->default_brightness = default_brightness;
    this->default_brightness_scale = default_brightness / 100.0;
    this->minimum_brightness_scale = minimum_brightness / 100.0;
}

// todo: move to blink class?
rgb_color Render::get_blink(rgb_color color, rgb_color render_color, byte effect, byte * cache)
{
    byte effect_only = effect & NOT_DYNAMIC_COLOR;
    float scale;

    if(effect_only == BLINK_ON_D){
      byte color_index = min((blink_effects->blink_on_cached(cache, BLINK_ON_D) ? color.red : color.green), NUM_PALETTE_COLORS-1);
      render_color = Colors::get_palette()[color_index];
      scale = default_brightness_scale;
    } else {
      scale = blink_effects->blink_on_cached(cache, effect_only) ? default_brightness_scale : minimum_brightness_scale;
    }
    return ColorMath::scale_color(render_color, scale);
}

rgb_color Render::get_breathe(rgb_color color, float scale)
{
  return ColorMath::scale_color(color, scale);
}

// this is a destructive rendering: the original color value is reduced
rgb_color Render::get_fade(rgb_color *color, byte effect)
{
  *color = fade_effects->apply_fade(*color, effect);
  return get_default(*color);
}

rgb_color Render::get_static()
{
  return ColorMath::random_color();
}

// default_brightness_scale is 0.0 - 1.0
rgb_color Render::get_default(rgb_color color, byte effect)
{
  return ColorMath::scale_color(color, default_brightness_scale);
}

rgb_color Render::render(rgb_color *color, byte effect, byte *cache, float breathe_scale)
{
  byte effect_only = effect & NOT_DYNAMIC_COLOR;
  if(effect_only == STATIC_ON)
    return get_default(get_static());

  if(fade_effects->is_handled_effect(effect_only))
    return get_fade(color, effect);

  rgb_color render_color;
  if(effect & DYNAMIC_COLOR)
    render_color = ColorMath ::correct_color(Colors::get_palette()[min(color->red, NUM_PALETTE_COLORS-1)]);
  else
    render_color = *color;

  if(blink_effects->is_handled_effect(effect_only))
    return get_blink(*color, render_color, effect, cache);

  if(breathe_effects->is_handled_effect(effect_only))
    return get_breathe(render_color, breathe_scale);

  return get_default(render_color, effect);
}

rgb_color Render::fast_render(rgb_color color, byte _effect)
{
    return get_default(color);
}

void Render::render_buffer(rgb_color *dest_buffer, rgb_color *src_buffer, byte count, byte *effects)
{
  byte cache[10];
  blink_effects->begin_render(cache);
  float breathe_scale = breathe_effects->breathe_ratio() * default_brightness_scale;

  for(byte i = 0; i < count; i++)
    dest_buffer[i] = render(&src_buffer[i], effects[i], cache, breathe_scale);
}

#define RESTRICT_TO_0_100(n) (max(0, min(100, n)))

void Render::set_default_brightness(byte brightness)
{
  this->default_brightness = brightness;
  this->default_brightness_scale = RESTRICT_TO_0_100(brightness) / 100.0;
}

void Render::set_minimum_brightness(byte brightness)
{
  this->minimum_brightness_scale = RESTRICT_TO_0_100(brightness) / 100.0;
}

byte Render::get_minimum_brightness()
{
  return int(this->minimum_brightness_scale * 100.0);
}

#endif
