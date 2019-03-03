#ifndef BREATHE_EFFECTS_H
#define BREATHE_EFFECTS_H

#define BREATHE_ON   21
#define BREATHE_ON_D 22

#define BREATHE_MIN BREATHE_ON
#define BREATHE_MAX BREATHE_ON_D

#define BREATHE_TIME 500
#define BREATHE_BRIGHTNESS_MAX DEFAULT_BRIGHTNESS_PERCENT
#define BREATHE_MAX_STEP 22
#define BREATHE_BRIGHTNESS_MIN 0

#define BYTE_MAP

class BreatheEffects
{
  public:

  byte breathe_step = 0;

#ifndef BYTE_MAP
  static const float PROGMEM breathe_steps[];
#else
  static const byte PROGMEM breathe_steps[];
#endif

  void begin(int breathe_time);
  void reset();
  bool process();
  bool is_handled_effect(byte effect);
  float breathe_ratio();
  void set_breathe_time(int time);
  rgb_color breathe_crossfade(rgb_color color1, rgb_color color2);
  bool alt_breathe() { return this->breathe_alt; }

  private:

  int breathe_time = BREATHE_TIME;
  int breathe_counter = 0;
  char breathe_direction = 1;
  bool breathe_alt = false;
};

#ifndef BYTE_MAP
// ruby: (0..90).step(5).each { |i| puts (Math.cos(i * Math::PI / 180)).round(15) }
const float PROGMEM BreatheEffects::breathe_steps[]
= {
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
#else
// (0..90).step(5).each { |i| puts ((Math.cos(i * Math::PI / 180)) * 255).round }
const byte PROGMEM BreatheEffects::breathe_steps[]
= {
    255,
    254,
    251,
    246,
    240,
    231,
    221,
    209,
    195,
    180,
    164,
    146,
    128,
    108,
    87,
    66,
    44,
    22,
    0,
    0,
    0,
    0
  };
#endif

void BreatheEffects::begin(int breathe_time = BREATHE_TIME)
{
  this->breathe_time = breathe_time;
  reset();
}

void BreatheEffects::reset()
{
  breathe_step = 0;
  breathe_counter = 0;
  breathe_direction = 1;
}

bool BreatheEffects::process()
{
  bool should_flush = false;
  breathe_counter = (breathe_counter + 1) % breathe_time;

  if(breathe_counter == 0)
  {
    byte next_breathe_step = breathe_step + breathe_direction;
    if(next_breathe_step >= BREATHE_MAX_STEP)
    {
      breathe_direction *= -1;
      if(breathe_direction > 0)
        this->breathe_alt = !this->breathe_alt;
    }

    breathe_step = breathe_step + breathe_direction;
    should_flush = true;
  }
  return should_flush;
}

bool BreatheEffects::is_handled_effect(byte effect)
{
  return effect >= BREATHE_MIN && effect <= BREATHE_MAX;
}

float BreatheEffects::breathe_ratio()
{
#ifndef BYTE_MAP
  return pgm_read_float(&breathe_steps[(BREATHE_MAX_STEP - 1) - breathe_step]);
#else
  return pgm_read_byte(&breathe_steps[(BREATHE_MAX_STEP - 1) - breathe_step]) / 255.0;
#endif
}

rgb_color BreatheEffects::breathe_crossfade(rgb_color color1, rgb_color color2)
{
  rgb_color result;

  // crossfade is 20 steps
  // breathe is 22 steps
  // use the two end steps to repeat the adjacent ones to give the native colors a bit of hang time,
  //   and to fix the mismatch
  byte crossfade_step = max(0, min(ColorMath::crossfade_steps(), this->breathe_step - 1));
  result.red = ColorMath::crossfade_component_raw(crossfade_step, color1.red, color2.red);
  result.green = ColorMath::crossfade_component_raw(crossfade_step, color1.green, color2.green);
  result.blue = ColorMath::crossfade_component_raw(crossfade_step, color1.blue, color2.blue);
  return result;
}

void BreatheEffects::set_breathe_time(int time)
{
  this->breathe_time = time;
}
#endif
