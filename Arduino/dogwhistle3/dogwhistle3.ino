
#define RANDOM_SEED_SAMPLES 16
#define RANDOM_SEED_PIN 0

void setup() 
{
  int seed = analogRead(RANDOM_SEED_PIN);
  for(byte i = 0; i < RANDOM_SEED_SAMPLES; i++)
    seed = (seed << 1) ^ analogRead(0);
  randomSeed(seed);
}


void loop() 
{

  int t = random(3000);
  tone(8, t + 15000);
  int d = random(500);
  delay(d + 250);

}


