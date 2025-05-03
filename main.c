#include "input.h"
#include "cursor.h"
#include "terminal.h"
#include "render.h"
#include "mem.h"
#include "timeman.h"
#include "ctx.h"
#include "light.h"

static vec3 cam_pos = {
	.x = 0.f, 
	.y = 0.f, 
	.z = 40.f,
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

static render_mode_t mode = RENDER_MODE_SOLID;
static bool run = true;

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

void button_callback(_UNUSED unsigned short btn_action, _UNUSED int btn) {
	/*
	if (btn_action == KEY_PRESSED && btn == BTN_LEFT) {
	}
	else if (btn_action == KEY_PRESSED && btn == BTN_RIGHT) {
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
	
	if (get_key(kbd, KEY_Q) == KEY_PRESSED)
		run = false;

	prev_time = curr_time;
}

void log_msg(int x, int y, char* msg) {
	int half_width = get_terminal_width() / 2;
	int half_height = get_terminal_height() / 2;

	for (int i = 0; msg[i] != 0; i++) {
		set_elem(-half_width + x + i, half_height - y, msg[i], -1.f, COLOR_WHITE + 1);
	}
}

void print_fps(int x, int y) {
	static char msg[32];
	static UINT frames_cnt = 0;
	static float fps = 0;
	static utime_t prev = 0;

	utime_t time = gettime_mls(CLOCK_MONOTONIC_RAW);
	frames_cnt++;

	if (time - prev >= 1000) {
		fps = (float)frames_cnt / (time - prev) * 1000;
		frames_cnt = 0;
		prev = time;
	}

	sprintf(msg, "FPS: %.3f", fps);
	log_msg(x, y, msg);
}

void stage_vertex(_entry_t* entry, void* attrib) {
	mat4* vt = (mat4*)(attrib);
	mat3* nt = (mat3*)((byte*)attrib + sizeof(mat4));

	*_ENTRY_POS4(entry) = mult_mv4(vt, _ENTRY_POS4(entry));
	*_ENTRY_NORM(entry) = mult_mv3(nt, _ENTRY_NORM(entry));
}	

void stage_fragment(_entry_t* normalized, _UNUSED void* attrib) {	
	// NOTE: LIGHT ID CAN BE ALSO PASSED VIA EXTRA ATTRIBUTES BUFFER.
	light_id_t* light_ids;
	size_t light_cnt;
	register_light_get(&light_ids, &light_cnt);

	light_directional* light_dir;
	get_light_source(*light_ids, (void**)&light_dir, NULL);

	float diffuse_factor = max(0.f, -dot3f(&light_dir->dir, _ENTRY_NORM(normalized)));

	vec3 diffuse = mult_av3(diffuse_factor, &light_dir->diffuse);
	vec3 amb_diff = add3f(&light_dir->ambient, &diffuse);

	*_ENTRY_COL(normalized) = mult_v3(&amb_diff, _ENTRY_COL(normalized));
}

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

	set_framerate_limit(-1);

	set_render_mode(mode);

	float cube[] = {
		10.f, -10.f, 10.f,   0.f, 1.f, 0.f, 1.f, 0.f, 0.f,
		10.f, 10.f, 10.f,     1.f, 0.f, 0.f, 1.f, 0.f, 0.f,
		10.f, 10.f, -10.f,   0.f, 1.f, 0.f, 1.f, 0.f, 0.f,
		10.f, -10.f, 10.f,   1.f, 0.f, 0.f, 1.f, 0.f, 0.f,
		10.f, -10.f, -10.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f,
		10.f, 10.f, -10.f,   1.f, 0.f, 0.f, 1.f, 0.f, 0.f,

		-10.f, -10.f, 10.f, 0.f, 0.5f, 1.f, 0.f, 0.f, 1.f,
		-10.f, 10.f, 10.f, 1.f, 0.2f, 1.f, 0.f, 0.f, 1.f,
		10.f, 10.f, 10.f, 1.f, 0.5f, 0.6f, 0.f, 0.f, 1.f,
		10.f, 10.f, 10.f,  0.f, 0.5f, 1.f, 0.f, 0.f, 1.f,
		10.f, -10.f, 10.f, 0.f, 0.6f, 0.f, 0.f, 0.f, 1.f,
		-10.f, -10.f, 10.f, 0.3f, 0.5f, 0.5f, 0.f, 0.f, 1.f,

		-10.f, -10.f, -10.f, 0.f, 0.5f, 0.2f, 0.f, 0.f, -1.f,
		-10.f, 10.f, -10.f, 0.1f, 0.7f, 0.1f, 0.f, 0.f, -1.f,
		10.f, 10.f, -10.f, 0.6f, 0.f, 0.f, 0.f, 0.f, -1.f,
		10.f, 10.f, -10.f,  0.6f, 0.f, 0.f, 0.f, 0.f, -1.f,
		10.f, -10.f, -10.f, 0.6f, 1.f, 1.f, 0.f, 0.f, -1.f,
		-10.f, -10.f, -10.f, 0.1f, 1.f, 1.f, 0.f, 0.f, -1.f,

		-10.f, -10.f, 10.f, 1.f, 0.f, 0.f, -1.f, 0.f, 0.f,
		-10.f, 10.f, 10.f, 1.f, 1.f, 1.f, -1.f, 0.f, 0.f,
		-10.f, 10.f, -10.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f,
		-10.f, -10.f, 10.f, 1.f, 1.f, 1.f, -1.f, 0.f, 0.f,
		-10.f, -10.f, -10.f, 0.f, 1.f, 1.f, -1.f, 0.f, 0.f,
		-10.f, 10.f, -10.f, 1.f, 0.f, 0.f, -1.f, 0.f, 0.f,

		-10.f, 10.f, 10.f, 0.4f, 1.f, 1.f, 0.f, 1.f, 0.f,
		-10.f, 10.f, -10.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f,
		10.f, 10.f, -10.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f,
		10.f, 10.f, -10.f, 0.2f, 1.f, 1.f, 0.f, 1.f, 0.f,
		10.f, 10.f, 10.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f,
		-10.f, 10.f, 10.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f,

		-10.f, -10.f, 10.f, 1.f, 0.1f, 0.5f, 0.f, -1.f, 0.f,
		-10.f, -10.f, -10.f, 1.f, 0.f, 0.2f, 0.f, -1.f, 0.f,
		10.f, -10.f, -10.f, 0.f, 0.f, 1.f, 0.f, -1.f, 0.f,
		10.f, -10.f, -10.f, 1.f, 0.f, 1.f, 0.f, -1.f, 0.f,
		10.f, -10.f, 10.f, 1.f, 1.f, 1.f, 0.f, -1.f, 0.f,
		-10.f, -10.f, 10.f, 0.5f, 1.f, 1.f, 0.f, -1.f, 0.f
	};

	vec3 cube_pos[5];
	cube_pos[0] = vec3f(0.f, 0.f, 0.f);
	cube_pos[1] = vec3f(25.f, 20.f, -30.f);
	cube_pos[2] = vec3f(40.f, -20.f, -40.f);
	cube_pos[3] = vec3f(-60.f, 10.f, -20.f);
	cube_pos[4] = vec3f(60.f, 0.f, -25.f);

	buff_idx_t id;
	if (gen_mem_buff(cube, sizeof(cube), 9 * sizeof(float), &id) == -1) {
		fprintf(stderr, "Failed to create buffer\n");
		return -1;
	}
	
	mat4 proj = projmat4f(60.f, 16.f / 9.f, 0.1f, 300.f);

	float angle = 0.f;
	utime_t prev_time = gettime_mls(CLOCK_MONOTONIC_RAW);
	
	char msg[512];

	light_id_t light_id;
	add_light_source(&light_id, LIGHT_DIRECTIONAL);
	
	light_directional* light;
	get_light_source(light_id, (void**)&light, NULL);

	light->dir = (vec3){
		.x = -1.f,
		.y = 0.f,
		.z = 0.f,
	};
	light->ambient = (vec3){
		.x = 0.1f,
		.y = 0.1f,
		.z = 0.1f,
	};
	light->diffuse = (vec3){
		.x = 1.f,
		.y = 1.f,
		.z = 1.f,
	};

	register_light_source(&light_id, 1);

	shader_t shader;
	shader.stage_vertex = stage_vertex;
	shader.stage_fragment = stage_fragment;

	while (run && !should_quit()) {
		clear_terminal((CHAR_T)' ');

		process_kbd_events(kbd);

		mat4 rot = diagmat4f(1);
		utime_t time = gettime_mls(CLOCK_MONOTONIC_RAW);
		utime_t delta_time = time - prev_time;
		prev_time = time;
		angle += 0.001f * delta_time;
		rot.rc[0][0] =  cos(angle);
		rot.rc[0][1] =  sin(angle);
		rot.rc[1][0] = -sin(angle);
		rot.rc[1][1] =  cos(angle);

		mat4 view = viewmat4f(&cam_pos, &cam_dir, &cam_up, &cam_right);

		for (UINT i = 0; i < 5; i++) {
			mat4 transl = diagmat4f(1);

			transl.rc[0][3] = cube_pos[i].x;
			transl.rc[1][3] = cube_pos[i].y;
			transl.rc[2][3] = cube_pos[i].z;

			mat4 m = mult_m4(&transl, &rot);

			mat4 view_model = mult_m4(&view, &m);
			mat4 proj_view_model = mult_m4(&proj, &view_model);
			mat3 normal_model = m3_from_m4(&m);
			
			byte mat_attrib_buff[128];
			memcpy(mat_attrib_buff, &proj_view_model, sizeof(mat4));
			memcpy(mat_attrib_buff + sizeof(mat4), &normal_model, sizeof(mat3));

			draw_buffer(&shader, id, mat_attrib_buff);
		}

		sprintf(msg, "POS: %f %f %f", cam_pos.x, cam_pos.y, cam_pos.z);
		log_msg(0, 0, msg);
		sprintf(msg, "DIR: %f %f %f", cam_dir.x, cam_dir.y, cam_dir.z);
		log_msg(0, 1, msg);
		sprintf(msg, "RIGHT: %f %f %f", cam_right.x, cam_right.y, cam_right.z);
		log_msg(0, 2, msg);
		
		print_fps(0, 3);

		ULONGLONG a, b;
		get_late_data(&a, &b);
		sprintf(msg, "FLUSH LATE CNT: %llu, WRITER LATE CNT: %llu", a, b);
		log_msg(0, 4, msg);

		poll_events_keyboard(kbd);
		poll_events_mouse(mice);
		
		swap_terminal_buffers();
	}

	register_light_clear();

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
