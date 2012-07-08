#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "argtable2.h"
#include "common.h"
#include "bitmap.h"

Primitive * load_scene(int & n_primitives, const char * scene)
{
	FILE * scene_file;
	scene_file = fopen(scene, "r");

	if (scene_file == NULL)
	{
		fprintf(stderr, "Scene file not found\n");
		return NULL;
	}

	Primitive * prim_list;

	char buf[BUFSIZ];
	// scenefile format
	// line 1: n_primitives
	// line 2+: primitive
	// type,r,g,b,refl,refr,refr_index,diff,spec,is_light,center/normal_x,center/normal_y,center/normal_z,radius/depth
	// type 0 = PLANE, type 1 = SPHERE; is_light 0 = FALSE, 1 = TRUE
	if (fgets (buf, BUFSIZ, scene_file) == NULL || sscanf(buf, "%d", &n_primitives) != 1)
	{
		fprintf(stderr, "Scene file format invalid\n");
		return NULL;
	}

	prim_list = (Primitive *) malloc(sizeof(Primitive) * n_primitives);

	if (prim_list == NULL)
	{
		fprintf(stderr,"Error: Failed to allocate primitive list memory on host.\n");
		return NULL;
	}

	memset(prim_list, 0, sizeof(Primitive) * n_primitives);

	for (int i = 0; i < n_primitives; i++)
	{
		int type = 0, light = 0;
		cl_float4 center_normal;
		center_normal.s[0] = 0;
		center_normal.s[1] = 0;
		center_normal.s[2] = 0;
		center_normal.s[3] = 0;
		cl_float radius_depth = 1;
		if (fgets (buf, BUFSIZ, scene_file) == NULL || sscanf(buf, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%d,%f,%f,%f,%f", &type, &prim_list[i].m_color.s[0], 
			&prim_list[i].m_color.s[1],	&prim_list[i].m_color.s[2], &prim_list[i].m_refl, &prim_list[i].m_refr, &prim_list[i].m_refr_index, 
			&prim_list[i].m_diff, &prim_list[i].m_spec, &light, &center_normal.s[0], &center_normal.s[1], &center_normal.s[2], 
			&radius_depth) != 14)
		{
			fprintf(stderr, "Scene file format invalid. Primitive %d\n", i + 1);
			return NULL;
		}

		prim_list[i].is_light = (light == 0) ? false : true;

		switch (type)
		{
		case 0:
			prim_list[i].type = PLANE;
			prim_list[i].normal.s[0] = center_normal.s[0];
			prim_list[i].normal.s[1] = center_normal.s[1];
			prim_list[i].normal.s[2] = center_normal.s[2];
			prim_list[i].depth = radius_depth;
			break;
		case 1:
			prim_list[i].type = SPHERE;
			prim_list[i].center.s[0] = center_normal.s[0];
			prim_list[i].center.s[1] = center_normal.s[1];
			prim_list[i].center.s[2] = center_normal.s[2];
			prim_list[i].radius = radius_depth;
			prim_list[i].sq_radius = radius_depth * radius_depth;
			prim_list[i].r_radius = 1.0f / radius_depth;
			break;
		default:
			fprintf(stderr, "Scene file format invalid. Primitive %d\n", i + 1);
			return NULL;
		}
	}

	return prim_list;
}

int run_tracer(int no_ocl, int use_cpu, int depth, int workgroup_size, int fast_norm, int buil_norm, int nati_sqrt, 
	int buil_dot, int buil_len, int width, int height, float viewport_width, float viewport_height, 
	float camera_x, float camera_y, float camera_z, const char* scene, const char* outfile, int ocl_plat)
{
	// Load scene
	printf("Loading scene..\n");
	int n_primitives;
	Primitive * primitive_list;
	primitive_list = load_scene(n_primitives, scene);

	if (primitive_list == NULL)
	{
		fprintf(stderr, "Failed to load scene from file: %s\n", scene);
		return 1;
	}

	// Allocate pixels for result
	Pixel * out_pixels;

	cl_uint pixel_size_bytes = sizeof(Pixel) * width * height;
	out_pixels = (Pixel *) malloc(pixel_size_bytes);

	if (out_pixels == NULL)
	{
		fprintf(stderr,"Error: Failed to allocate output pixel memory on host.\n");
		return 1;
	}

	// null all colors to 0
	memset(out_pixels, 0, pixel_size_bytes);

	int retval;

	printf("Raytracing..\n");
	// Perform raytracing
	if (no_ocl > 0)
	{
		retval = do_raytrace_no_ocl(out_pixels, primitive_list, n_primitives, depth, width, height, 
			camera_x, camera_y, camera_z, viewport_width, viewport_height);
	}
	else
	{
		retval = do_raytrace_ocl(out_pixels, primitive_list, n_primitives, use_cpu, depth, 
			workgroup_size, fast_norm, buil_norm, nati_sqrt, buil_dot, buil_len, width, 
			height, camera_x, camera_y, camera_z, viewport_width, viewport_height, ocl_plat);
	}

	if (retval == 1)
	{
		fprintf(stderr, "Raytracing failed. Aborting image write.\n");
		return 1;
	}

	// Write output
	fprintf(stderr, "Writing image..\n");
	retval = write_bmp_file(out_pixels, width, height, outfile);

	if (retval == 1)
	{
		fprintf(stderr, "Image write failed.\n");
		return 1;
	}

	fprintf(stderr, "Wrote image file.\n");
	// Free memory
	free(primitive_list);
	free(out_pixels);

	// Success!
	return 0;
}

int main(int argc, char **argv)
{
	// commandline argument handling
	const char *progname = "raytracer";
	struct arg_lit  *no_ocl   = arg_lit0(NULL, "no-opencl",              "render using non-OpenCL renderer");
	struct arg_lit  *use_cpu  = arg_lit0(NULL, "use-cpu",                "render using the CPU");
	struct arg_int  *ocl_plat = arg_int0(NULL, "opencl-platform", NULL,  "choose platform (default: 0)");
	struct arg_int  *depth    = arg_int0(NULL, "tracedepth", NULL,       "refl/refr depth to render (default: 5)");
	struct arg_int  *wg_size  = arg_int0(NULL, "workgroup-size", NULL,   "OpenCL workgroup size to use (def: 64)");
	struct arg_lit  *fast_norm = arg_lit0(NULL, "fast-normalize",        "use OpenCL fast_normalize builtin");
	struct arg_lit  *buil_norm = arg_lit0(NULL, "builtin-normalize",     "use OpenCL normalize builtin");
	struct arg_lit  *nati_sqrt = arg_lit0(NULL, "native-sqrt",           "use OpenCL native_sqrt builtin");
	struct arg_lit  *buil_dot  = arg_lit0(NULL, "builtin-dot",           "use OpenCL dot product builtin");
	struct arg_lit  *buil_len  = arg_lit0(NULL, "builtin-length",        "use OpenCL vector length builtin");
	struct arg_int  *width    = arg_int0(NULL, "width", NULL,            "width of output image (default: 800)");
	struct arg_int  *height   = arg_int0(NULL, "height", NULL,           "height of output image (default: 600)");
	struct arg_dbl  *viewp_w  = arg_dbl0(NULL, "viewport-width", NULL,   "viewport width (default: 6.0)");
	struct arg_dbl  *viewp_h  = arg_dbl0(NULL, "viewport-height", NULL,  "viewport height (default: 4.5)");
	struct arg_dbl  *camera_x = arg_dbl0(NULL, "camera-x", NULL,         "camera x value (default: 0.0)");
	struct arg_dbl  *camera_y = arg_dbl0(NULL, "camera-y", NULL,         "camera y value (default: 0.0)");
	struct arg_dbl  *camera_z = arg_dbl0(NULL, "camera-z", NULL,         "camera z value (default: 0.0)");
	struct arg_str  *scene    = arg_str0("s", "scene", NULL,             "scene to render (default: def.scn)");
	struct arg_str  *out_file = arg_str0("o", "outfile", NULL,           "output filename (default: out.bmp)");
	struct arg_lit  *help     = arg_lit0(NULL, "help",                   "display this help and exit");
	struct arg_end  *end      = arg_end(20);

	void* argtable[] = {help, no_ocl, use_cpu, ocl_plat, depth, wg_size, fast_norm, buil_norm, nati_sqrt, buil_dot, buil_len, 
		width, height, viewp_w, viewp_h, camera_x, camera_y, camera_z, scene, out_file, end};

	int exitcode=0;
	int nerrors;

	/* verify the argtable[] entries were allocated sucessfully */
	if (arg_nullcheck(argtable) != 0)
	{
		/* NULL entries were detected, some allocations must have failed */
		fprintf(stderr, "%s: insufficient memory\n",progname);
		exitcode=1;
		goto exit;
	}

	// set defaults
	depth->ival[0] = 5;
	width->ival[0] = 800;
	height->ival[0] = 600;
	viewp_w->dval[0] = 6.0;
	viewp_h->dval[0] = 4.5;
	wg_size->ival[0] = 64;
	camera_x->dval[0] = 0.0;
	camera_y->dval[0] = 0.25;
	camera_z->dval[0] = -7.0;
	scene->sval[0] = "def.scn";
	out_file->sval[0] = "out.bmp";
	ocl_plat->ival[0] = 1;

	/* Parse the command line as defined by argtable[] */
	nerrors = arg_parse(argc,argv,argtable);

	/* special case: '--help' takes precedence over error reporting */
	if (help->count > 0)
	{
		fprintf(stderr, "Usage: %s", progname);
		arg_print_syntax(stderr,argtable,"\n");
		fprintf(stderr, "Render the sample scene, using either the OpenCL renderer (CPU or GPU) or non-OpenCL renderer (CPU). "
			"Defaults to using the OpenCL renderer on the GPU.\n\n");
		arg_print_glossary(stderr,argtable,"  %-30s %s\n");
		exitcode=0;
		goto exit;
	}


	/* If the parser returned any errors then display them and exit */
	if (nerrors > 0)
	{
		/* Display the error details contained in the arg_end struct.*/
		arg_print_errors(stderr,end,progname);
		fprintf(stderr, "Try '%s --help' for more information.\n",progname);
		exitcode=1;
		goto exit;
	}

	exitcode = run_tracer(no_ocl->count, use_cpu->count, depth->ival[0], wg_size->ival[0], fast_norm->count, buil_norm->count, nati_sqrt->count,
		buil_dot->count, buil_len->count, width->ival[0], height->ival[0], (float) viewp_w->dval[0], (float) viewp_h->dval[0],
		(float) camera_x->dval[0], (float) camera_y->dval[0], (float) camera_z->dval[0], scene->sval[0], out_file->sval[0], ocl_plat->ival[0]);

exit:
	/* deallocate each non-null entry in argtable[] */
	arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

	return exitcode;
}
