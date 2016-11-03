#define DATA_OUT_PIN 12               // data out pin for sending color data to the LEDs
#define RANDOM_SEED_PIN A1            // floating pin for seeding the RNG
#define LIGHT_SENSOR_PIN A0           // photocell pin for auto-brightness setting

#define DEFAULT_BRIGHTNESS_PERCENT 25 // standard brightness
#define MINIMUM_BRIGHTNESS_PERCENT 2  // brightness scale for blinking leds in the off state
#define FADE_RATE 0.9999

#define ANIM_LED_COUNT 64             // visible led count
#define EASE_ANIM_MARGIN 10           // safety margin for visual effects that go past the end of the LEDs
#define LED_COUNT (ANIM_LED_COUNT + EASE_ANIM_MARGIN)

