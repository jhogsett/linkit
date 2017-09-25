#ifndef RENDER_H
#define RENDER_H

#include <effects_processor.h>

#define NO_BLINKC

class Render
{
  public:
  void begin(BlinkEffects *blink_effects, BreatheEffects *breathe_effects, FadeEffects * fade_effects, byte default_brightness, byte minimum_brightness);
  rgb_color render(rgb_color * color, byte effect);
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

  rgb_color get_blink(rgb_color color, byte effect);
  rgb_color get_breathe(rgb_color color);
  rgb_color get_static();
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
rgb_color Render::get_blink(rgb_color color, byte effect)
{
  return ColorMath::scale_color(color, blink_effects->blink_on(effect) ? default_brightness_scale : minimum_brightness_scale);
}

rgb_color Render::get_breathe(rgb_color color)
{
  return ColorMath::scale_color(color, breathe_effects->breathe_ratio() * default_brightness_scale);
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
rgb_color Render::get_default(rgb_color color)
{
  return ColorMath::scale_color(color, default_brightness_scale);
}

rgb_color Render::render(rgb_color *color, byte effect)
{
                         if(effect == STATIC_ON) { return get_default(get_static()); } else
    if(blink_effects->is_handled_effect(effect)) { return get_blink(*color, effect); } else
  if(breathe_effects->is_handled_effect(effect)) { return get_breathe(*color);       } else
     if(fade_effects->is_handled_effect(effect)) { return get_fade(color, effect);   } else
                                                 { return get_default(*color);       }
}

rgb_color Render::fast_render(rgb_color color, byte _effect)
{
    return get_default(color);
}

void Render::render_buffer(rgb_color *dest_buffer, rgb_color *src_buffer, byte count, byte *effects)
{
  for(byte i = 0; i < count; i++)
    dest_buffer[i] = render(&src_buffer[i], effects[i]);
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
