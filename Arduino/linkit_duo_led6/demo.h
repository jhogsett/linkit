/*******************************************************************/

#define DEMO_OBJECT_SIZE 3
#define DEMO_GAP_SIZE 1
#define DEMO_TOTAL_SIZE (DEMO_OBJECT_SIZE + DEMO_GAP_SIZE)

void do_demo(int count = ANIM_LED_COUNT / DEMO_TOTAL_SIZE){

  // set some pattern of colors and effects
  // set window including last place it should animate to
  // animate N positions
  // during animation, render 

  if(count < 1) count = 1;
  int window = ANIM_LED_COUNT;
  
  for(int i = 0; i < count; i++){
    rgb_color color = random_color();
 
//    do_power_shift(window, window + DEMO_TOTAL_SIZE);
//    //do_elastic_shift(window, window + DEMO_TOTAL_SIZE);

    power_shift_object(DEMO_TOTAL_SIZE, window);
    
    window -= DEMO_TOTAL_SIZE;

    int effect = random_effect();

    for(int j = DEMO_GAP_SIZE; j < DEMO_TOTAL_SIZE; j++){
      set_color(j, color, false, effect);
    }
  }
}

