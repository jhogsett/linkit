#ifndef CONFIG_H
#define CONFIG_H

// enable to change settings for the 8-LED wearable version
// #define WEARABLE

// enable to change settings for the 93-LED disc
// #define DISC93

// enable to change settings for a single 50-LED strand
// #define STRAND1

// enable to change settings for two 50-LED strands
// #define STRAND2

// should have the auto-brightness range here
#define AUTO_BRIGHTNESS_MIN 3
#define AUTO_BRIGHTNESS_MAX 33

#define NUM_DISPLAYS 3                // how many physical LED strips there are (all expected to have the same # of LEDs)

#define DISPLAY_PIN1 12
#define DISPLAY_PIN2 11
#define DISPLAY_PIN3 10

#define RANDOM_SEED_PIN A1            // floating pin for seeding the RNG
#define LIGHT_SENSOR_PIN A0           // photocell pin for auto-brightness setting

// standard brightness
#if defined(WEARABLE)
#define DEFAULT_BRIGHTNESS_PERCENT 10
#elif defined(DISK93)
#define DEFAULT_BRIGHTNESS_PERCENT 20
#elif defined(STRAND1)
#define DEFAULT_BRIGHTNESS_PERCENT 25
#elif defined(STRAND2)
#define DEFAULT_BRIGHTNESS_PERCENT 25
#else
#define DEFAULT_BRIGHTNESS_PERCENT 25
#endif

#define MINIMUM_BRIGHTNESS_PERCENT 2  // brightness scale for blinking leds in the off state
#define FADE_RATE 0.9999

// visible led count
#if defined(WEARABLE)
#define ANIM_LED_COUNT  8             
#elif defined(DISC93)
#define ANIM_LED_COUNT 93
#elif defined(STRAND1)
#define ANIM_LED_COUNT 50
#elif defined(STRAND2)
#define ANIM_LED_COUNT 100
#else
#define ANIM_LED_COUNT 72
#endif

#if defined(WEARABLE)
#define DEMO_OBJECT_SIZE 1
#define DEMO_GAP_SIZE 0
#define DEMO_TOTAL_SIZE (DEMO_OBJECT_SIZE + DEMO_GAP_SIZE)
#define DEMO_DELAY 125
#elif defined(DISC93)
#define DEMO_OBJECT_SIZE 3
#define DEMO_GAP_SIZE 0
#define DEMO_TOTAL_SIZE (DEMO_OBJECT_SIZE + DEMO_GAP_SIZE)
#define DEMO_DELAY 35
#elif defined(STRAND2)
#define DEMO_OBJECT_SIZE 10
#define DEMO_GAP_SIZE 0
#define DEMO_TOTAL_SIZE (DEMO_OBJECT_SIZE + DEMO_GAP_SIZE)
#define DEMO_DELAY 35
#else
#define DEMO_OBJECT_SIZE 3
#define DEMO_GAP_SIZE 1
#define DEMO_TOTAL_SIZE (DEMO_OBJECT_SIZE + DEMO_GAP_SIZE)
#define DEMO_DELAY 0
#endif

#define EASE_ANIM_MARGIN 10           // safety margin for visual effects that go past the end of the LEDs

#define BAUD_RATE 115200              // speed for communicating with the MT7688

class Config
{
  public:
  static const byte num_displays = NUM_DISPLAYS;
  static constexpr byte display_pins[NUM_DISPLAYS] = {DISPLAY_PIN1, DISPLAY_PIN2, DISPLAY_PIN3};  
  static const byte random_seed_pin = RANDOM_SEED_PIN;
  static const byte light_sensor_pin = LIGHT_SENSOR_PIN;
  static const byte default_brightness_percent = DEFAULT_BRIGHTNESS_PERCENT;
  static const byte minimum_brightness_percent = MINIMUM_BRIGHTNESS_PERCENT;
  static constexpr float fade_rate = FADE_RATE;
  static const byte visible_led_count = ANIM_LED_COUNT;
  static const byte ease_anim_margin = EASE_ANIM_MARGIN;
  static const byte led_count = visible_led_count + ease_anim_margin;
};

#endif
  
