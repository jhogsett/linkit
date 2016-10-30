
#define MAX_BRIGHTNESS_PERCENT (DEFAULT_BRIGHTNESS_PERCENT * 4)
#define DIM_BRIGHTNESS_PERCENT (DEFAULT_BRIGHTNESS_PERCENT / 2)
#define BRIGHT_BRIGHTNESS_PERCENT (DEFAULT_BRIGHTNESS_PERCENT * 2)

bool paused = false;

void do_blend(){
  Buffer::colors[0] = ColorMath::blend_colors(Buffer::colors[0], Buffer::colors[1]);
  Buffer::colors[1] = Buffer::colors[0];
}

// only works properly when used immediately after placing a standard color
void do_max(){
  Buffer::colors[0] = ColorMath::scale_color(Buffer::colors[0], MAX_BRIGHTNESS_PERCENT / 100.0);
}

void do_dim(){
  Buffer::colors[0].red = Buffer::colors[0].red >> 1;
  Buffer::colors[0].green = Buffer::colors[0].green >> 1;
  Buffer::colors[0].blue = Buffer::colors[0].blue >> 1;
}

void do_bright(){
  Buffer::colors[0].red = Buffer::colors[0].red << 1;
  Buffer::colors[0].green = Buffer::colors[0].green << 1;
  Buffer::colors[0].blue = Buffer::colors[0].blue << 1;
}

void do_fade(float rate = FADE_RATE){
  for(int i = 0; i < FADE_TIMES; i++){
    fade(rate);
    Buffer::display_buffer(Buffer::colors);
    delay(FADE_DELAY);
  }
  Buffer::erase();
}

void do_exhale_fade(){
  for(int i = 0; i < BREATHE_MAX_STEP; i++){
    exhale_fade(i);
    Buffer::display_buffer(Buffer::colors);
    delay(FADE_DELAY);
  }
  Buffer::erase();
}

void do_flood(){
  for(int i = 1; i < ANIM_LED_COUNT; i++){
    if(effects[0] == RANDOM){
      Buffer::colors[i] = ColorMath::random_color();
    } else {
      Buffer::colors[i] = Buffer::colors[0];
    }
    effects[i] = effects[0];    
  }
}

void do_random(){
  Buffer::push_color(ColorMath::random_color());
  effects[0] = RANDOM;
}

void do_mirror(){
  for(int i = 0; i < ANIM_LED_COUNT / 2; i++){
    Buffer::colors[(ANIM_LED_COUNT - 1) - i] = Buffer::colors[i];
    effects[(ANIM_LED_COUNT - 1) - i] = effects[i];
  }
}

void do_repeat(int times = 1){
  times = (times < 1) ? 1 : times;
  for(int i = 0; i < times; i++){
    if(effects[0] == RANDOM){
      Buffer::push_color(ColorMath::random_color());
    } else {
      Buffer::push_color(Buffer::colors[0]);
    }
    effects[0] = effects[1];
  }
}

void do_elastic_shift(int count, int max = ANIM_LED_COUNT, bool display_only = false){
  count = count == 0 ? 1 : count;
  if(count >= 1){
    for(int i = 0; i < ElasticEase::ease_count(); i++){
      int pos = ElasticEase::ease[i] * count;
      delay(ElasticEase::ease_delay());
      Buffer::shift(pos+1, max);
    }
    Buffer::finalize_shift(count, max);
  }
}

void do_power_shift(int count, int max = ANIM_LED_COUNT, bool display_only = false){
  count = count == 0 ? 1 : count;
  if(count >= 1){
    for(int i = 0; i < PowerEase::ease_count(); i++){
      int pos = PowerEase::ease[i] * count;
      delay(PowerEase::ease_delay());
      Buffer::shift(pos+1, max);
    }
    Buffer::finalize_shift(count, max);
  }
}

void power_shift_object(int width, int shift){
  do_power_shift(shift, shift + width);
}

void flush(){
  if(!paused){
    render_buffer();
    Buffer::display_buffer();
  }
}

void reset(){
  paused = false;
  Buffer::set_window(0);
  reset_effects();
  breathe_direction = 1;
}

