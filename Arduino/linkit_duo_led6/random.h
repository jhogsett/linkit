// Compute a random seed by sampling an unconnected analog input pin 
int random_seed(){
  int seed = analogRead(RANDOM_SEED_PIN);
  for(int i = 0; i < 16; i++){
    seed = (seed << 1) ^ analogRead(0);  
  }
  randomSeed(seed);
}


