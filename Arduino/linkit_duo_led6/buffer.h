#ifndef BUFFER_H
#define BUFFER_H

extern PololuLedStrip<DATA_OUT_PIN> ledStrip;

#define DEFAULT_BRIGHTNESS_SCALE (DEFAULT_BRIGHTNESS_PERCENT / 100.0)
#define MINIMUM_BRIGHTNESS_SCALE (MINIMUM_BRIGHTNESS_PERCENT / 100.0)

class Buffer
{
  public:
  static rgb_color colors[LED_COUNT];
  static rgb_color render[LED_COUNT];

  static void display_buffer(rgb_color * pbuffer);
  static void erase(bool display);
  static void push_color(rgb_color color, bool display, int effect, rgb_color *buffer, int max);
  static void push_rgb_color(int red, int green, int blue);
  static void push_hsl_color(int hue, int sat, int lit);
  static void shift(int count, int maxx);
  static void finalize_shift(int count, int max);
  static void set_color(int pos, rgb_color color, bool display, int effect, rgb_color *buffer);
  static void set_window(int width);

  private:
  static int window;
  static void shift_buffer(rgb_color * buffer, bool include_effects, int max);
  static void copy_buffer(rgb_color *source, rgb_color *destination, int count);
};

  rgb_color Buffer::colors[LED_COUNT];
  rgb_color Buffer::render[LED_COUNT];
  int Buffer::window = 0;  
  
  void Buffer::display_buffer(rgb_color * pbuffer = render){
    ledStrip.write(pbuffer, ANIM_LED_COUNT);  
  }
  
  void Buffer::erase(bool display = false){
    for(int i = 0; i < ANIM_LED_COUNT; i++){
      colors[i] = black;
      effects[i] = 0;
    }
    if(display){
      display_buffer(colors);
    }
  }
  
  void Buffer::shift_buffer(rgb_color * buffer, bool include_effects = true, int max = ANIM_LED_COUNT){
    for(int i = max - 1; i >= 1; i--){
      buffer[i] = buffer[i-1];
      effects[i] = effects[i-1];
    }
  }
  
  void Buffer::copy_buffer(rgb_color *source, rgb_color *destination, int count = ANIM_LED_COUNT){
    for(int i = 0; i < ANIM_LED_COUNT; i++){
      destination[i] = source[i];    
    }
  }
  
  void Buffer::push_color(rgb_color color, bool display = false, int effect = NO_EFFECT, rgb_color *buffer = colors, int max = window){
    if(max == 0){
      max = ANIM_LED_COUNT;
    }
    shift_buffer(buffer, true, max);
    
    buffer[0] = color;
    effects[0] = effect;
    
    if(display){
      display_buffer(buffer);
    }
  }
  
  void Buffer::push_rgb_color(int red, int green, int blue){
    rgb_color color = (rgb_color){red, green, blue}; 
    color = ColorMath::unscale_color(color, DEFAULT_BRIGHTNESS_SCALE);
    push_color(color);
  }
  
  void Buffer::push_hsl_color(int hue, int sat, int lit){
    rgb_color color = ColorMath::hsl_to_rgb(hue, sat, lit);
    push_rgb_color(color.red, color.green, color.blue);
  }
  
  void Buffer::set_color(int pos, rgb_color color, bool display = false, int effect = NO_EFFECT, rgb_color *buffer = colors){
    buffer[pos] = color;
  
    if(effect != LEAVE_EFFECT){
      effects[pos] = effect;
    }
    
    if(display){
      display_buffer(buffer);
    }
  }
  
  void Buffer::set_window(int width){
    if(width < 0){
      width = 0;
    }
    window = width;
  }
  
  // animate by shifting frame (future: shift in from back buffer)
  void Buffer::shift(int count, int maxx = ANIM_LED_COUNT){
    maxx = min(maxx, LED_COUNT);
    for(int i = 0; i < count; i++){
      render[i] = black;
    }
    for(int i = count; i < maxx; i++){
      render[i] = ColorMath::scale_color(colors[i - count], DEFAULT_BRIGHTNESS_SCALE);
    }
  
    display_buffer();
  }
  
  void Buffer::finalize_shift(int count, int max){
    for(int i = 0; i < count; i++){
      push_color(black, false, NO_EFFECT, colors, max);
    }
  }


#endif

