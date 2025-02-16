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
	.z = 20.f,
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

void mouse_callback(int dx, int dy) {
	static float yaw = -90.f;
	static float pitch = 0.f;

	float xoffset = dx * 0.1f;
	float yoffset = dy * 0.1f;

	yaw += xoffset;
	pitch -= yoffset;

	cam_dir.x = cos(RADIANS(yaw)) * cos(RADIANS(pitch));
	cam_dir.y = sin(RADIANS(pitch));
	cam_dir.z = sin(RADIANS(yaw)) * cos(RADIANS(pitch));

	cam_right = cross3f(&cam_dir, &cam_up);
	cam_up = cross3f(&cam_right, &cam_dir);
	
	normalize3f(&cam_dir);
	normalize3f(&cam_right);
	normalize3f(&cam_up);
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

	if (get_key(kbd, KEY_W) == KEY_PRESSED) {
		cam_pos.x += 0.01 * delta_time * cam_dir.x;
		cam_pos.y += 0.01 * delta_time * cam_dir.y;
		cam_pos.z += 0.01 * delta_time * cam_dir.z;
	}
	if (get_key(kbd, KEY_S) == KEY_PRESSED) {
		cam_pos.x -= 0.01 * delta_time * cam_dir.x;
		cam_pos.y -= 0.01 * delta_time * cam_dir.y;
		cam_pos.z -= 0.01 * delta_time * cam_dir.z;
	}
	if (get_key(kbd, KEY_A) == KEY_PRESSED) {
		cam_pos.x -= 0.01 * delta_time * cam_right.x;
		cam_pos.y -= 0.01 * delta_time * cam_right.y;
		cam_pos.z -= 0.01 * delta_time * cam_right.z;
	}
	if (get_key(kbd, KEY_D) == KEY_PRESSED) {
		cam_pos.x += 0.01 * delta_time * cam_right.x;
		cam_pos.y += 0.01 * delta_time * cam_right.y;
		cam_pos.z += 0.01 * delta_time * cam_right.z;
	}

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
		10.f, -10.f, 0.f,
		10.f, 10.f, 0.f,
		10.f, 10.f, -10.f,
		10.f, -10.f, 0.f,
		10.f, -10.f, -10.f,
		10.f, 10.f, -10.f,
	};

	buff_idx_t id;
	gen_mem_buff(pos, sizeof(pos), &id);

	float n = 1.f;
	float f = 100.f;
	float r = 1.f;
	float t = 1.f;

	mat4 proj = mat4f(NULL);

	proj.rc[0][0] = n / r;
	proj.rc[1][1] = n / t;
	proj.rc[2][2] = -(f + n) / (n - f);
	proj.rc[2][3] = 2 * f * n / (n - f);
	proj.rc[3][2] = 1;

	float angle = 0.f;
	utime_t prev_time = gettime_mls(CLOCK_MONOTONIC_RAW);
	
	while (!should_quit()) {
		poll_events_keyboard(kbd);
		poll_events_mouse(mice);
		process_kbd_events(kbd);
		
		clear_terminal((CHAR_T)' ');
		
		/*
		mat4 m = diagmat4f(NULL);
		utime_t time = gettime_mls(CLOCK_MONOTONIC_RAW);
		utime_t delta_time = time - prev_time;
		prev_time = time;
		angle += 0.003f * delta_time;
		m.rc[0][0] =  cos(angle);
		m.rc[0][1] =  sin(angle);
		m.rc[1][0] = -sin(angle);
		m.rc[1][1] =  cos(angle);
		*/

		mat4 view = mat4f(NULL);
			
		view.rc[0][0] = cam_right.x;
		view.rc[0][1] = cam_right.y;
		view.rc[0][2] = cam_right.z;
		view.rc[1][0] = cam_up.x;
		view.rc[1][1] = cam_up.y;
		view.rc[1][2] = cam_up.z;
		view.rc[2][0] = cam_dir.x;
		view.rc[2][1] = cam_dir.y;
		view.rc[2][2] = cam_dir.z;
		view.rc[3][3] = 1;

		mat4 cam_translation = diagmat4f(1);

		cam_translation.rc[0][3] = -cam_pos.x;
		cam_translation.rc[1][3] = -cam_pos.y;
		cam_translation.rc[2][3] = -cam_pos.z;

		mat4 view_final  = mult_m4(&view, &cam_translation);
		mat4 proj_view = mult_m4(&proj, &view_final);

		draw_buffer(id, &proj_view);

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
