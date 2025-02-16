#include "input.h"
#include "cursor.h"
#include "terminal.h"
#include "render.h"
#include "render_core.h"
#include "mem.h"
#include "timeman.h"

static vec3 cam_pos = {
	.x = 0.f, 
	.y = 0.f, 
	.z = 0.18f
};

static vec3 cam_dir = {
	.x = 0.f,
	.y = 0.f,
	.z = -1.f
};

static vec3 cam_right = {
	.x = 1.f,
	.y = 0.f,
	.z = 0.f
};

static vec3 cam_up = {
	.x = 0.f,
	.y = 1.f,
	.z = 0.f
};

#define PI 3.141592
#define RAD(deg) deg / 180. * PI

_FORCE_INLINE void normalize_3(vec3* v) {
	UINT norm = sqrt(dot3f(v, v));
	v->x /= norm;
	v->y /= norm;
	v->z /= norm;
}

void mouse_callback(int dx, int dy) {
	static float yaw = -90.f;
	static float pitch = 0.f;

	float xoffset = dx * 0.0001f;
	float yoffset = dy * 0.0001f;

	yaw += xoffset;
	pitch += yoffset;

	cam_dir.x = cos(RAD(yaw)) * cos(RAD(pitch));
	cam_dir.y = sin(RAD(pitch));
	cam_dir.z = sin(RAD(yaw)) * cos(RAD(pitch));

	cam_right = cross3f(&cam_dir, &cam_up);
	cam_up = cross3f(&cam_dir, &cam_right);

	normalize_3(&cam_dir);
	normalize_3(&cam_up);
	normalize_3(&cam_right);
}

void button_callback(unsigned short btn_action, int btn) {
	/*
	if (btn_action == KEY_PRESSED && btn == BTN_LEFT) {
		// do something
	}
	else if (btn_action == KEY_PRESSED && btn == BTN_RIGHT) {
		// do something
	}
	*/
}

void process_kbd_events(keyboard* kbd) {
	static utime_t prev_time = 0;
	utime_t curr_time = gettime_mls(CLOCK_MONOTONIC_RAW);
	utime_t delta_time = curr_time - prev_time;

	if (get_key(kbd, KEY_W) == KEY_PRESSED)
		cam_pos.z -= 0.00001 * delta_time;
	if (get_key(kbd, KEY_S) == KEY_PRESSED)
		cam_pos.z += 0.00001 * delta_time;
	if (get_key(kbd, KEY_A) == KEY_PRESSED)
		cam_pos.x -= 0.1 * delta_time;
	if (get_key(kbd, KEY_D) == KEY_PRESSED)
		cam_pos.x += 0.1 * delta_time;

	prev_time = curr_time;
}

int main() {
	init_terminal_state();
	enable_raw_mode();
	enable_focus_events();

	mouse* mice = malloc(sizeof(mouse));
	init_mouse(mice);
	set_pos_callback(mice, &mouse_callback);
	set_button_callback(mice, &button_callback);

	keyboard* kbd = malloc(sizeof(keyboard));
	init_keyboard(kbd);
	
	make_terminal_fullscreen();
	set_terminal_title("ASCIIGRAPHICS");
	disable_console_cursor();
	hide_cursor();

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
		process_kbd_events(kbd);
		
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

		mat4 v = mat4f(NULL);
			
		v.rc[0][0] = cam_right.x;
		v.rc[1][0] = cam_right.y;
		v.rc[2][0] = cam_right.z;
		v.rc[0][1] = cam_up.x;
		v.rc[1][1] = cam_up.y;
		v.rc[2][1] = cam_up.z;
		v.rc[0][2] = cam_dir.x;
		v.rc[1][2] = cam_dir.y;
		v.rc[2][2] = cam_dir.z;
		v.rc[0][3] = -cam_pos.x;
		v.rc[1][3] = -cam_pos.y;
		v.rc[2][3] = -cam_pos.z;
		v.rc[3][3] = 1;

		mat4 pv = mult_m4(&p, &v);
		mat4 pvm = mult_m4(&pv, &m);

		draw_buffer(id, &pvm);

		if (get_key(kbd, KEY_Q) == KEY_PRESSED)
			break;

	    flush_terminal();
	}
	
	delete_mem_buff(id);

	close_keyboard(kbd);
	close_mouse(mice);
	free(kbd);
	free(mice);
	
	unmake_terminal_fullscreen();
	disable_raw_mode();
	disable_focus_events();
	show_cursor();
	enable_console_cursor();

	close_terminal_state();

	fflush(stdout);
	return 0;
}
