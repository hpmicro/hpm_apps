#ifndef __MSC_APP_H
#define __MSC_APP_H

typedef int (*DataProcess)(unsigned char* data, int len);

extern DataProcess data_process;

#endif //__MSC_APP_H