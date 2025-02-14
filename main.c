#include "input.h"
#include "cursor.h"
#include "terminal.h"
#include "render.h"
#include "render_core.h"
#include "mem.h"
#include "timeman.h"
#include "dev.h"
//#include <libusb-1.0/libusb.h>

void mouse_callback(int dx, int dy) {
	/*printf("(%d, %d)\n", dx, dy);*/
}

void button_callback(unsigned short btn_action, int btn) {
	/*
	if (btn_action == KEY_PRESSED && btn == BTN_LEFT)
		printf("PRESSED LEFT MOUSE BUTTON\n");
	else if (btn_action == KEY_PRESSED && btn == BTN_RIGHT)
		printf("PRESSED RIGHT MOUSE BUTTON\n");
	*/
}

void print_dev(_dev_simple* dev) {
	printf("%x %x \n%s. \n%d \n%s. \n%x \n%s. \n", dev->id_vendor, dev->id_product, dev->name, dev->usb, dev->handler,
		dev->ev_types, dev->keys);
	fflush(stdout);
}

int main() {
	FILE* f = fopen(_DEVICES_FILEPATH, "r");
	_dev_simple d; 
	
	while (_next_device(f, &d)) {
		printf("DEVICE:\n");
		printf("Is mouse: %d, is keyboard: %d\n", _is_mouse_device(&d), _is_keyboard_device(&d));
		print_dev(&d);
		putchar('\n');
	}

	fclose(f);
	raise(SIGTERM);
	return 0;

	init_terminal_state();

	enable_raw_mode();
	enable_focus_events();
	make_terminal_fullscreen();
	set_terminal_title("ASCIIGRAPHICS");
	disable_console_cursor();
	hide_cursor();

	keyboard* kbd = malloc(sizeof(keyboard));
	init_keyboard(kbd);

	mouse* mice = malloc(sizeof(mouse));
	init_mouse(mice);
	set_pos_callback(mice, &mouse_callback);
	set_button_callback(mice, &button_callback);

	set_framerate_limit(100);

	float pos[] = {
		10.f, -10.f, -0.1f,
		10.f, 10.f, -0.1f,
		10.f, 10.f, -0.15f,
		10.f, -10.f, -0.1f,
		10.f, -10.f, -0.15f,
		10.f, 10.f, -0.15f,
	};

	buff_idx_t id;
	gen_mem_buff(pos, sizeof(pos), &id);

	float n = 0.1f;
	float r = get_terminal_width() / 2;
	float t = get_terminal_height() / 2;

	mat4 p = mat4f(NULL);

	p.rc[0][0] = n / r;
	p.rc[1][1] = n / t;
	p.rc[2][2] = -1;
	p.rc[2][3] = -2 * n;
	p.rc[3][2] = -1;

	float angle = 0.f;
	utime_t prev_time = gettime_mls(CLOCK_MONOTONIC_RAW);
	
	while (!should_quit()) {
		poll_events_keyboard(kbd);
		poll_events_mouse(mice);
		
		clear_terminal((CHAR_T)' ');

		mat4 m = mat4f(NULL);
		utime_t time = gettime_mls(CLOCK_MONOTONIC_RAW);
		utime_t delta_time = time - prev_time;
		prev_time = time;
		angle += 0.003f * delta_time;

		m.rc[0][0] =  cos(angle);
		m.rc[0][1] =  sin(angle);
		m.rc[1][0] = -sin(angle);
		m.rc[1][1] =  cos(angle);
		m.rc[2][2] = 1;
		m.rc[3][3] = 1;
		
		vec3 cam_pos = vec3f(0.f, 0.f, 0.06f);

		mat4 v = mat4f(NULL);
			
		v.rc[0][0] = 1;
		v.rc[1][1] = 1;
		v.rc[2][2] = -1;
		v.rc[0][3] = -cam_pos.x;
		v.rc[1][3] = -cam_pos.y;
		v.rc[2][3] = -cam_pos.z;
		v.rc[3][3] = 1;

		mat4 vm = mult_m4(&v, &m);
		mat4 pvm = mult_m4(&p, &vm);

		draw_buffer(id, &pvm);

		if (get_key(kbd, KEY_Q) == KEY_PRESSED)
			break;

	    flush_terminal();
	}
	
	delete_mem_buff(id);

	close_keyboard(kbd);
	free(kbd);
	free(mice);
	
	unmake_terminal_fullscreen();
	disable_raw_mode();
	disable_focus_events();
	show_cursor();
	enable_console_cursor();

	close_terminal_state();

	fflush(stdout);
}
