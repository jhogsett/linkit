void do_blend(){
  colors[0] = blend_colors(colors[0], colors[1]);
  colors[1] = colors[0];
}

// only works properly when used immediately after placing a standard color
void do_max(){
  colors[0] = scale_color(colors[0], MAX_BRIGHTNESS_PERCENT / 100.0);
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
    //ledStrip.write(colors, ANIM_LED_COUNT);
    display_buffer(colors);
    delay(FADE_DELAY);
  }
  erase();
}

void do_exhale_fade(){
  for(int i = 0; i < BREATHE_MAX_STEP; i++){
    exhale_fade(i);
    display_buffer(colors);
    delay(FADE_DELAY);
  }
  erase();
}

void do_flood(){
  for(int i = 1; i < ANIM_LED_COUNT; i++){
    if(effects[0] == RANDOM){
      colors[i] = random_color();
    } else {
      colors[i] = colors[0];
    }
    effects[i] = effects[0];    
  }
}

void do_random(){
  push_color(random_color());
  effects[0] = RANDOM;
}

void do_mirror(){
  for(int i = 0; i < ANIM_LED_COUNT / 2; i++){
    colors[(ANIM_LED_COUNT - 1) - i] = colors[i];
    effects[(ANIM_LED_COUNT - 1) - i] = effects[i];
  }
}

void do_repeat(int times = 1){
  for(int i = 0; i < times; i++){
    if(effects[0] == RANDOM){
      push_color(random_color());
    } else {
      push_color(colors[0]);
    }
    effects[0] = effects[1];
  }
}

void do_elastic_shift(int count, int max = ANIM_LED_COUNT, bool display_only = false){
  count = count == 0 ? 1 : count;
  if(count >= 1){
    for(int i = 0; i < EASE_COUNT; i++){
      int pos = elastic_ease[i] * count;
      delay(EASE_DELAY);
      shift(pos+1, max);
    }
    finalize_shift(count, max);
  }
}

void do_power_shift(int count, int max = ANIM_LED_COUNT, bool display_only = false){
  count = count == 0 ? 1 : count;
  if(count >= 1){
    for(int i = 0; i < POWER_EASE_COUNT; i++){
      int pos = power_ease[i] * count;
      delay(POWER_EASE_DELAY);
      shift(pos+1, max);
    }
    finalize_shift(count, max);
  }
}

void power_shift_object(int width, int shift){
  do_power_shift(shift, shift + width);
}


