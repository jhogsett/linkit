#ifndef RENDER_H
#define RENDER_H

#include <effects_processor.h>

#define NO_BLINKC

#define NO_EFFECT 0

#define DYNAMIC_COLOR 0x80
#define NOT_DYNAMIC_COLOR 0x7f

#ifdef USE_COLOR_CACHE
// cache rendered forms of dynamic color pairs while rendering
#define COLOR_CACHE_SIZE 6
#endif

class Render
{
  public:
  void begin(BlinkEffects *blink_effects, BreatheEffects *breathe_effects, FadeEffects * fade_effects, byte default_brightness, byte minimum_brightness);

#ifdef USE_COLOR_CACHE
  rgb_color render(rgb_color * color, byte effect, float breathe_scale, rgb_color* color_cache, bool effects_enabled);
#else
  rgb_color render(rgb_color * color, byte effect, float breathe_scale, bool effects_enabled);
#endif

  void render_buffer(rgb_color *dest_buffer, rgb_color *src_buffer, byte count, byte *effects, bool effects_enabled);
  void set_default_brightness(byte brightness);
  void set_minimum_brightness(byte brightness);
  byte get_minimum_brightness();

#ifdef USE_COLOR_CACHE
  rgb_color compute_dynamic_breathe(rgb_color color);
#endif

#ifdef USE_DYNAMIC_BREATHE
rgb_color get_dynamic_breathe(rgb_color color);
#endif

  static rgb_color black;

  private:
  BlinkEffects *blink_effects;
  BreatheEffects *breathe_effects;
  FadeEffects *fade_effects;
  byte default_brightness;
  float default_brightness_scale;
  float minimum_brightness_scale;

  rgb_color get_blink(rgb_color color, rgb_color render_color, byte effect, byte * cache = NULL);

#ifdef USE_COLOR_CACHE
  rgb_color get_breathe(rgb_color color, float scale, byte effect, rgb_color orig_color, rgb_color* color_cache);
#else
  rgb_color get_breathe(rgb_color color, float scale, byte effect, rgb_color orig_color);
#endif
  rgb_color get_default(rgb_color);
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
      byte color_index = min((blink_effects->blink_on(effect_only) ? color.red : color.green), NUM_PALETTE_COLORS-1);
      render_color = ColorMath::correct_color(Colors::get_palette()[color_index]);
      scale = default_brightness_scale;
    } else {
      scale = blink_effects->blink_on(effect_only) ? default_brightness_scale : minimum_brightness_scale;
    }
    return ColorMath::scale_color(render_color, scale);
}

#ifdef USE_COLOR_CACHE
rgb_color Render::compute_dynamic_breathe(rgb_color color)
{
  rgb_color color1 = Colors::get_palette()[color.red];
  rgb_color color2 = Colors::get_palette()[color.green];
  return this->breathe_effects->breathe_crossfade(color1, color2);
}
#endif

#ifdef USE_DYNAMIC_BREATHE
rgb_color Render::get_dynamic_breathe(rgb_color color)
{
  byte color_index = this->breathe_effects->alt_breathe() ? color.green : color.red;
  return ColorMath::correct_color(Colors::get_palette()[color_index]);
}
#endif

#ifdef USE_COLOR_CACHE
rgb_color Render::get_breathe(rgb_color color, float scale, byte effect, rgb_color orig_color, rgb_color* color_cache)
#else
rgb_color Render::get_breathe(rgb_color color, float scale, byte effect, rgb_color orig_color)
#endif
{
    byte effect_only = effect & NOT_DYNAMIC_COLOR;

#ifdef USE_DYNAMIC_BREATHE
    rgb_color result;
    result = (effect_only == BREATHE_ON_D) ? get_dynamic_breathe(orig_color) : color;
    return ColorMath::scale_color(result, scale);
#else
    return ColorMath::scale_color(color, scale);
#endif
}

// this is a destructive rendering: the original color value is reduced
rgb_color Render::get_fade(rgb_color *color, byte effect)
{
  *color = fade_effects->apply_fade(*color, effect);
  return get_default(*color);
}

// default_brightness_scale is 0.0 - 1.0
rgb_color Render::get_default(rgb_color color)
{
  return ColorMath::scale_color(color, default_brightness_scale);
}

#ifdef USE_COLOR_CACHE
rgb_color Render::render(rgb_color *color, byte effect, float breathe_scale, rgb_color* color_cache, bool effects_enabled)
#else
rgb_color Render::render(rgb_color *color, byte effect, float breathe_scale, bool effects_enabled)
#endif
{
  rgb_color render_color;
  if(effect & DYNAMIC_COLOR)
    render_color = ColorMath::correct_color(Colors::get_palette()[min(color->red, NUM_PALETTE_COLORS-1)]);
  else
    render_color = *color;

  if(effects_enabled)
  {
    byte effect_only = effect & NOT_DYNAMIC_COLOR;

    if(fade_effects->is_handled_effect(effect_only))
      return get_fade(color, effect);

    if(blink_effects->is_handled_effect(effect_only))
      return get_blink(*color, render_color, effect);

    if(breathe_effects->is_handled_effect(effect_only))
#ifdef USE_COLOR_CACHE
      return get_breathe(render_color, breathe_scale, effect, *color, color_cache);
#else
      return get_breathe(render_color, breathe_scale, effect, *color);
#endif
  }

  return get_default(render_color);
}

void Render::render_buffer(rgb_color *dest_buffer, rgb_color *src_buffer, byte count, byte *effects, bool effects_enabled)
{
  float breathe_scale;
#ifdef USE_COLOR_CACHE  
  rgb_color color_cache_data[COLOR_CACHE_SIZE * COLOR_CACHE_SIZE];
#endif
  if(effects_enabled)
  {
    breathe_scale = breathe_effects->breathe_ratio() * default_brightness_scale;
#ifdef USE_COLOR_CACHE  
    memset(color_cache_data, 0, sizeof(color_cache_data));
#endif
  }

  for(byte i = 0; i < count; i++)
#ifdef USE_COLOR_CACHE  
    dest_buffer[i] = render(&src_buffer[i], effects[i], breathe_scale, color_cache_data, effects_enabled);
#else
    dest_buffer[i] = render(&src_buffer[i], effects[i], breathe_scale, effects_enabled);
#endif
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
