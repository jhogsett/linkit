/*******************************************************************/

#define LED_COUNT (ANIM_LED_COUNT + EASE_ANIM_MARGIN)

#define LEAVE_EFFECT -1
#define NO_EFFECT 0
#define BREATHE_ON 1
#define RANDOM 2

#define BLINK_ON 10

// blink group 1-6
#define BLINK_ON_1 11
#define BLINK_ON_2 12
#define BLINK_ON_3 13
#define BLINK_ON_4 14
#define BLINK_ON_5 15
#define BLINK_ON_6 16

// blink group a-b
#define BLINK_ON_A 21
#define BLINK_ON_B 22

#define BLINK_MIN BLINK_ON 
#define BLINK_MAX BLINK_ON_B

#define STATIC_ON 101

#define MAX_BLINK 6000
#define BLINK_0 0
#define BLINK_1 999
#define BLINK_2 1999
#define BLINK_3 2999
#define BLINK_4 3999
#define BLINK_5 4999
#define BLINK_6 5999
#define BLINK_A BLINK_1
#define BLINK_B BLINK_4
int blink_counter = 0;
bool blink_state = true; // false means blank

// blink groups
bool blink_state_1 = true; 
bool blink_state_2 = true; 
bool blink_state_3 = true; 
bool blink_state_4 = true; 
bool blink_state_5 = true; 
bool blink_state_6 = true; 
bool blink_state_a = true; 
bool blink_state_b = true; 

int effects[LED_COUNT];

#define RANDOM_EFFECTS 12
int random_effects[] = { NO_EFFECT, BREATHE_ON, RANDOM, BLINK_ON, BLINK_ON_1, BLINK_ON_2, BLINK_ON_3, BLINK_ON_4, BLINK_ON_5, BLINK_ON_6, BLINK_ON_A, BLINK_ON_B }; 

void start_effect(int effect){
  effects[0] = effect;
}

void start_blinking_r(){
  effects[0] = BLINK_ON_1 + random((BLINK_ON_6 - BLINK_ON_1) + 1);
}

void start_effect_r(){
  effects[0] = random_effects[random(RANDOM_EFFECTS)];
}

#define BREATHE_TIME 500
#define BREATHE_BRIGHTNESS_MAX DEFAULT_BRIGHTNESS_PERCENT
#define BREATHE_MAX_STEP 22 
#define BREATHE_BRIGHTNESS_MIN 0

int random_effect(){
  return random_effects[random(RANDOM_EFFECTS)];
}

int breathe_counter = 0;
int breathe_step = 0;
int breathe_direction = 1;

// ruby: (0..90).step(5).each { |i| puts (Math.cos(i * Math::PI / 180)).round(4) }
float breathe_steps[] = {
  1.0,
  0.9962,
  0.9848,
  0.9659,
  0.9397,
  0.9063,
  0.866,
  0.8192,
  0.766,
  0.7071,
  0.6428,
  0.5736,
  0.5,
  0.4226,
  0.342,
  0.2588,
  0.1736,
  0.0872,
  0.0,
  0.0,  // a few extra make it feel more natural
  0.0,
  0,0
};

void reset_effects(){
  // reset the effects so the automatic render won't interfere
  blink_state = true;
  blink_state_1 = false;
  blink_state_2 = false;
  blink_state_3 = false;
  blink_state_4 = false;
  blink_state_5 = false;
  blink_state_6 = false;
  blink_state_a = false;
  blink_state_b = false;
  blink_counter = 0;
  breathe_step = 0;
  breathe_counter = 0;
}

bool process_effects(){
  bool should_flush = false;
  
  blink_counter = (blink_counter + 1) % MAX_BLINK;
  if(blink_counter == BLINK_1){
    if(blink_state){
      blink_state = false;
    } else {
      blink_state = true;
    }
    should_flush = true;
  }

  if(blink_counter == BLINK_1){
    blink_state_6 = false;
    blink_state_1 = true;
    blink_state_a = true;
    blink_state_b = false;
    should_flush = true;
  }

  if(blink_counter == BLINK_2){
    blink_state_1 = false;
    blink_state_2 = true;
    should_flush = true;
  }

  if(blink_counter == BLINK_3){
    blink_state_2 = false;
    blink_state_3 = true;
    should_flush = true;
  }

  if(blink_counter == BLINK_4){
    blink_state_3 = false;
    blink_state_4 = true;
    blink_state_a = false;
    blink_state_b = true;
    should_flush = true;
  }

  if(blink_counter == BLINK_5){
    blink_state_4 = false;
    blink_state_5 = true;
    should_flush = true;
  }

  if(blink_counter == BLINK_6){
    blink_state_5 = false;
    blink_state_6 = true;
    should_flush = true;
  }

  breathe_counter = (breathe_counter + 1) % BREATHE_TIME;
  if(breathe_counter == 0){
    int next_breathe_step = breathe_step + breathe_direction;
    if(next_breathe_step < 0 || next_breathe_step >= BREATHE_MAX_STEP){
      breathe_direction *= -1;
    }
    breathe_step = breathe_step + breathe_direction;
    should_flush = true;
  }
  return should_flush;
}

