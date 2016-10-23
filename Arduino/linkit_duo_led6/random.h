#define RANDOM_SEED_SAMPLES 16

template<unsigned char pin> class RandomSeed
{
  public:
  void randomize(void);
};

template<unsigned char pin> void RandomSeed<pin>::randomize(void){
  int seed = analogRead(pin);
  for(int i = 0; i < RANDOM_SEED_SAMPLES; i++){
    seed = (seed << 1) ^ analogRead(0);  
  }
  randomSeed(seed);
}

