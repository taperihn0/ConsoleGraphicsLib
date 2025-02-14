#pragma once

#include "common.h"

#define _STR_DEV_NAME_SIZE_LIMIT 	 128
#define _STR_HANDLER_NAME_SIZE_LIMIT 8
#define _STR_KEYS_SIZE_LIMIT 		 128

typedef struct _simple_dev {
	unsigned short id_vendor;
	unsigned short id_product;
	char 		   name[_STR_DEV_NAME_SIZE_LIMIT];
	int 		   usb;
	char		   handler[_STR_HANDLER_NAME_SIZE_LIMIT];
	unsigned int   ev_types;
	char 		   keys[_STR_KEYS_SIZE_LIMIT];
} _simple_dev;

#define _DEVICES_FILEPATH "/proc/bus/input/devices"

typedef FILE _devices_file;

int _next_device(_devices_file* f, _simple_dev* dev);