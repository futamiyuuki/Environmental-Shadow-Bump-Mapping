#ifndef SKYBOX_H
#define	SKYBOX_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include "stb_image.h"
#include "maths_funcs.h"
#include "obj_parser.h"
#include "util.h"
#include "gl_utils.h"
#include "engine_common.h"

class Skybox
{
public:
	Skybox();

	~Skybox();

	void Render();

	//void RotSB(versor q_rot);
	void RotSB(float rot, float x, float y, float z);
	void MoveSB(float posx, float posy, float posz);

	void Calc();

private:
	GLuint make_big_cube();
	bool load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name);
	void create_cube_map(const char* front,
		const char* back,
		const char* top,
		const char* bottom,
		const char* left,
		const char* right,
		GLuint* tex_cube);

	GLuint monkey_sp;
	GLuint cube_sp;
	GLuint cube_map_texture;
	GLuint cube_vao;
	GLuint vao;
	int monkey_M_location;
	int monkey_V_location;
	int monkey_P_location;
	int cube_V_location;
	int cube_P_location;
	mat4 model_mat;
	mat4 view_mat;
	mat4 proj_mat;
	vec3 cam_pos;
	int g_point_count;
	float cam_heading;
	mat4 R;
	versor q;
	vec4 fwd;
	vec4 rgt;
	vec4 up;
	vec3 move;
};

#endif	/* SKYBOX_H */
