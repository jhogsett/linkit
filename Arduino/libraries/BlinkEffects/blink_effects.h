#ifndef BLINK_EFFECTS_H
#define BLINK_EFFECTS_H

// main blink
#define BLINK_ON 11

// blink group 1-6 (1/6)
#define BLINK_ON_1 12
#define BLINK_ON_2 13
#define BLINK_ON_3 14
#define BLINK_ON_4 15
#define BLINK_ON_5 16
#define BLINK_ON_6 17

// blink group a-b (1/2)
#define BLINK_ON_A 18
#define BLINK_ON_B 19
#define BLINK_ON_D 20 // dynamic blink, a=primary color, b=secondary color

#define BLINK_MIN BLINK_ON
#define BLINK_MAX BLINK_ON_D

#define MAX_BLINK_SEGMENTS 6
#define DEFAULT_BLINK_PERIOD 1500

#define BLINK_CACHE 10

class BlinkEffects
{
  public:

  void begin(int blink_period);
  void reset();
  bool process();
  static bool is_handled_effect(byte effect);
  void set_blink_period(int blink_period);
  bool blink_on(byte effect);

  private:

  void rebuild_cache();
  bool blink_test();
  bool blink_1_6_test(byte effect);
  bool blink_a_test();

  int fast_counter = 0;
  int fast_period;
  int slow_counter = 0;
  int slow_period;

  static bool blink_cache[BLINK_CACHE];
};

bool BlinkEffects::blink_cache[BLINK_CACHE];

void BlinkEffects::begin(int blink_period = DEFAULT_BLINK_PERIOD)
{
  set_blink_period(blink_period);
  reset();
}

void BlinkEffects::reset()
{
  this->fast_counter = 0;
  this->slow_counter = 0;
  rebuild_cache();
}

void BlinkEffects::set_blink_period(int blink_period)
{
  this->fast_period = blink_period;
  this->slow_period = blink_period * 2;
}

bool BlinkEffects::process()
{
  bool flush = false;
  this->fast_counter = (this->fast_counter + 1) % this->fast_period;
  this->slow_counter = (this->slow_counter + 1) % this->slow_period;

  int half_fast = this->fast_period / 2;
  int sixth_fast = this->fast_period / MAX_BLINK_SEGMENTS;
  if(this->fast_counter % half_fast == 0 || this->fast_counter % sixth_fast == 0)
  {
    rebuild_cache();
    flush = true;
  }

  return flush;
}

bool BlinkEffects::is_handled_effect(byte effect)
{
  return effect >= BLINK_MIN && effect <= BLINK_MAX;
}

bool BlinkEffects::blink_test()
{
  int half_slow = this->slow_period / 2;
  return this->slow_counter < half_slow;
}

bool BlinkEffects::blink_1_6_test(byte effect)
{
  int sixth_fast = this->fast_period / MAX_BLINK_SEGMENTS;
  int start = (effect - BLINK_ON_1) * sixth_fast;
  int end = start + sixth_fast;
  return this->fast_counter >= start && this->fast_counter < end;
}

bool BlinkEffects::blink_a_test()
{
  int half_fast = this->fast_period / 2;
  return this->fast_counter < half_fast;
}

void BlinkEffects::rebuild_cache()
{
  this->blink_cache[0] = blink_test();

  for(byte i = 1; i <= MAX_BLINK_SEGMENTS; i++)
    this->blink_cache[i] = blink_1_6_test(BLINK_MIN + i);

  bool blink_a = blink_a_test();
  this->blink_cache[7] = blink_a;
  this->blink_cache[8] = !blink_a;

  this->blink_cache[9] = blink_a;
}

bool BlinkEffects::blink_on(byte effect)
{
  return this->blink_cache[effect - BLINK_MIN];
}
#endif
