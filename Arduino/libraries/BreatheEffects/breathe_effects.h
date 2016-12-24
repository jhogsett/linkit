#ifndef BREATHE_EFFECTS_H
#define BREATHE_EFFECTS_H

#define USE_PROGMEM

#define BREATHE_ON 30

#define BREATHE_TIME 500
#define BREATHE_BRIGHTNESS_MAX DEFAULT_BRIGHTNESS_PERCENT
#define BREATHE_MAX_STEP 22 
#define BREATHE_BRIGHTNESS_MIN 0

class BreatheEffects
{
  public:
  byte breathe_step = 0;
#ifdef USE_PROGMEM
  static const float PROGMEM breathe_steps[];
#else
  static float breathe_steps[];
#endif

  void begin(int breathe_time);
  void reset();
  bool process();
  bool is_handled_effect(byte effect);
  float breathe_ratio();
 
  private:
  int breathe_time = BREATHE_TIME;               // 2
  int breathe_counter = 0;                       // 2
  char breathe_direction = 1;                    // 1
};

// ruby: (0..90).step(5).each { |i| puts (Math.cos(i * Math::PI / 180)).round(15) }
#ifdef USE_PROGMEM
const float PROGMEM BreatheEffects::breathe_steps[]
#else
float BreatheEffects::breathe_steps[]
#endif
= {        // 88
    1.0,
    0.996194698091746,
    0.984807753012208,
    0.965925826289068,
    0.939692620785908,
    0.90630778703665,
    0.866025403784439,
    0.819152044288992,
    0.766044443118978,
    0.707106781186548,
    0.642787609686539,
    0.573576436351046,
    0.5,
    0.422618261740699,
    0.342020143325669,
    0.258819045102521,
    0.17364817766693,
    0.087155742747658,
    0.0,
    0.0,  // a few extra make it feel more natural
    0.0,
    0,0
  };

void BreatheEffects::begin(int breathe_time = BREATHE_TIME){
  this->breathe_time = breathe_time;
}

void BreatheEffects::reset(){
  breathe_step = 0;
  breathe_counter = 0;
  breathe_direction = 1;
}

bool BreatheEffects::process(){
  bool should_flush = false;

  breathe_counter = (breathe_counter + 1) % breathe_time;
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

bool BreatheEffects::is_handled_effect(byte effect){
  return effect == BREATHE_ON;
}

float BreatheEffects::breathe_ratio(){
#ifdef USE_PROGMEM
  return pgm_read_float(&breathe_steps[BREATHE_MAX_STEP - breathe_step]);
#else
  return breathe_steps[BREATHE_MAX_STEP - breathe_step];
#endif
}

#endif

