#ifndef __MSC_RAM_H
#define __MSC_RAM_H

typedef int (*DataProcess)(unsigned int addr, unsigned char* data, int len);

extern DataProcess data_process;

void msc_ram_init(uint8_t busid, uint32_t reg_base);

#endif //__MSC_RAM_H