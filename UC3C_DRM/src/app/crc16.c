
#include "crc16.h"

 uint16_t get_crc16(uint8_t *array , uint16_t num , uint8_t *h, uint8_t *l)
 {
	 uint8_t  i, j ,n = 0;
	 uint8_t  CL= 0x01,CH = 0xA0;
	 uint8_t  CRC16LoVAL= 0xFF,CRC16HiVAL= 0xFF;
	 uint8_t  UseHiVAL,UseLoVAL;
	 uint16_t crcval;

	 for(i = 0; i < num; i++)
	 {
		 CRC16LoVAL = CRC16LoVAL ^ array[n+i] ;
		 for(j = 0; j < 8; j++)
		 {
			 UseHiVAL = CRC16HiVAL;
			 UseLoVAL = CRC16LoVAL;
			 CRC16HiVAL = CRC16HiVAL >> 1;
			 CRC16LoVAL = CRC16LoVAL >> 1;
			 if((UseHiVAL & 0x01) == 0x01)
			 CRC16LoVAL = CRC16LoVAL | 0x80;
			 if((UseLoVAL & 0x01) == 0x01)
			 {
				 CRC16HiVAL = CRC16HiVAL ^ CH;
				 CRC16LoVAL = CRC16LoVAL ^ CL;
			 }
		 }
	 }
	 crcval = *h = CRC16HiVAL;
	 *l = CRC16LoVAL;
	 crcval = (crcval<<8) + CRC16LoVAL;
	 return  (crcval);
 }