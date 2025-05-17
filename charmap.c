#include "charmap.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define _CHAR_CNT 256

typedef struct _char_data {
	CHAR_T code;
	UINT brightness;
} _char_data;

typedef struct _char_tab {
	_char_data chars[_CHAR_CNT];
	size_t idx;
} _char_tab;

_char_tab _chtab = {
	.idx = 0
};

typedef struct _ft_state {
	FT_Library lib;
	FT_Face face;
} _ft_state;

_ft_state _ft;

_STATIC _FORCE_INLINE UINT _bitmap_brightness(FT_Bitmap* bitmap) {
	UINT sum = 0;
	for (UINT y = 0; y < bitmap->rows; y++) {
		for (UINT x = 0; x < bitmap->width; x++) {
			sum += (UINT)bitmap->buffer[y * bitmap->width + x];
		}
	}
	return sum;
}

_STATIC _FORCE_INLINE void _add(CHAR_T ch) {
	ASSERT(_chtab.idx < _CHAR_CNT, "Character table overflow while adding new element");
	
	// skip non-renderable characters
	if (FT_Load_Char(_ft.face, ch, FT_LOAD_RENDER))
		return;

	UINT brightness = _bitmap_brightness(&_ft.face->glyph->bitmap);

	_chtab.chars[_chtab.idx++] = (_char_data){
		.code = ch,
		.brightness = brightness
	};
}

_STATIC _FORCE_INLINE void _add_range(CHAR_T beg, CHAR_T end) {
	ASSERT(end >= beg, "Invalid character range to add");

	for (CHAR_T ch = beg; ch <= end; ch++) {
		_add(ch);
	}
}

int cmp_brightness(const void* a, const void* b) {
	return ((_char_data*)a)->brightness - ((_char_data*)b)->brightness;
}

#define _BITMAP_SIZE 8

int _init_char_map() {
	if (FT_Init_FreeType(&_ft.lib)) {
		fprintf(stderr, "Could not initialize FreeType\n");
		return 1;
	}

	if (FT_New_Face(_ft.lib, "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 0, &_ft.face)) {
		fprintf(stderr, "Could not load font\n");
		return 1;
	}

	FT_Set_Pixel_Sizes(_ft.face, 0, _BITMAP_SIZE);

	// add standart ASCII characters
	_add_range(' ', (CHAR_T)126);
	//_add_range((CHAR_T)0x2586, (CHAR_T)0x2589);
	//_add((CHAR_T)0x2588);
	//_add((CHAR_T)0x2588);
	//_add_range((CHAR_T)0x2591, (CHAR_T)0x2593);

	qsort(_chtab.chars, _chtab.idx, sizeof(_char_data), cmp_brightness);

	FT_Done_Face(_ft.face);
	FT_Done_FreeType(_ft.lib);

	return 0;
}

CHAR_T _char_by_brightness(float brightness) {
	ASSERT(_chtab.idx > 0, "Character map uninitialized");
	size_t entry = (size_t)roundl(brightness * (_chtab.idx - 1));
	ASSERT(entry <= _chtab.idx, "Invalid brightness");
	return _chtab.chars[entry].code;
}

#ifdef DEBUG
void print_char_map() {
	for (UINT i = 0; i < _chtab.idx; i++) {
		CHAR_T ch = _chtab.chars[i].code;
		CHAR_T chs[2] = { ch, '\0' };
		printf("%ls (U+%.4x)", chs, ch);
		putchar('\n');
	}
}
#endif