#ifndef ZONES_H
#define ZONES_H

// todo: rearrange zones:
// zone 0: always the whole width
// zones 1-last fine:the best zones for quick animation
// zones > last fine:additional special purpose zones
// change meaning of find zones to mean the the number of them, not the starting point
  
class Zones
{
  public:

  void begin(byte num_zones, byte num_fine_zones, const byte *zone_offsets, const byte *zone_windows);
  byte get_num_zones();
  byte get_fine_zones();
  byte zone_offset(byte zone);
  byte zone_window(byte zone);

  private:

  byte num_zones;
  byte fine_zones;
  const byte *zone_offsets;
  const byte *zone_windows;

};

void Zones::begin(byte num_zones, byte num_fine_zones, const byte *zone_offsets, const byte *zone_windows){
  this->num_zones = num_zones;
  this->fine_zones = fine_zones;
  this->zone_offsets = zone_offsets;
  this->zone_windows = zone_windows;
}

byte Zones::get_num_zones(){
  return this->num_zones;
}

byte Zones::get_fine_zones(){
  return this->fine_zones;
}

byte Zones::zone_offset(byte zone){
  zone = max(0, min(this->num_zones, zone));
  return pgm_read_byte(&zone_offsets[zone]);
}

byte Zones::zone_window(byte zone){
  zone = max(0, min(this->num_zones, zone));
  return pgm_read_byte(&zone_windows[zone]);
}

#endif
