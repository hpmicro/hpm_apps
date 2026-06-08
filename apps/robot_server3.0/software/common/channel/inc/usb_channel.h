#ifndef __USB_CHANNEL_H
#define __USB_CHANNEL_H

void hpm_usb_device_channel_init(void);

void hpm_usb_device_block_task(void* proc);

void hpm_usb_host_channel_init(void);

void hpm_usb_host_block_task(void* proc);

#endif //__USB_CHANNEL_H