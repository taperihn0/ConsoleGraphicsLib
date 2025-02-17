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

	prev_time = curr_time;
}

void log_msg(int x, int y, char* msg) {
	int half_width = get_terminal_width() / 2;
	int half_height = get_terminal_height() / 2;

	for (int i = 0; msg[i] != 0; i++) {
		set_elem(-half_width + x + i, half_height - y, msg[i], 1.f);
	}
}

mat4 projection_mat4f(float fov, float aspect_ratio, float n, float f) {
	mat4 proj = mat4f(NULL);
	
	float t = tan(RADIANS(fov / 2.f)) * n;
	float r = t * aspect_ratio;

	proj.rc[0][0] = n / r;
	proj.rc[1][1] = n / t;
	proj.rc[2][2] = -(f + n) / (n - f);
	proj.rc[2][3] = 2 * f * n / (n - f);
	proj.rc[3][2] = 1;

	return proj;
}

mat4 view_mat4f(vec3* pos, vec3* dir, vec3* up, vec3* right) {
	mat4 view = mat4f(NULL);

	view.rc[0][0] = right->x;
	view.rc[0][1] = right->y;
	view.rc[0][2] = right->z;
	view.rc[1][0] = up->x;
	view.rc[1][1] = up->y;
	view.rc[1][2] = up->z;
	view.rc[2][0] = dir->x;
	view.rc[2][1] = dir->y;
	view.rc[2][2] = dir->z;
	view.rc[3][3] = 1;

	mat4 translation = diagmat4f(1);

	translation.rc[0][3] = -pos->x;
	translation.rc[1][3] = -pos->y;
	translation.rc[2][3] = -pos->z;

	mat4 final = mult_m4(&view, &translation);
	return final;
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

	set_framerate_limit(30);

	float pos[] = {
		10.f, -10.f, 10.f,
		10.f, 10.f, 10.f,
		10.f, 10.f, -10.f,
		10.f, -10.f, 10.f,
		10.f, -10.f, -10.f,
		10.f, 10.f, -10.f,

		-10.f, -10.f, 10.f,
		-10.f, 10.f, 10.f,
		10.f, 10.f, 10.f,
		10.f, 10.f, 10.f, 
		10.f, -10.f, 10.f,
		-10.f, -10.f, 10.f,

		-10.f, -10.f, -10.f,
		-10.f, 10.f, -10.f,
		10.f, 10.f, -10.f,
		10.f, 10.f, -10.f, 
		10.f, -10.f, -10.f,
		-10.f, -10.f, -10.f,

		-10.f, -10.f, 10.f,
		-10.f, 10.f, 10.f,
		-10.f, 10.f, -10.f,
		-10.f, -10.f, 10.f,
		-10.f, -10.f, -10.f,
		-10.f, 10.f, -10.f,

		-10.f, 10.f, 10.f,
		-10.f, 10.f, -10.f,
		10.f, 10.f, -10.f,
		10.f, 10.f, -10.f,
		10.f, 10.f, 10.f,
		-10.f, 10.f, 10.f,

		-10.f, -10.f, 10.f,
		-10.f, -10.f, -10.f,
		10.f, -10.f, -10.f,
		10.f, -10.f, -10.f,
		10.f, -10.f, 10.f,
		-10.f, -10.f, 10.f
	};

	buff_idx_t id;
	if (gen_mem_buff(pos, sizeof(pos), &id) == -1) {
		fprintf(stderr, "Failed to create buffer\n");
		return -1;
	}
	
	mat4 proj = projection_mat4f(60.f, 16 / 9.f, 0.1f, 100.f);

	float angle = 0.f;
	utime_t prev_time = gettime_mls(CLOCK_MONOTONIC_RAW);
	
	char msg[512];

	while (!should_quit()) {
		poll_events_keyboard(kbd);
		poll_events_mouse(mice);

		process_kbd_events(kbd);
		
		clear_terminal((CHAR_T)' ');

		mat4 m = diagmat4f(1);
		utime_t time = gettime_mls(CLOCK_MONOTONIC_RAW);
		utime_t delta_time = time - prev_time;
		prev_time = time;
		angle += 0.001f * delta_time;
		m.rc[0][0] =  cos(angle);
		m.rc[0][1] =  sin(angle);
		m.rc[1][0] = -sin(angle);
		m.rc[1][1] =  cos(angle);

		mat4 view = view_mat4f(&cam_pos, &cam_dir, &cam_up, &cam_right);
		mat4 view_model = mult_m4(&view, &m);
		mat4 proj_view_model = mult_m4(&proj, &view_model);
		
		sprintf(msg, "POS: %f %f %f", cam_pos.x, cam_pos.y, cam_pos.z);
		log_msg(0, 0, msg);
		sprintf(msg, "DIR: %f %f %f", cam_dir.x, cam_dir.y, cam_dir.z);
		log_msg(0, 1, msg);
		sprintf(msg, "RIGHT: %f %f %f", cam_right.x, cam_right.y, cam_right.z);
		log_msg(0, 2, msg);

		draw_buffer(id, &proj_view_model);

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
