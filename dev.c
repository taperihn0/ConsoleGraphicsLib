#include "dev.h"
#include <ctype.h>

#define _INVALID_BUFFER_SYNTAX  -1
#define _DEVICE_INFO_INVALID    -1
#define _DEVICE_INFO_ID			 0
#define _DEVICE_INFO_NAME	 	 1
#define _DEVICE_INFO_PHYS 		 2
#define _DEVICE_INFO_HANDLER     3
#define _DEVICE_INFO_BITMAP_EV	 4
#define _DEVICE_INFO_BITMAP_KEYS 5
#define _DEVICE_INFO_REDUNDANT   32

typedef int _device_info_t;

_INLINE long _get_value_of(const char* line, const char* str, size_t n) {
	char* b = strstr(line, str) + strlen(str) + strlen("=");
	char* e = b;

	while ((isdigit(*e) || *e == ' ') && (e - line) < n)
		e++;

	if (e - line >= n)
		return _INVALID_BUFFER_SYNTAX;

	long id = strtol(b, &e, 16);

	return id;
}

_INLINE int _get_str_of(const char* line, const char* str, char** beg, size_t* strsize, size_t n) {
	char* b = strstr(line, str) + strlen(str) + strlen("=");
	
	if (*b == '\"') b++;

	char* e = b;

	while (*e != '\"' && *e != '\n' && (e - line) < n)
		e++;

	if (e - line >= n)
		return _INVALID_BUFFER_SYNTAX;
	
	*beg = b;
	*strsize = e - b;

	return 0;
}

_INLINE int _spec_dev_bitmap_info(const char* line, size_t n) {
	bool is_ev_info = (strstr(line, "EV") != NULL);
	bool is_key_info = (strstr(line, "KEY") != NULL);

	return is_ev_info  ? _DEVICE_INFO_BITMAP_EV :
		   is_key_info ? _DEVICE_INFO_BITMAP_KEYS :
		   			     _DEVICE_INFO_REDUNDANT;
}

_INLINE int _get_dev_info_from(const char* line, size_t n) {
	switch (line[0]) {
	case 'I': return _DEVICE_INFO_ID;
	case 'N': return _DEVICE_INFO_NAME;
	case 'P': return _DEVICE_INFO_PHYS;
	case 'H': return _DEVICE_INFO_HANDLER;
	case 'B': return _spec_dev_bitmap_info(line, n);
	default:  break;
	}

	return _DEVICE_INFO_REDUNDANT;
}

int _next_device(_devices_file* f, _simple_dev* dev) {
	size_t n = 1024;
	char* line = malloc(n * sizeof(char));

	int r;
	while ((r = getline(&line, &n, f)) != EOF && (r > 0 && line[0] != '\n')) {
		_device_info_t info = _get_dev_info_from(line, r);

		switch (info) {
		case _DEVICE_INFO_ID:
			dev->id_vendor  = _get_value_of(line, "Vendor", r);
			ASSERT(dev->id_vendor != (unsigned short)_INVALID_BUFFER_SYNTAX, "Invalid device file syntax");
			dev->id_product = _get_value_of(line, "Product", r);
			ASSERT(dev->id_product != (unsigned short)_INVALID_BUFFER_SYNTAX, "Invalid device file syntax");
			break;
		case _DEVICE_INFO_NAME:
			char* pn;
			size_t sn;
			int cn = _get_str_of(line, "Name", &pn, &sn, r);
			ASSERT(cn != _INVALID_BUFFER_SYNTAX && sn < _STR_DEV_NAME_SIZE_LIMIT, "Invalid device file syntax");

			memmove(dev->name, pn, sn);
			dev->name[sn] = '\0';
			break;
		case _DEVICE_INFO_PHYS:
			char* pp; 
			size_t sp;
			int cp = _get_str_of(line, "Phys", &pp, &sp, r);
			ASSERT(cp != _INVALID_BUFFER_SYNTAX, "Invalid device file syntax");

			dev->usb = (strstr(pp, "usb") != NULL);
			break;
		case _DEVICE_INFO_HANDLER:
			char* ev_occ = strstr(line, "event");
			ASSERT(ev_occ != NULL, "No proper eventX handler");
			size_t evs = 0;

			while (ev_occ[evs] != ' ' && ev_occ[evs] != '\n' && ev_occ[evs] != '\0') 
				evs++;

			ASSERT(evs < _STR_HANDLER_NAME_SIZE_LIMIT, "Too long handler name");
			memmove(dev->handler, ev_occ, evs);
			dev->handler[evs] = '\0';

			break;
		case _DEVICE_INFO_BITMAP_EV:
			dev->ev_types = _get_value_of(line, "EV", r);
			ASSERT(dev->ev_types != (unsigned short)_INVALID_BUFFER_SYNTAX, "Invalid device file syntax");
			break;
		case _DEVICE_INFO_BITMAP_KEYS:
			char* pk; 
			size_t sk;
			int ck = _get_str_of(line, "KEY", &pk, &sk, r); 
			ASSERT(ck != _INVALID_BUFFER_SYNTAX && sk < _STR_KEYS_SIZE_LIMIT, "Invalid device file syntax");
			
			size_t i = 0, j = 0;

			while (i < sk) {
				if (pk[i] != ' ') dev->keys[j++] = pk[i];
				i++;
			}

			dev->keys[j] = '\0';

			break;
		default: break;
		}
	}

	free(line);
	return r > 0;
}