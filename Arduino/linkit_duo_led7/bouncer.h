//
//class Bouncer
//{
//// How many parallel effects are running
//#define NUM_SETS 2
//
//// Bit flags for collision-detection
//unsigned int existence[NUM_SETS][LED_COUNT];
//unsigned int bitmask[16] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000};
//
//// Is this position occupied by another object?
//bool position_occupied(int pos, int set, int id){
//  return (existence[set][pos] & ~bitmask[id]) == 0 ? false : true;
//}
//
//// The heart of the bouncing ball algorithm
//float reflect_f(float *f, float *d, int set, int id, float mx = 1.0, float mn = 0.0){
//  float next = *f + *d;
//  int nexti = int(next);
//  bool clash =  position_occupied(nexti, set, id);
//  
//  if(next > mx || next < mn || clash){
//    *d *= -1.0;
//  }
//  *f += *d;
//
//  return clash;
//}
//
//// Set an object as occupying a position
//void occupy_position(int pos, int set, int id){
//  existence[set][pos] |= bitmask[id];
//}
//
//// Remove an object from a position
//void vacate_position(int pos, int set, int id){
//  existence[set][pos] &= ~bitmask[id];
//}
//
//// stray code to reset existence data
//  for(int j = 0; j < NUM_SETS; j++){
//    for(int i = 0; i < VLED_COUNT; i++){
//       existence[j][i] = 0;
//    }
//  }
//};

