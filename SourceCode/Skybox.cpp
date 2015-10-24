#include "Skybox.h"

#define FRONT "Content/negz_1.jpg"
#define BACK "Content/posz_1.jpg"
#define TOP "Content/posy_1.jpg"
#define BOTTOM "Content/negy_1.jpg"
#define LEFT "Content/negx_1.jpg"
#define RIGHT "Content/posx_1.jpg"
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444
#define MESH_FILE "Content/suzanne.obj"
#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 1000

GLuint Skybox::make_big_cube() {
	float points[] = {
		-40.0f, 50.0f, -40.0f,
		-40.0f, -30.0f, -40.0f,
		40.0f, -30.0f, -40.0f,
		40.0f, -30.0f, -40.0f,
		40.0f, 50.0f, -40.0f,
		-40.0f, 50.0f, -40.0f,

		-40.0f, -30.0f, 40.0f,
		-40.0f, -30.0f, -40.0f,
		-40.0f, 50.0f, -40.0f,
		-40.0f, 50.0f, -40.0f,
		-40.0f, 50.0f, 40.0f,
		-40.0f, -30.0f, 40.0f,

		40.0f, -30.0f, -40.0f,
		40.0f, -30.0f, 40.0f,
		40.0f, 50.0f, 40.0f,
		40.0f, 50.0f, 40.0f,
		40.0f, 50.0f, -40.0f,
		40.0f, -30.0f, -40.0f,

		-40.0f, -30.0f, 40.0f,
		-40.0f, 50.0f, 40.0f,
		40.0f, 50.0f, 40.0f,
		40.0f, 50.0f, 40.0f,
		40.0f, -30.0f, 40.0f,
		-40.0f, -30.0f, 40.0f,

		-40.0f, 50.0f, -40.0f,
		40.0f, 50.0f, -40.0f,
		40.0f, 50.0f, 40.0f,
		40.0f, 50.0f, 40.0f,
		-40.0f, 50.0f, 40.0f,
		-40.0f, 50.0f, -40.0f,

		-40.0f, -30.0f, -40.0f,
		-40.0f, -30.0f, 40.0f,
		40.0f, -30.0f,-40.0f,
		40.0f, -30.0f, -40.0f,
		-40.0f, -30.0f, 40.0f,
		40.0f, -30.0f, 40.0f
	};
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, 3 * 36 * sizeof (GLfloat), &points, GL_STATIC_DRAW);

	GLuint vao0;
	glGenVertexArrays(1, &vao0);
	glBindVertexArray(vao0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	return vao0;
}

/* use stb_image to load an image file into memory, and then into one side of
a cube-map texture. */
bool Skybox::load_cube_map_side(
	GLuint texture, GLenum side_target, const char* file_name
	) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int x, y, n;
	int force_channels = 4;
	unsigned char*  image_data = stbi_load(
		file_name, &x, &y, &n, force_channels);
	if (!image_data) {
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// non-power-of-2 dimensions check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf(
			stderr, "WARNING: image %s is not power-of-2 dimensions\n", file_name
			);
	}

	// copy image data into 'target' side of cube map
	glTexImage2D(
		side_target,
		0,
		GL_RGBA,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data
		);
	free(image_data);
	return true;
}

/* load all 6 sides of the cube-map from images, then apply formatting to the
final texture */
void Skybox::create_cube_map(
	const char* front,
	const char* back,
	const char* top,
	const char* bottom,
	const char* left,
	const char* right,
	GLuint* tex_cube
	) {
	// generate a cube-map texture to hold all the sides
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, tex_cube);

	// load each image and copy into a side of the cube-map texture
	assert(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front));
	assert(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back));
	assert(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top));
	assert(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom));
	assert(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left));
	assert(load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right));
	// format cube map texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

Skybox::Skybox() {
	cam_pos = vec3(0, -2.5, 5);

	cube_vao = make_big_cube();
	create_cube_map(FRONT, BACK, TOP, BOTTOM, LEFT, RIGHT, &cube_map_texture);

	GLfloat* vp = NULL; // array of vertex points
	GLfloat* vn = NULL; // array of vertex normals
	GLfloat* vt = NULL; // array of texture coordinates
	g_point_count = 0;

	assert(load_obj_file(MESH_FILE, vp, vt, vn, g_point_count));

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint points_vbo, normals_vbo;
	if (NULL != vp) {
		glGenBuffers(1, &points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
		glBufferData(
			GL_ARRAY_BUFFER, 3 * g_point_count * sizeof (GLfloat), vp, GL_STATIC_DRAW
			);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
	}
	if (NULL != vn) {
		glGenBuffers(1, &normals_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
		glBufferData(
			GL_ARRAY_BUFFER, 3 * g_point_count * sizeof (GLfloat), vn, GL_STATIC_DRAW
			);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
	}


	monkey_sp = create_programme_from_files("shaders/reflect.vs", "shaders/reflect.fs");
	monkey_M_location = glGetUniformLocation(monkey_sp, "M");
	monkey_V_location = glGetUniformLocation(monkey_sp, "V");
	monkey_P_location = glGetUniformLocation(monkey_sp, "P");

	// cube-map shaders
	cube_sp = create_programme_from_files("shaders/cube.vs", "shaders/cube.fs");

	// note that this view matrix should NOT contain camera translation.
	cube_V_location = glGetUniformLocation(cube_sp, "V");
	cube_P_location = glGetUniformLocation(cube_sp, "P");

	// input variables
	float nearr = 0.1f; // clipping plane
	float farr = 100.0f; // clipping plane
	float fovy = 67.0f; // 67 degrees
	float aspect = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT; // aspect ratio
	proj_mat = perspective(fovy, aspect, nearr, farr);

	float cam_speed = 3.0f; // 1 unit per second
	float cam_heading_speed = 50.0f; // 30 degrees per second
	cam_heading = 0.0f; // y-rotation in degrees
	mat4 T = translate(
		identity_mat4(), vec3(-cam_pos.v[0], -cam_pos.v[1], -cam_pos.v[2])
		);
	R = rotate_y_deg(identity_mat4(), -cam_heading);
	q = quat_from_axis_deg(-cam_heading, 0.0f, 1.0f, 0.0f);
	view_mat = R * T;
	// keep track of some useful vectors that can be used for keyboard movement
	fwd = vec4(0.0f, 0.0f, -1.0f, 0.0f);
	rgt = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	up = vec4(0.0f, 1.0f, 0.0f, 0.0f);

	glUseProgram(monkey_sp);
	glUniformMatrix4fv(monkey_V_location, 1, GL_FALSE, view_mat.m);
	glUniformMatrix4fv(monkey_P_location, 1, GL_FALSE, proj_mat.m);
	glUseProgram(cube_sp);
	glUniformMatrix4fv(cube_V_location, 1, GL_FALSE, R.m);
	glUniformMatrix4fv(cube_P_location, 1, GL_FALSE, proj_mat.m);

	model_mat = identity_mat4();

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // set counter-clock-wise vertex order to mean the front
	glClearColor(0.2, 0.2, 0.2, 1.0); // grey background to help spot mistakes
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glUseProgram(0);

	cout << "done making skybox" << endl;
}

void Skybox::Render() {
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glFrontFace(GL_CCW);

	// render a sky-box using the cube-map texture
	//glDepthMask(GL_FALSE);
	glUseProgram(cube_sp);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map_texture);
	glBindVertexArray(cube_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	//glUseProgram(0);

	glUseProgram(monkey_sp);
	glBindVertexArray(vao);
	glUniformMatrix4fv(monkey_M_location, 1, GL_FALSE, model_mat.m);
	glDrawArrays(GL_TRIANGLES, 0, g_point_count);

	// control keys
	bool cam_moved = false;
	move = vec3(0.0, 0.0, 0.0);
	float cam_yaw = 0.0f; // y-rotation in degrees
	float cam_pitch = 0.0f;
	float cam_roll = 0.0;

	

	glUseProgram(0);

	glFrontFace(GL_CW);

}

void Skybox::RotSB(float rot, float x, float y, float z) {
	versor q_temp = quat_from_axis_deg(-rot, x, y, z);
	q = q_temp * q;
	Calc();
}

void Skybox::MoveSB(float posx, float posy, float posz) {
	move.v[0] -= posx;
	move.v[1] -= posy;
	move.v[2] -= posz;
	Calc();
}

void Skybox::Calc() {
	R = quat_to_mat4(q);
	fwd = R * vec4(0.0, 0.0, -1.0, 0.0);
	rgt = R * vec4(1.0, 0.0, 0.0, 0.0);
	up = R * vec4(0.0, 1.0, 0.0, 0.0);

	cam_pos = cam_pos + vec3(fwd) * -move.v[2];
	cam_pos = cam_pos + vec3(up) * move.v[1];
	cam_pos = cam_pos + vec3(rgt) * move.v[0];

	mat4 T = translate(identity_mat4(), vec3(cam_pos));

	view_mat = inverse(R) * inverse(T);
	glUseProgram(monkey_sp);
	glUniformMatrix4fv(monkey_V_location, 1, GL_FALSE, view_mat.m);

	// cube-map view matrix has rotation, but not translation
	glUseProgram(cube_sp);
	glUniformMatrix4fv(cube_V_location, 1, GL_FALSE, inverse(R).m);
}