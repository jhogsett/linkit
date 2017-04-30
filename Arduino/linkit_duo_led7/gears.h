
// a gear is
// 
// a period (teeth)
// a state (at tooth zero)


class ByteGear
{
  byte teeth;
  byte tooth;

  void begin(byte teeth){
    this->teeth = teeth;
    this->tooth = 0;
  }

  byte turn(char steps = 1){
    return (tooth = (tooth + steps) % teeth);
  }

  bool at_top(){
    return tooth == 0 ? true : false
  }
};

class WordGear
{
  unsigned int teeth;
  unsigned int tooth;

  void begin(unsigned int teeth){
    this->teeth = teeth;
    this->tooth = 0;
  }

  unsigned int turn(int steps = 1){
    return (tooth = (tooth + steps) % teeth);
  }

  bool at_top(){
    return tooth == 0 ? true : false
  }
};


