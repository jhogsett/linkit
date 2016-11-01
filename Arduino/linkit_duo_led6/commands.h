
#define MAX_BRIGHTNESS_PERCENT (DEFAULT_BRIGHTNESS_PERCENT * 4)
#define DIM_BRIGHTNESS_PERCENT (DEFAULT_BRIGHTNESS_PERCENT / 2)
#define BRIGHT_BRIGHTNESS_PERCENT (DEFAULT_BRIGHTNESS_PERCENT * 2)

extern rgb_color colors[LED_COUNT];
extern Buffer buffer;
extern int effects[LED_COUNT];
extern EffectsProcessor effects_processor;
extern rgb_color render[LED_COUNT];
extern Render renderer;

bool paused = false;

void do_blend(){
  colors[0] = ColorMath::blend_colors(colors[0], colors[1]);
  colors[1] = colors[0];
}

// only works properly when used immediately after placing a standard color
void do_max(){
  colors[0] = ColorMath::scale_color(colors[0], MAX_BRIGHTNESS_PERCENT / 100.0);
}

void do_dim(){
  colors[0].red = colors[0].red >> 1;
  colors[0].green = colors[0].green >> 1;
  colors[0].blue = colors[0].blue >> 1;
}

void do_bright(){
  colors[0].red = colors[0].red << 1;
  colors[0].green = colors[0].green << 1;
  colors[0].blue = colors[0].blue << 1;
}

void do_fade(float rate = FADE_RATE){
  for(int i = 0; i < FADE_TIMES; i++){
    fade(rate);
    buffer.display_buffer(colors);
    delay(FADE_DELAY);
  }
  buffer.erase();
}

void do_exhale_fade(){
  for(int i = 0; i < BREATHE_MAX_STEP; i++){
    exhale_fade(i);
    buffer.display_buffer(colors);
    delay(FADE_DELAY);
  }
  buffer.erase();
}

void do_flood(){
  for(int i = 1; i < ANIM_LED_COUNT; i++){
    if(effects[0] == RANDOM){
      colors[i] = ColorMath::random_color();
    } else {
      colors[i] = colors[0];
    }
    effects[i] = effects[0];    
  }
}

void do_random(){
  buffer.push_color(ColorMath::random_color());
  effects[0] = RANDOM;
}

void do_mirror(){
  for(int i = 0; i < ANIM_LED_COUNT / 2; i++){
    colors[(ANIM_LED_COUNT - 1) - i] = colors[i];
    effects[(ANIM_LED_COUNT - 1) - i] = effects[i];
  }
}

void do_repeat(int times = 1){
  times = (times < 1) ? 1 : times;
  for(int i = 0; i < times; i++){
    if(effects[0] == RANDOM){
      buffer.push_color(ColorMath::random_color());
    } else {
      buffer.push_color(colors[0]);
    }
    effects[0] = effects[1];
  }
}

void do_elastic_shift(int count, int max = ANIM_LED_COUNT){
  count = count == 0 ? 1 : count;
  if(count >= 1){
    for(int i = 0; i < ElasticEase::ease_count(); i++){
      int pos = ElasticEase::ease[i] * count;
      delay(ElasticEase::ease_delay());
      buffer.shift(pos+1, max);
    }
    buffer.finalize_shift(count, max);
  }
}

void do_power_shift(int count, int max = ANIM_LED_COUNT){
  count = count == 0 ? 1 : count;
  if(count >= 1){
    for(int i = 0; i < PowerEase::ease_count(); i++){
      int pos = PowerEase::ease[i] * count;
      delay(PowerEase::ease_delay());
      buffer.shift(pos+1, max);
    }
    buffer.finalize_shift(count, max);
  }
}

void do_power_shift_object(int width, int shift){
  do_power_shift(shift, shift + width);
}

void flush(){
  if(!paused){
    renderer.render_buffer(render, colors, ANIM_LED_COUNT, effects);
    buffer.display_buffer();
  }
}

void reset(){
  paused = false;
  buffer.set_window(0);
  effects_processor.reset_effects();
}

