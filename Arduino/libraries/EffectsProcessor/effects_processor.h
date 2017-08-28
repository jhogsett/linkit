#ifndef EFFECTS_PROCESSOR_H
#define EFFECTS_PROCESSOR_H

// built-in simple effects
#define STATIC_ON  10

#include <buffer.h>
#include <blink_effects.h>
#include <breathe_effects.h>
#include <fade_effects.h>

#define LEAVE_EFFECT -1
#define NO_EFFECT 0
#define RANDOM0 0
#define RANDOM1 1
#define RANDOM2 2
#define RANDOM_EFFECTS 11

class EffectsProcessor
{
  public:
  const static byte random_effects[RANDOM_EFFECTS];
  Buffer *buffer;
  BlinkEffects *blink_effects;
  BreatheEffects *breathe_effects;
  FadeEffects *fade_effects;

  static byte random_effect();

  void begin(Buffer *buffer, BlinkEffects *blink_effects, BreatheEffects *breathe_effects, FadeEffects * fade_effects);
  void start_effect(byte effect);
  void start_blinking_r();
  void start_effect_r();
  void reset_effects();
  bool process_effects();
};

// todo: move to progmem
const byte EffectsProcessor::random_effects[] = { NO_EFFECT, BREATHE_ON, BLINK_ON, BLINK_ON_1, BLINK_ON_2, BLINK_ON_3, BLINK_ON_4, BLINK_ON_5, BLINK_ON_6, BLINK_ON_A, BLINK_ON_B }; //, BLINK_ON_P };

void EffectsProcessor::begin(Buffer *buffer, BlinkEffects *blink_effects, BreatheEffects *breathe_effects, FadeEffects *fade_effects){
  this->buffer = buffer;
  this->blink_effects = blink_effects;
  this->breathe_effects = breathe_effects;
  this->fade_effects = fade_effects;
}

void EffectsProcessor::start_effect(byte effect) {
  buffer->get_effects_buffer()[buffer->get_offset()] = effect;
}

void EffectsProcessor::start_blinking_r() {
  buffer->get_effects_buffer()[buffer->get_offset()] = BLINK_ON_1 + random((BLINK_ON_6 - BLINK_ON_1) + 1);
}

byte EffectsProcessor::random_effect() {
  return random_effects[random(RANDOM_EFFECTS)];
}

void EffectsProcessor::start_effect_r() {
  buffer->get_effects_buffer()[buffer->get_offset()] = random_effect();
}

// reset the effects to reduce jarring when an update occurs
void EffectsProcessor::reset_effects() {
  blink_effects->reset();
  breathe_effects->reset();
  fade_effects->reset();
}

bool EffectsProcessor::process_effects() {
  bool should_flush = false;
  should_flush = blink_effects->process() ? true : should_flush;
  should_flush = breathe_effects->process() ? true : should_flush;
  should_flush = fade_effects->process() ? true : should_flush;
  return should_flush;

//  return blink_effects->process() || breathe_effects->process() || fade_effects->process();

  // twinkle is left out of deciding to flush
  // otherwise it would be rendering constantly
}

#endif
