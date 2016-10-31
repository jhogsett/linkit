#ifndef EFFECTS_PROCESSOR_H
#define EFFECTS_PROCESSOR_H

#define LED_COUNT (ANIM_LED_COUNT + EASE_ANIM_MARGIN)
#define LEAVE_EFFECT -1
#define NO_EFFECT 0
#define RANDOM 2
#define STATIC_ON 101
#define RANDOM_EFFECTS 12

class EffectsProcessor
{
  public:
  static int random_effects[RANDOM_EFFECTS];
  int *effects;

  static int random_effect();

  void begin(int *effects);
  void start_effect(int effect);
  void start_blinking_r();
  void start_effect_r();
  void reset_effects();
  bool process_effects();
};


int EffectsProcessor::random_effects[] = { NO_EFFECT, BREATHE_ON, RANDOM, BLINK_ON, BLINK_ON_1, BLINK_ON_2, BLINK_ON_3, BLINK_ON_4, BLINK_ON_5, BLINK_ON_6, BLINK_ON_A, BLINK_ON_B };

void EffectsProcessor::begin(int *effects){
  this->effects = effects;
}

void EffectsProcessor::start_effect(int effect) {
  effects[0] = effect;
}

void EffectsProcessor::start_blinking_r() {
  effects[0] = BLINK_ON_1 + random((BLINK_ON_6 - BLINK_ON_1) + 1);
}

int EffectsProcessor::random_effect() {
  return random_effects[random(RANDOM_EFFECTS)];
}

void EffectsProcessor::start_effect_r() {
  effects[0] = random_effect();
}

// reset the effects to reduce jarring when an update occurs
void EffectsProcessor::reset_effects() {
  blink_effects.reset();
  breathe_effects.reset();
}

bool EffectsProcessor::process_effects() {
  bool should_flush = false;
  should_flush = blink_effects.process() ? true : should_flush;
  should_flush = breathe_effects.process() ? true : should_flush;
  return should_flush;
}

#endif

