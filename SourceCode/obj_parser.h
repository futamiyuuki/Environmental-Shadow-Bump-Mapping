#ifndef _OBJ_PARSER_H_
#define _OBJ_PARSER_H_

bool load_obj_file (
	const char* file_name,
	float*& points,
	float*& tex_coords,
	float*& normals,
	int& point_count
);

#endif
