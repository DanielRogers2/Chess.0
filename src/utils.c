#include "utils.h"

/*
 *all hail http://graphics.stanford.edu/~seander/bithacks.html
 */

uint8_t indexLSB(uint64_t value)
{
   uint8_t index = 64;
   //Isolate lowest set bit
   value &= (uint64_t) (-(int64_t) value);

   index = (value) ? index - 1 : index;
   //Binary search index
   index = (value & 0x00000000FFFFFFFF) ? index - 32 : index;
   index = (value & 0x0000FFFF0000FFFF) ? index - 16 : index;
   index = (value & 0x00FF00FF00FF00FF) ? index -  8 : index;
   index = (value & 0x0F0F0F0F0F0F0F0F) ? index -  4 : index;
   index = (value & 0x3333333333333333) ? index -  2 : index;
   index = (value & 0x5555555555555555) ? index -  1 : index;

   return index;
}

uint8_t indexMSB(uint64_t value)
{
   uint8_t index = 0;
   //There is probably a more efficient way to do this
   value |= value >> 32;
   value |= value >> 16;
   value |= value >>  8;
   value |= value >>  4;
   value |= value >>  2;
   value |= value >>  1;

   index = (value & 0xFFFFFFFF00000000) ? index | 32 : index;
   index = (value & 0xFFFF0000FFFF0000) ? index | 16 : index;
   index = (value & 0xFF00FF00FF00FF00) ? index |  8 : index;
   index = (value & 0x0F0F0F0F0F0F0F0F) ? index |  4 : index;
   index = (value & 0xCCCCCCCCCCCCCCCC) ? index |  2 : index;
   index = (value & 0xAAAAAAAAAAAAAAAA) ? index |  1 : index;

   return index;
}
