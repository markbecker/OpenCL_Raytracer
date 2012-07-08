#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

#include "common.h"

/** Variable Declarations **/
// memory buffers for opencl kernel
cl_mem primitive_buffer; // stores primitive list
cl_mem pixel_buffer; // stores traced pixels

cl_context			context;
cl_device_id		*devices;
cl_command_queue	command_queue;
cl_program			program;
cl_kernel			raytracer_kernel;

/** Function Declarations **/
int initialize_openCL(int deviceType, int width, int height, char * defines, Primitive * primitive_list, int n_primitives, Pixel * out_pixels, int platform);
int run_openCL_kernel(int width, int height, float camera_x, float camera_z, float camera_y, 
	float viewport_width, float viewport_height, int workgroup_size, int n_primitives, Pixel * out_pixels);
int cleanup_openCL(void);
static char * read_kernel(const char * filename);

#endif