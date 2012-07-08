#ifndef __COMMON_H__
#define __COMMON_H__

#include <CL/cl.h>

typedef cl_uchar4 Pixel;
typedef cl_float4 Color;

typedef enum {
	PLANE = 0,
	SPHERE = 1
} prim_type;

// The dummy_3 value is used to align the struct properly.
// OpenCL requires 16-byte alignment (ie 4 floats) for its vector data types. Without
// the dummy_3 value, the Primitive struct will not copy from the host correctly.
typedef struct {
	Color m_color; // material_color
	cl_float m_refl; // material_reflection(_factor)
	cl_float m_diff; // material_diffuse(_lighting_factor)
	cl_float m_refr; // material_refraction(_factor)
	cl_float m_refr_index; // material_refraction_index
	cl_float m_spec; // material_specular(_lighting_factor)
	cl_float dummy_3; // dummy value
	prim_type type; // primitive type
	cl_bool is_light; // light?
	cl_float4 normal; // normal used to define a plane
	cl_float4 center; // center used to define a sphere
	cl_float depth; // depth used to define a plane
	cl_float radius, sq_radius, r_radius; // radius used to define a sphere
} Primitive;

int do_raytrace_no_ocl(Pixel * out_pixels, Primitive * prim_list, int n_primitives, int depth,
	int width, int height, float camera_x, float camera_y, float camera_z, float viewport_width, 
	float viewport_height);

int do_raytrace_ocl(Pixel * out_pixels, Primitive * prim_list, int n_primitives, int use_cpu, int depth, 
	int workgroup_size, int fast_norm, int buil_norm, int nati_sqrt, int buil_dot, int buil_len, int width, 
	int height, float camera_x, float camera_y, float camera_z, float viewport_width, float viewport_height, int ocl_plat);

#endif