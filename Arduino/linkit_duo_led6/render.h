/*******************************************************************/

void render_buffer(){
  for(int i = 0; i < ANIM_LED_COUNT; i++){
    int effect = effects[i];

    if(effect ==  STATIC_ON) colors[i] = random_color();

    if(effect >= BLINK_MIN && effect <= BLINK_MAX){
      if((effect == BLINK_ON && blink_state) || 
            (effect == BLINK_ON_1 && blink_state_1) || 
            (effect == BLINK_ON_2 && blink_state_2) || 
            (effect == BLINK_ON_3 && blink_state_3) || 
            (effect == BLINK_ON_4 && blink_state_4) || 
            (effect == BLINK_ON_5 && blink_state_5) || 
            (effect == BLINK_ON_6 && blink_state_6) || 
            (effect == BLINK_ON_A && blink_state_a) || 
            (effect == BLINK_ON_B && blink_state_b)) {
          render[i] = scale_color(colors[i], DEFAULT_BRIGHTNESS_SCALE);  
        } else {
          render[i] = scale_color(colors[i], MINIMUM_BRIGHTNESS_SCALE);
        }
    }
    
    else if(effects[i] == BREATHE_ON) render[i] = scale_color(colors[i], DEFAULT_BRIGHTNESS_SCALE * breathe_steps[BREATHE_MAX_STEP - breathe_step]);

    else render[i] = scale_color(colors[i], DEFAULT_BRIGHTNESS_SCALE);
  }
}

