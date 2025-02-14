#pragma once

#include "common.h"
#include <linux/input-event-codes.h>

#define _STR_DEV_NAME_SIZE_LIMIT 	 64
#define _STR_HANDLER_NAME_SIZE_LIMIT 8
#define _STR_KEYS_SIZE_LIMIT 		 KEY_CNT / 4 + 1

typedef struct _dev_simple {
	unsigned short id_vendor;
	unsigned short id_product;
	char 		   name[_STR_DEV_NAME_SIZE_LIMIT];
	int 		   usb;
	char		   handler[_STR_HANDLER_NAME_SIZE_LIMIT];
	unsigned int   ev_types;
	char 		   keys[_STR_KEYS_SIZE_LIMIT];
} _dev_simple;

#define _DEVICES_FILEPATH "/proc/bus/input/devices"

typedef FILE _devices_file;

int _next_device(_devices_file* f, _dev_simple* dev);
int _is_keyboard_device(_dev_simple* dev);
int _is_mouse_device(_dev_simple* dev);