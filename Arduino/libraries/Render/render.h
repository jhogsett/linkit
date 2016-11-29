#ifndef RENDER_H
#define RENDER_H

class Render
{
  public:
  void begin(BlinkEffects *blink_effects, BreatheEffects *breathe_effects, int default_brightness, int minimum_brightness);
  rgb_color render(rgb_color color, char effect);
  rgb_color fast_render(rgb_color color, char _effect);
  void render_buffer(rgb_color *dest_buffer, rgb_color *src_buffer, int count, char *effects);
  void render_buffer_low_power(rgb_color *dest_buffer, rgb_color *src_buffer, int count, char *effects, int position);
  void set_default_brightness(int brightness);
  void set_minimum_brightness(int brightness);

  private:
  static rgb_color black;
  BlinkEffects *blink_effects;
  BreatheEffects *breathe_effects;
  float default_brightness_scale;
  float minimum_brightness_scale;
  rgb_color get_blink(rgb_color color, char effect);
  rgb_color get_breathe(rgb_color color);
  rgb_color get_static();
  rgb_color get_default(rgb_color);
};

rgb_color Render::black = {0, 0, 0};

void Render::begin(BlinkEffects *blink_effects, BreatheEffects *breathe_effects, int default_brightness, int minimum_brightness){
    this->blink_effects = blink_effects;
    this->breathe_effects = breathe_effects;
    this->default_brightness_scale = default_brightness / 100.0;
    this->minimum_brightness_scale = minimum_brightness / 100.0;
}

rgb_color Render::get_blink(rgb_color color, char effect){
  rgb_color render_color;
  if(blink_effects->blink_on(effect))
  {
    render_color = ColorMath::scale_color(color, default_brightness_scale);
  }
  else
  {
    render_color = ColorMath::scale_color(color, minimum_brightness_scale);
  }
  return render_color;
}

rgb_color Render::get_breathe(rgb_color color){
  return ColorMath::scale_color(color, breathe_effects->breathe_ratio() * default_brightness_scale);
}

rgb_color Render::get_static(){
  return ColorMath::random_color();
}

rgb_color Render::get_default(rgb_color color){
  return ColorMath::scale_color(color, default_brightness_scale);
}

rgb_color Render::render(rgb_color color, char effect){
  rgb_color render_color;

                        if(effect ==  STATIC_ON) { render_color = get_static();             } else
      if(blink_effects->is_blink_effect(effect)) { render_color = get_blink(color, effect); } else
  if(breathe_effects->is_breathe_effect(effect)) { render_color = get_breathe(color);       } else
                                                 { render_color = get_default(color);       }
  return render_color;
}

rgb_color Render::fast_render(rgb_color color, char _effect){
    return get_default(color);
}

void Render::render_buffer(rgb_color *dest_buffer, rgb_color *src_buffer, int count, char *effects){
  for(int i = 0; i < count; i++)
    dest_buffer[i] = render(src_buffer[i], effects[i]);
}

void Render::render_buffer_low_power(rgb_color *dest_buffer, rgb_color *src_buffer, int count, char *effects, int position){
  for(int i = 0; i < count; i++){
    if(i == position){
      dest_buffer[i] = render(src_buffer[i], effects[i]);
    } else {
      dest_buffer[i] = black;
    }
  }
}

#define RESTRICT_TO_0_100(n) (max(0, min(100, n)))

void Render::set_default_brightness(int brightness){
  default_brightness_scale = RESTRICT_TO_0_100(brightness) / 100.0;
}

void Render::set_minimum_brightness(int brightness){
  minimum_brightness_scale = RESTRICT_TO_0_100(brightness) / 100.0;
}

#endif

