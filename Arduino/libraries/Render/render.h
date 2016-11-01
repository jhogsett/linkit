#ifndef RENDER_H
#define RENDER_H

extern rgb_color colors[LED_COUNT];
extern int effects[LED_COUNT];

#define DEFAULT_BRIGHTNESS_SCALE (DEFAULT_BRIGHTNESS_PERCENT / 100.0)
#define MINIMUM_BRIGHTNESS_SCALE (MINIMUM_BRIGHTNESS_PERCENT / 100.0)

class Render
{
  public:
  void begin(BlinkEffects *blink_effects, BreatheEffects *breathe_effects);
  rgb_color render(rgb_color color, int effect);
  rgb_color fast_render(rgb_color color, int effect);
  void render_buffer(rgb_color *dest_buffer, rgb_color *src_buffer, int *effects);

  private:
  BlinkEffects *blink_effects;
  BreatheEffects *breathe_effects;
  rgb_color get_blink(rgb_color color, int effect);
  rgb_color get_breathe(rgb_color color);
  rgb_color get_static();
  rgb_color get_default(rgb_color);
};

void Render::begin(BlinkEffects *blink_effects, BreatheEffects *breathe_effects){
    this->blink_effects = blink_effects;
    this->breathe_effects = breathe_effects;
}

rgb_color Render::get_blink(rgb_color color, int effect){
  rgb_color render_color;
  if(blink_effects->blink_on(effect))
  {
    render_color = ColorMath::scale_color(color, DEFAULT_BRIGHTNESS_SCALE);
  }
  else
  {
    render_color = ColorMath::scale_color(color, MINIMUM_BRIGHTNESS_SCALE);
  }
  return render_color;
}

rgb_color Render::get_breathe(rgb_color color){
  return ColorMath::scale_color(color, breathe_effects->breathe_ratio() * DEFAULT_BRIGHTNESS_SCALE);
}

rgb_color Render::get_static(){
  return ColorMath::random_color();
}

rgb_color Render::get_default(rgb_color color){
  return ColorMath::scale_color(color, DEFAULT_BRIGHTNESS_SCALE);
}

rgb_color Render::render(rgb_color color, int effect){
  rgb_color render_color;

                        if(effect ==  STATIC_ON) { render_color = get_static();             } else
      if(blink_effects->is_blink_effect(effect)) { render_color = get_blink(color, effect); } else
  if(breathe_effects->is_breathe_effect(effect)) { render_color = get_breathe(color);       } else
                                                 { render_color = get_default(color);       }
  return render_color;
}

rgb_color Render::fast_render(rgb_color color, int _effect){
    return get_default(color);
}

void Render::render_buffer(rgb_color *dest_buffer, rgb_color *src_buffer, int *effects){
  for(int i = 0; i < ANIM_LED_COUNT; i++)
    dest_buffer[i] = render(src_buffer[i], effects[i]);
}

#endif

