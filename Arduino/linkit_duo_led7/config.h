#ifndef CONFIG_H
#define CONFIG_H

// enable to change settings for the 8-LED wearable version
// #define WEARABLE

#define NUM_DISPLAYS 3                // how many physical LED strips there are (all expected to have the same # of LEDs)

#define DISPLAY_PIN1 12
#define DISPLAY_PIN2 11
#define DISPLAY_PIN3 10

#define RANDOM_SEED_PIN A1            // floating pin for seeding the RNG
#define LIGHT_SENSOR_PIN A0           // photocell pin for auto-brightness setting

#ifdef WEARABLE
#define DEFAULT_BRIGHTNESS_PERCENT  6 // standard brightness
#else
#define DEFAULT_BRIGHTNESS_PERCENT 25 // standard brightness
#endif

#define MINIMUM_BRIGHTNESS_PERCENT 2  // brightness scale for blinking leds in the off state
#define FADE_RATE 0.9999

#ifdef WEARABLE
#define ANIM_LED_COUNT  8             // visible led count
#else
#define ANIM_LED_COUNT 72             // visible led count
#endif

#define EASE_ANIM_MARGIN 10           // safety margin for visual effects that go past the end of the LEDs

#define BAUD_RATE 115200              // speed for communicating with the MT7688

class Config
{
  public:
  static const int num_displays = NUM_DISPLAYS;
  static constexpr int display_pins[NUM_DISPLAYS] = {DISPLAY_PIN1, DISPLAY_PIN2, DISPLAY_PIN3};  
  static const int random_seed_pin = RANDOM_SEED_PIN;
  static const int light_sensor_pin = LIGHT_SENSOR_PIN;
  static const int default_brightness_percent = DEFAULT_BRIGHTNESS_PERCENT;
  static const int minimum_brightness_percent = MINIMUM_BRIGHTNESS_PERCENT;
  static constexpr float fade_rate = FADE_RATE;
  static const int visible_led_count = ANIM_LED_COUNT;
  static const int ease_anim_margin = EASE_ANIM_MARGIN;
  static const int led_count = visible_led_count + ease_anim_margin;
};

#endif
  
