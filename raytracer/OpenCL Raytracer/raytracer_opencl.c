#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raytracer_opencl.h"
#include "common.h"
#include "benchmark.h"

static char * read_kernel(const char *file_name) {

	FILE *file = fopen(file_name, "rb");
	if (!file) {
		fprintf(stderr, "Failed to open file '%s'\n", file_name);
		exit(-1);
	}

	if (fseek(file, 0, SEEK_END)) {
		fprintf(stderr, "Failed to seek file '%s'\n", file_name);
		exit(-1);
	}

	long size = ftell(file);
	if (size == 0) {
		fprintf(stderr, "Failed to check position on file '%s'\n", file_name);
		exit(-1);
	}

	rewind(file);

	char *src = (char *)malloc(sizeof(char) * size + 1);
	if (!src) {
		fprintf(stderr, "Failed to allocate memory for file '%s'\n", file_name);
		exit(-1);
	}

	fprintf(stderr, "Reading file '%s' (size %ld bytes)\n", file_name, size);
	size_t res = fread(src, 1, sizeof(char) * size, file);
	if (res != sizeof(char) * size) {
		fprintf(stderr, "Failed to read file '%s' (read %ld)\n", file_name, res);
		exit(-1);
	}
	src[size] = '\0'; /* NULL terminated */

	fclose(file);

	return src;
}

int initialize_openCL(int deviceType, int width, int height, char* defines, Primitive * primitive_list, int n_primitives, Pixel * out_pixels, int ocl_plat)
{
    cl_int status = 0;
    size_t deviceListSize;

    /*
     * Have a look at the available platforms and pick either
     * the AMD one if available or a reasonable default.
     */

    cl_uint numPlatforms;
    cl_platform_id platform = NULL;
    status = clGetPlatformIDs(0, NULL, &numPlatforms);
    if(status != CL_SUCCESS)
    {
        fprintf(stderr,"Error: Getting Platforms. (clGetPlatformsIDs)\n");
        return 1;
    }
    
    if(numPlatforms > 0)
    {
        cl_platform_id* platforms = new cl_platform_id[numPlatforms];
        status = clGetPlatformIDs(numPlatforms, platforms, NULL);
        if(status != CL_SUCCESS)
        {
            fprintf(stderr,"Error: Getting Platform Ids. (clGetPlatformsIDs)\n");
            return 1;
        }
        for(unsigned int i=0; i < numPlatforms; ++i)
        {
            char pbuff[100];
            status = clGetPlatformInfo(
                        platforms[i],
                        CL_PLATFORM_VENDOR,
                        sizeof(pbuff),
                        pbuff,
                        NULL);
            if(status != CL_SUCCESS)
            {
                fprintf(stderr,"Error: Getting Platform Info.(clGetPlatformInfo)\n");
                return 1;
            }

			// added by Mark ///////////////////////////////////////
			fprintf(stderr, "OpenCL Platform %d: %s\n", i, pbuff);
			// added by Mark ///////////////////////////////////////

            platform = platforms[i];
			if(i == ocl_plat)
            {
                fprintf(stderr, "Using platform %d.\n", i);
				break;
            }
        }
        delete platforms;
    }

    if(NULL == platform)
    {
        fprintf(stderr,"NULL platform found so Exiting Application.");
        return 1;
    }

    /*
     * If we could find our platform, use it. Otherwise use just available platform.
     */
    cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 };

    /////////////////////////////////////////////////////////////////
    // Create an OpenCL context
    /////////////////////////////////////////////////////////////////
	context = clCreateContextFromType(cps, 
									  deviceType,
                                      NULL, 
                                      NULL, 
                                      &status);
    if(status != CL_SUCCESS) 
    {  
        fprintf(stderr,"Error: Creating Context. (clCreateContextFromType)\n");
        return 1; 
    }

    /* First, get the size of device list data */
    status = clGetContextInfo(context, 
                              CL_CONTEXT_DEVICES, 
                              0, 
                              NULL, 
                              &deviceListSize);
    if(status != CL_SUCCESS) 
    {  
        fprintf(stderr,
            "Error: Getting Context Info \
            (device list size, clGetContextInfo)\n");
        return 1;
    }

    /////////////////////////////////////////////////////////////////
    // Detect OpenCL devices
    /////////////////////////////////////////////////////////////////
    devices = (cl_device_id *)malloc(deviceListSize);
    if(devices == 0)
    {
       fprintf(stderr,"Error: No devices found.\n");
        return 1;
    }

    /* Now, get the device list data */
    status = clGetContextInfo(
                 context, 
                 CL_CONTEXT_DEVICES, 
                 deviceListSize, 
                 devices, 
                 NULL);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,
            "Error: Getting Context Info \
            (device list, clGetContextInfo)\n");
        return 1;
    }


// added by Mark ///////////////////////////////////////

	// Print devices list 
	for (int i = 0; i < deviceListSize / sizeof(cl_device_id); ++i) {
		cl_device_type type = 0;
		status = clGetDeviceInfo(devices[i],
				CL_DEVICE_TYPE,
				sizeof(cl_device_type),
				&type,
				NULL);
		if (status != CL_SUCCESS) {
			fprintf(stderr, "Failed to get OpenCL device info: %d\n", status);
			exit(-1);
		}

		char *stype;
		switch (type) {
			case CL_DEVICE_TYPE_ALL:
				stype = "TYPE_ALL";
				break;
			case CL_DEVICE_TYPE_DEFAULT:
				stype = "TYPE_DEFAULT";
				break;
			case CL_DEVICE_TYPE_CPU:
				stype = "TYPE_CPU";
				break;
			case CL_DEVICE_TYPE_GPU:
				stype = "TYPE_GPU";
				break;
			default:
				stype = "TYPE_UNKNOWN";
				break;
		}
		fprintf(stderr, "OpenCL Device %d: Type = %s\n", i, stype);

		char buf[256];
		status = clGetDeviceInfo(devices[i],
				CL_DEVICE_NAME,
				sizeof(char[256]),
				&buf,
				NULL);
		if (status != CL_SUCCESS) {
			fprintf(stderr, "Failed to get OpenCL device info: %d\n", status);
			exit(-1);
		}

		fprintf(stderr, "OpenCL Device %d: Name = %s\n", i, buf);

		cl_uint units = 0;
		status = clGetDeviceInfo(devices[i],
				CL_DEVICE_MAX_COMPUTE_UNITS,
				sizeof(cl_uint),
				&units,
				NULL);
		if (status != CL_SUCCESS) {
			fprintf(stderr, "Failed to get OpenCL device info: %d\n", status);
			exit(-1);
		}

		fprintf(stderr, "OpenCL Device %d: Compute units = %u\n", i, units);

		size_t gsize = 0;
		status = clGetDeviceInfo(devices[i],
				CL_DEVICE_MAX_WORK_GROUP_SIZE,
				sizeof(size_t),
				&gsize,
				NULL);
		if (status != CL_SUCCESS) {
			fprintf(stderr, "Failed to get OpenCL device info: %d\n", status);
			exit(-1);
		}

		fprintf(stderr, "OpenCL Device %d: Max. work group size = %d\n", i, (unsigned int)gsize);
	}
// added by Mark ///////////////////////////////////////


    /////////////////////////////////////////////////////////////////
    // Create an OpenCL command queue
    /////////////////////////////////////////////////////////////////
    command_queue = clCreateCommandQueue(
                       context, 
                       devices[0], 
                       0, 
                       &status);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: Creating Command Queue. (clCreateCommandQueue)\n");
        return 1;
    }

    /////////////////////////////////////////////////////////////////
    // Create OpenCL memory buffers
    /////////////////////////////////////////////////////////////////
    primitive_buffer = clCreateBuffer(
                      context, 
                      CL_MEM_READ_ONLY,
                      sizeof(Primitive) * n_primitives,
                      NULL,
                      &status);

    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: clCreateBuffer (primitive_buffer): %d\n", status);
        return 1;
    }

	status = clEnqueueWriteBuffer(
					   command_queue,
					   primitive_buffer,
					   CL_FALSE,
					   0,
					   sizeof(Primitive) * n_primitives,
					   primitive_list,
					   0,
					   NULL,
					   NULL);

	if (status != CL_SUCCESS)
	{
		fprintf(stderr,"Error: Failed to write the primitive_buffer: %d\n", status);
		return 1;
	}

    pixel_buffer = clCreateBuffer(
                       context, 
                       CL_MEM_WRITE_ONLY,
                       sizeof(Pixel) * width * height,
                       NULL, 
                       &status);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: clCreateBuffer (pixel_buffer)\n");
        return 1;
    }

	status = clEnqueueWriteBuffer(
					   command_queue,
					   pixel_buffer,
					   CL_FALSE,
					   0,
					   sizeof(Pixel) * width * height,
					   out_pixels,
					   0,
					   NULL,
					   NULL);

	if (status != CL_SUCCESS)
	{
		fprintf(stderr,"Error: Failed to write the pixel_buffer: %d\n", status);
		return 1;
	}


    /////////////////////////////////////////////////////////////////
    // Load CL file, build CL program object, create CL kernel object
    /////////////////////////////////////////////////////////////////
    const char * filename  = "raytracer_kernel.cl";
    const char * source    = read_kernel(filename);
    size_t sourceSize[]    = { strlen(source) };

    program = clCreateProgramWithSource(
                  context, 
                  1, 
                  &source,
                  sourceSize,
                  &status);
    if(status != CL_SUCCESS) 
    { 
      fprintf(stderr,
               "Error: Loading Binary into cl_program \
               (clCreateProgramWithBinary)\n");
      return 1;
    }

    /* create a cl program executable for all the devices specified */
    status = clBuildProgram(program, 1, devices, defines, NULL, NULL);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: Building Program (clBuildProgram): %d\n", status);
		size_t len;
		char *buffer;
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
		buffer = (char *) calloc(1, len);
		clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, len, buffer, NULL);
		fprintf(stderr,"%s\n", buffer);
        return 1; 
    }

    /* get a kernel object handle for a kernel with the given name */
    raytracer_kernel = clCreateKernel(program, "raytracer_kernel", &status);
    if(status != CL_SUCCESS) 
    {  
        fprintf(stderr,"Error: Creating Kernel from program. (clCreateKernel)\n");
        return 1;
    }

    return 0;
}

int run_openCL_kernel(int width, int height, float camera_x, float camera_y, float camera_z,
	float viewport_width, float viewport_height, int workgroup_size, int n_primitives, Pixel * out_pixels)
{
    cl_int   status;
    cl_uint maxDims;
    cl_event events[2];
    size_t globalThreads[1];
    size_t localThreads[1];
    size_t maxWorkGroupSize;
    size_t maxWorkItemSizes[3];

    /**
    * Query device capabilities. Maximum 
    * work item dimensions and the maximmum
    * work item sizes
    */ 
    status = clGetDeviceInfo(
        devices[0], 
        CL_DEVICE_MAX_WORK_GROUP_SIZE, 
        sizeof(size_t), 
        (void*)&maxWorkGroupSize, 
        NULL);
    if(status != CL_SUCCESS) 
    {  
        fprintf(stderr,"Error: Getting Device Info. (clGetDeviceInfo)\n");
        return 1;
    }
    
    status = clGetDeviceInfo(
        devices[0], 
        CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, 
        sizeof(cl_uint), 
        (void*)&maxDims, 
        NULL);
    if(status != CL_SUCCESS) 
    {  
        fprintf(stderr,"Error: Getting Device Info. (clGetDeviceInfo)\n");
        return 1;
    }

    status = clGetDeviceInfo(
        devices[0], 
        CL_DEVICE_MAX_WORK_ITEM_SIZES, 
        sizeof(size_t)*maxDims,
        (void*)maxWorkItemSizes,
        NULL);
    if(status != CL_SUCCESS) 
    {  
        fprintf(stderr,"Error: Getting Device Info. (clGetDeviceInfo)\n");
        return 1;
    }
    
    globalThreads[0] = width * height;

	if (globalThreads[0] % workgroup_size != 0)
		globalThreads[0] = (globalThreads[0] / workgroup_size + 1) * workgroup_size;

    localThreads[0]  = workgroup_size;

    if(localThreads[0] > maxWorkGroupSize ||
        localThreads[0] > maxWorkItemSizes[0])
    {
        fprintf(stderr,"Unsupported: Device does not support requested number of work items.");
        return 1;
    }

    /*** Set appropriate arguments to the kernel ***/
    /* the pixel buffer */
    status = clSetKernelArg(
                    raytracer_kernel, 
                    0, 
                    sizeof(cl_mem), 
                    (void *) & pixel_buffer);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: Setting kernel argument. (pixel_buffer)\n");
        return 1;
    }
	
	/* width */
    status = clSetKernelArg(
                    raytracer_kernel, 
                    1, 
                    sizeof(cl_uint), 
                    (void *) & width);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: Setting kernel argument. (width)\n");
        return 1;
    }
	
	/* height */
    status = clSetKernelArg(
                    raytracer_kernel, 
                    2, 
                    sizeof(cl_uint), 
                    (void *) & height);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: Setting kernel argument. (height)\n");
        return 1;
    }

	/* camera */
    status = clSetKernelArg(
                    raytracer_kernel, 
                    3, 
                    sizeof(cl_float), 
                    (void *) & camera_x);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: Setting kernel argument. (camera_x) %d\n", status);
        return 1;
    }

	status = clSetKernelArg(
                    raytracer_kernel, 
                    4, 
                    sizeof(cl_float), 
                    (void *) & camera_y);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: Setting kernel argument. (camera_y)\n");
        return 1;
    }

	status = clSetKernelArg(
                    raytracer_kernel, 
                    5, 
                    sizeof(cl_float), 
                    (void *) & camera_z);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: Setting kernel argument. (camera_z)\n");
        return 1;
    }

	/* viewport size */
	status = clSetKernelArg(
                    raytracer_kernel, 
                    6, 
                    sizeof(cl_float), 
                    (void *) & viewport_width);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: Setting kernel argument. (viewport_width)\n");
        return 1;
    }

	status = clSetKernelArg(
                    raytracer_kernel, 
                    7, 
                    sizeof(cl_float), 
                    (void *) & viewport_height);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: Setting kernel argument. (viewport_height)\n");
        return 1;
    }
	
	/* the primitive buffer */
    status = clSetKernelArg(
                    raytracer_kernel, 
                    8, 
                    sizeof(cl_mem), 
                    (void *)& primitive_buffer);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: Setting kernel argument. (primitive_buffer)\n");
        return 1;
    }

	/* number of primitives */
    status = clSetKernelArg(
                    raytracer_kernel, 
                    9, 
                    sizeof(cl_uint), 
                    (void *)& n_primitives);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,"Error: Setting kernel argument. (n_primitives) : %d\n", status);
        return 1;
    }

	/* local primitive */
	status = clSetKernelArg(
					raytracer_kernel,
					10,
					sizeof(Primitive) * n_primitives,
					NULL);

	if (status != CL_SUCCESS)
	{
		fprintf(stderr,"Error: Setting kernel argument. (local_prims)\n", status);
		return 1;
	}
    /* 
     * Enqueue a kernel run call.
     */
    status = clEnqueueNDRangeKernel(
                 command_queue,
                 raytracer_kernel,
                 1,
                 NULL,
                 globalThreads,
                 localThreads,
                 0,
                 NULL,
                 &events[0]);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,
            "Error: Enqueueing kernel onto command queue. \
            (clEnqueueNDRangeKernel)\n");
        return 1;
    }


    /* wait for the kernel call to finish execution */
    status = clWaitForEvents(1, &events[0]);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,
            "Error: Waiting for kernel run to finish. \
            (clWaitForEvents)\n");
        return 1;
    }

    status = clReleaseEvent(events[0]);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,
            "Error: Release event object. \
            (clReleaseEvent)\n");
        return 1;
    }

    /* Enqueue readBuffer*/
    status = clEnqueueReadBuffer(
                command_queue,
                pixel_buffer,
                CL_TRUE,
                0,
                width * height * sizeof(Pixel),
                out_pixels,
                0,
                NULL,
                &events[1]);
    
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr, 
            "Error: clEnqueueReadBuffer failed. \
             (clEnqueueReadBuffer)\n");

        return 1;
    }
    
    /* Wait for the read buffer to finish execution */
    status = clWaitForEvents(1, &events[1]);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,
            "Error: Waiting for read buffer call to finish. \
            (clWaitForEvents)\n");
        return 1;
    }
    
    status = clReleaseEvent(events[1]);
    if(status != CL_SUCCESS) 
    { 
        fprintf(stderr,
            "Error: Release event object. \
            (clReleaseEvent)\n");
        return 1;
    }

    return 0;
}

int cleanup_openCL(void)
{
    cl_int status;

    status = clReleaseKernel(raytracer_kernel);
    if(status != CL_SUCCESS)
    {
        fprintf(stderr,"Error: In clReleaseKernel \n");
        return 1; 
    }
    status = clReleaseProgram(program);
    if(status != CL_SUCCESS)
    {
        fprintf(stderr,"Error: In clReleaseProgram\n");
        return 1; 
    }
    status = clReleaseMemObject(primitive_buffer);
    if(status != CL_SUCCESS)
    {
        fprintf(stderr,"Error: In clReleaseMemObject (primitive_buffer)\n");
        return 1; 
    }
    status = clReleaseMemObject(pixel_buffer);
    if(status != CL_SUCCESS)
    {
        fprintf(stderr,"Error: In clReleaseMemObject (pixel_buffer)\n");
        return 1; 
    }
    status = clReleaseCommandQueue(command_queue);
    if(status != CL_SUCCESS)
    {
        fprintf(stderr,"Error: In clReleaseCommandQueue\n");
        return 1;
    }
    status = clReleaseContext(context);
    if(status != CL_SUCCESS)
    {
        fprintf(stderr,"Error: In clReleaseContext\n");
        return 1;
    }

    return 0;
}

int do_raytrace_ocl(Pixel * out_pixels, Primitive * prim_list, int n_primitives, int use_cpu, int depth, 
	int workgroup_size, int fast_norm, int buil_norm, int nati_sqrt, int buil_dot, int buil_len, int width, 
	int height, float camera_x, float camera_y, float camera_z, float viewport_width, float viewport_height, int ocl_plat)
{

	int deviceType;
	if (use_cpu > 0)
		deviceType = CL_DEVICE_TYPE_CPU;
	else
		deviceType = CL_DEVICE_TYPE_GPU;

	// build define list for options
	char def_string[100];
	strcpy(def_string, "-I. -D__OPENCL__ ");

	if (depth > 5)
	{
		fprintf(stderr,"Warning: Max tracedepth supported is 5. Using 5.");
		depth = 5;
	}

	// hardcoding this for now. change later maybe.
	switch(depth)
	{
	case 5:
		strcat(def_string, "-DD_TRACEDEPTH_5 ");
		break;
	case 4:
		strcat(def_string, "-DD_TRACEDEPTH_4 ");
		break;
	case 3:
		strcat(def_string, "-DD_TRACEDEPTH_3 ");
		break;
	case 2:
		strcat(def_string, "-DD_TRACEDEPTH_2 ");
		break;
	case 1:
		strcat(def_string, "-DD_TRACEDEPTH_1 ");
		break;
	case 0:
		strcat(def_string, "-DD_TRACEDEPTH_0 ");
		break;
	}

	if (fast_norm > 0)
		strcat(def_string, "-DD_FAST_NORMALIZE ");

	if (buil_norm > 0)
		strcat(def_string, "-DD_BUILTIN_NORMALIZE ");

	if (nati_sqrt > 0)
		strcat(def_string, "-DD_NATIVE_SQRT ");

	if (buil_dot > 0)
		strcat(def_string, "-DD_BUILTIN_DOT ");

	if (buil_len > 0)
		strcat(def_string, "-DD_BUILTIN_LEN ");

	// init openCL
	if (initialize_openCL(deviceType, width, height, def_string, prim_list, n_primitives, out_pixels, ocl_plat) == 1)
		return 1;

	double fstart = get_time();

	// run openCL
	if (run_openCL_kernel(width, height, camera_x, camera_y, camera_z, viewport_width, viewport_height, workgroup_size,
		n_primitives, out_pixels) == 1)
		return 1;

	double ftime = get_time();

	fprintf(stderr, "Done\n");
	fprintf(stderr, "Elapsed time: %f s\n", ftime - fstart);

	// cleanup openCL
	if (cleanup_openCL() == 1)
		return 1;

	return 0;
}
