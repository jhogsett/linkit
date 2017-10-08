#ifndef MAP_DEFS_H
#define MAP_DEFS_H

#ifdef RADIUS8_MAPPING

#define NUM_ROWS 16
#define NUM_COLS 16
#define X 255

const byte map_row_01[] PROGMEM = {   X,   X,   X,   X,   X,   0,   1,   2,   3,   4,   5,   X,   X,   X,   X,   X,  };
const byte map_row_02[] PROGMEM = {   X,   X,   X,   X,  13,  12,  11,  10,   9,   8,   7,   6,   X,   X,   X,   X,  };
const byte map_row_03[] PROGMEM = {   X,   X,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,   X,   X,  };
const byte map_row_04[] PROGMEM = {   X,   X,  37,  36,  35,  34,  33,  32,  31,  30,  29,  28,  27,  26,   X,   X,  };
const byte map_row_05[] PROGMEM = {   X,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,   X,  };
const byte map_row_06[] PROGMEM = {  67,  66,  65,  64,  63,  62,  61,  60,  59,  58,  57,  56,  55,  54,  53,  52,  };
const byte map_row_07[] PROGMEM = {  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  };
const byte map_row_08[] PROGMEM = {  99,  98,  97,  96,  95,  94,  93,  92,  91,  90,  89,  88,  87,  86,  85,  84,  };
const byte map_row_09[] PROGMEM = { 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115,  };
const byte map_row_10[] PROGMEM = { 131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116,  };
const byte map_row_11[] PROGMEM = { 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147,  };
const byte map_row_12[] PROGMEM = {   X, 161, 160, 159, 158, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148,   X,  };
const byte map_row_13[] PROGMEM = {   X,   X, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173,   X,   X,  };
const byte map_row_14[] PROGMEM = {   X,   X, 185, 184, 183, 182, 181, 180, 179, 178, 177, 176, 175, 174,   X,   X,  };
const byte map_row_15[] PROGMEM = {   X,   X,   X,   X, 186, 187, 188, 189, 190, 191, 192, 193,   X,   X,   X,   X,  };
const byte map_row_16[] PROGMEM = {   X,   X,   X,   X,   X, 199, 198, 197, 196, 195, 194,   X,   X,   X,   X,   X,  };

const byte* const map_rows[] PROGMEM = {
  map_row_01, map_row_02, map_row_03, map_row_04, map_row_05, map_row_06, map_row_07, map_row_08,  
  map_row_09, map_row_10, map_row_11, map_row_12, map_row_13, map_row_14, map_row_15, map_row_16
};
#endif

#ifdef USE_MAPPING
class Maps
{
  public:

  static const byte INVALID_POS = X; 

  void begin(const byte* const *map_defs);
  byte get_led(byte col, byte row);

  private:
  
  byte* get_row_P(byte row);
  byte get_col(byte col, byte* row_P);

  const byte* const *map_defs;
};

void Maps::begin(const byte* const *map_defs){
  this->map_defs = map_defs;
}

byte Maps::get_led(byte col, byte row){
  return get_col(col, get_row_P(row));  
}

byte* Maps::get_row_P(byte row){
  return (byte*)pgm_read_word(&(this->map_defs[row]));
}

byte Maps::get_col(byte col, byte* row_P){
  return pgm_read_byte(&row_P[col]);
}

#endif
#endif

