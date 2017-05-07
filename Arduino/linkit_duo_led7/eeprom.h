#ifndef _EEPROM_H
#define _EEPROM_H

#include <avr/eeprom.h>

#define DEFAULT_ERASE_BYTE '\0'

void strcpy_memory_from_eeprom(char * dest, char * src){
  while(char c = eeprom_read_byte((const unsigned char*)src++))
    *dest++ = c;
  *dest = 0;
}

void strcpy_eeprom_from_memory(char * dest, char * src){
  while(char c = *src++)
    eeprom_write_byte((unsigned char*)dest++, c);
  eeprom_write_byte((unsigned char*)dest, 0);
}

// erase a range of eeprom memory
void erase_eeprom(char * pos, int count, byte fill = DEFAULT_ERASE_BYTE){
  for(int i = 0; i < count; i++)
    eeprom_write_byte((unsigned char*)pos++, fill);
}

#endif

