#include "input.h"
#include "cursor.h"
#include "terminal.h"
#include "render.h"
#include "mem.h"
#include "timeman.h"
#include "ctx.h"

static vec3 cam_pos = {
	.x = 0.f, 
	.y = 0.f, 
	.z = 0.f,
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

render_mode_t mode = RENDER_MODE_SOLID;

void mouse_callback(int dx, int dy) {
	static float yaw = -90.f;
	static float pitch = 0.f;

	float xoffset = dx * 0.1f;
	float yoffset = dy * 0.1f;

	yaw += xoffset;
	pitch -= yoffset;

	if (pitch > 89.f)
		pitch = 89.f;
	else if (pitch < -89.f)
		pitch = -89.f;

	cam_dir.x = cos(RADIANS(yaw)) * cos(RADIANS(pitch));
	cam_dir.y = sin(RADIANS(pitch));
	cam_dir.z = sin(RADIANS(yaw)) * cos(RADIANS(pitch));
	
	static vec3 tmp_up = {
		0.f, 1.f, 0.f
	};

	cam_right = cross3f(&cam_dir, &tmp_up);
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
	if (get_key(kbd, KEY_SPACE) == KEY_PRESSED) {
		cam_pos.y += 0.01 * delta_time;
	}
	if (get_key(kbd, KEY_LEFTSHIFT) == KEY_PRESSED) {
		cam_pos.y -= 0.01 * delta_time;
	}

	if (get_key(kbd, KEY_M) == KEY_PRESSED) {
		mode++;
		mode %= _RENDER_MODE_CNT;
		set_render_mode(mode);
	}

	prev_time = curr_time;
}

void log_msg(int x, int y, char* msg) {
	int half_width = get_terminal_width() / 2;
	int half_height = get_terminal_height() / 2;

	for (int i = 0; msg[i] != 0; i++) {
		set_elem(-half_width + x + i, half_height - y, msg[i], 1.f);
	}
}

#include "charmap.h"

int main() {
	init_mode();

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

	set_framerate_limit(40);

	set_render_mode(mode);

	float cube[] = {
		10.f, -10.f, 10.f, 0.f, 0.01f, 0.01f,
		10.f, 10.f, 10.f, 1.f, 0.01f, 0.01f,
		10.f, 10.f, -10.f, 1.f, 0.01f, 0.01f,
		10.f, -10.f, 10.f, 0.f, 1.f, 1.f,
		10.f, -10.f, -10.f, 1.f, 1.f, 1.f,
		10.f, 10.f, -10.f, 1.f, 1.f, 1.f,

		-10.f, -10.f, 10.f, 1.f, 1.f, 1.f,
		-10.f, 10.f, 10.f, 1.f, 1.f, 1.f,
		10.f, 10.f, 10.f, 1.f, 1.f, 1.f,
		10.f, 10.f, 10.f,  1.f, 1.f, 1.f,
		10.f, -10.f, 10.f, 1.f, 1.f, 1.f,
		-10.f, -10.f, 10.f, 1.f, 1.f, 1.f,

		-10.f, -10.f, -10.f, 1.f, 1.f, 1.f,
		-10.f, 10.f, -10.f, 1.f, 1.f, 1.f,
		10.f, 10.f, -10.f, 1.f, 1.f, 1.f,
		/*
		10.f, 10.f, -10.f,  1.f, 1.f, 1.f,
		10.f, -10.f, -10.f, 1.f, 1.f, 1.f,
		-10.f, -10.f, -10.f, 1.f, 1.f, 1.f,

		-10.f, -10.f, 10.f, 1.f, 1.f, 1.f,
		-10.f, 10.f, 10.f, 1.f, 1.f, 1.f,
		-10.f, 10.f, -10.f, 1.f, 1.f, 1.f,
		-10.f, -10.f, 10.f, 1.f, 1.f, 1.f,
		-10.f, -10.f, -10.f, 1.f, 1.f, 1.f,
		-10.f, 10.f, -10.f, 1.f, 1.f, 1.f,

		-10.f, 10.f, 10.f, 1.f, 1.f, 1.f,
		-10.f, 10.f, -10.f, 1.f, 1.f, 1.f,
		10.f, 10.f, -10.f, 1.f, 1.f, 1.f,
		10.f, 10.f, -10.f, 1.f, 1.f, 1.f,
		10.f, 10.f, 10.f, 1.f, 1.f, 1.f,
		-10.f, 10.f, 10.f, 1.f, 1.f, 1.f,

		-10.f, -10.f, 10.f, 1.f, 1.f, 1.f,
		-10.f, -10.f, -10.f, 1.f, 1.f, 1.f,
		10.f, -10.f, -10.f, 1.f, 1.f, 1.f,
		10.f, -10.f, -10.f, 1.f, 1.f, 1.f,
		10.f, -10.f, 10.f, 1.f, 1.f, 1.f,
		-10.f, -10.f, 10.f, 1.f, 1.f, 1.f*/
	};

	vec3 cube_pos[5];
	cube_pos[0] = vec3f(0.f, 0.f, 0.f);
	cube_pos[1] = vec3f(25.f, 20.f, -30.f);
	cube_pos[2] = vec3f(40.f, -20.f, -40.f);
	cube_pos[3] = vec3f(-60.f, 10.f, -20.f);
	cube_pos[4] = vec3f(60.f, 0.f, -25.f);

	buff_idx_t id;
	if (gen_mem_buff(cube, sizeof(cube), 3 * sizeof(float) + 3 * sizeof(float), &id) == -1) {
		fprintf(stderr, "Failed to create buffer\n");
		return -1;
	}
	
	mat4 proj = projmat4f(60.f, 16 / 9.f, 0.1f, 300.f);

	float angle = 0.f;
	utime_t prev_time = gettime_mls(CLOCK_MONOTONIC_RAW);
	
	char msg[512];

	while (!should_quit()) {
		poll_events_keyboard(kbd);
		poll_events_mouse(mice);

		process_kbd_events(kbd);
		
		clear_terminal((CHAR_T)' ');

		mat4 rot = diagmat4f(1);
		utime_t time = gettime_mls(CLOCK_MONOTONIC_RAW);
		utime_t delta_time = time - prev_time;
		prev_time = time;
		angle += 0.001f * delta_time;
		rot.rc[0][0] =  cos(0);
		rot.rc[0][1] =  sin(0);
		rot.rc[1][0] = -sin(0);
		rot.rc[1][1] =  cos(0);

		mat4 view = viewmat4f(&cam_pos, &cam_dir, &cam_up, &cam_right);

		for (UINT i = 0; i < 1; i++) {
			mat4 transl = diagmat4f(1);

			transl.rc[0][3] = cube_pos[i].x;
			transl.rc[1][3] = cube_pos[i].y;
			transl.rc[2][3] = cube_pos[i].z;

			mat4 m = mult_m4(&transl, &rot);

			mat4 view_model = mult_m4(&view, &m);
			mat4 proj_view_model = mult_m4(&proj, &view_model);

			draw_buffer(id, &proj_view_model);
		}

		if (get_key(kbd, KEY_Q) == KEY_PRESSED)
			break;

		sprintf(msg, "POS: %f %f %f", cam_pos.x, cam_pos.y, cam_pos.z);
		log_msg(0, 0, msg);
		sprintf(msg, "DIR: %f %f %f", cam_dir.x, cam_dir.y, cam_dir.z);
		log_msg(0, 1, msg);
		sprintf(msg, "RIGHT: %f %f %f", cam_right.x, cam_right.y, cam_right.z);
		log_msg(0, 2, msg);

	    swap_terminal_buffers();
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

	close_mode();

	fflush(stdout);
	return 0;
}
