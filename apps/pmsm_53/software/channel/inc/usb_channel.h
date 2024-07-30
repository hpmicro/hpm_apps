#ifndef __USB_CHANNEL_H
#define __USB_CHANNEL_H

#define USB_POLLING_PROCESS

void hpm_usb_device_channel_init(void);

void hpm_usb_device_channel_handle(void);

void hpm_usb_device_block_task(void* proc);

#endif //__USB_CHANNEL_H