/* CPU Raytracer - Mirrors OpenCL Kernel
 * For SE195B Project by:
 *	Cameron Brown
 *  Mark Becker
 *  Mike Sawaya
 *  Jason Allen
 * at SJSU, Fall 2011
 */

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common.h"
#include "benchmark.h"

cl_float4 soft_normalize(cl_float4 vec)
{
	float l = 1/sqrt(vec.s[0] * vec.s[0] + vec.s[1] * vec.s[1] + vec.s[2] * vec.s[2]);
	cl_float4 temp;
	temp.s[0] = vec.s[0] * l;
	temp.s[1] = vec.s[1] * l;
	temp.s[2] = vec.s[2] * l;
	return temp;
}

float soft_dot(cl_float4 vec_a, cl_float4 vec_b)
{
	return vec_a.s[0] * vec_b.s[0] + vec_a.s[1] * vec_b.s[1] + vec_a.s[2] * vec_b.s[2];
}

float soft_length(cl_float4 vec)
{
	return sqrt(vec.s[0] * vec.s[0] + vec.s[1] * vec.s[1] + vec.s[2] * vec.s[2]);
}

inline cl_float4 create_float4(float x, float y, float z, float w)
{
	cl_float4 temp;
	temp.s[0] = x;
	temp.s[1] = y;
	temp.s[2] = z;
	temp.s[3] = w;
	return temp;
}

inline cl_float4 vector_sub(cl_float4 vec_a, cl_float4 vec_b)
{
	return create_float4(vec_a.s[0] - vec_b.s[0], vec_a.s[1] - vec_b.s[1], vec_a.s[2] - vec_b.s[2], vec_a.s[3] - vec_b.s[3]);
}

inline cl_float4 vector_add(cl_float4 vec_a, cl_float4 vec_b)
{
	return create_float4(vec_a.s[0] + vec_b.s[0], vec_a.s[1] + vec_b.s[1], vec_a.s[2] + vec_b.s[2], vec_a.s[3] + vec_b.s[3]);
}

inline cl_float4 vector_mul(cl_float4 vec_a, cl_float4 vec_b)
{
	return create_float4(vec_a.s[0] * vec_b.s[0], vec_a.s[1] * vec_b.s[1], vec_a.s[2] * vec_b.s[2], vec_a.s[3] * vec_b.s[3]);
}

inline cl_float4 vector_scalar_mul(cl_float4 vec, cl_float s)
{
	return create_float4(vec.s[0] * s, vec.s[1] * s, vec.s[2] * s, vec.s[3] * s);
}

inline cl_float4 vector_exp(cl_float4 vec)
{
	return create_float4(exp(vec.s[0]), exp(vec.s[1]), exp(vec.s[2]), exp(vec.s[3]));
}

#ifndef max
	#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
	#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

// Intersection method return values
#define HIT		 1		// Ray hit primitive
#define MISS	 0		// Ray missed primitive
#define INPRIM	-1		// Ray started inside primitive

#define EPSILON 0.001f

// ray queue to simulate recursion

#define PUSH_RAY(q, r, c, n) \
	if (c >= max_ray_count) \
		c = 0; \
	q[c++] = r; \
	n++; 

#define POP_RAY(q, r, c, n) \
	if (c >= max_ray_count) \
		c = 0; \
	r = q[c++]; \
	n--;
	
typedef enum raytype {
	ORIGIN = 0,
	REFLECTED = 1,
	REFRACTED = 2
} ray_type;

typedef struct {
	cl_float4 origin;
	cl_float4 direction;
	float weight;
	float depth;
	int origin_primitive;
	ray_type type;
	float r_index;
	Color transparency;
} Ray;

// functions
int plane_intersect(Primitive * p, Ray * ray, float * cumu_dist )
{
	float d = soft_dot( p->normal, ray->direction );
	if ( d != 0 )
	{
		float dist = - ( soft_dot( p->normal, ray->origin ) + p->depth ) / d;
		if (dist > 0 && dist < *cumu_dist)
		{
			*cumu_dist = dist;
			return HIT;
		}
	}
	return MISS;
}

int sphere_intersect(Primitive * p, Ray * ray, float * cumu_dist )
{
	cl_float4 v = vector_sub(ray->origin, p->center);
	float b = - soft_dot( v, ray->direction );
	float det = (b * b) - soft_dot(v, v) + p->sq_radius;
	int retval = MISS;
	if (det > 0)
	{
		det = sqrt(det);
		float i1 = b - det;
		float i2 = b + det;
		if (i2 > 0)
		{
			if (i1 < 0)
			{
				if (i2 < * cumu_dist)
				{
					*cumu_dist = i2;
					retval = INPRIM;
				}
			}
			else
			{
				if (i1 < * cumu_dist)
				{
					*cumu_dist = i1;
					retval = HIT;
				}
			}
		}
	}
	return retval;
}

int intersect(Primitive * p, Ray * ray, float * cumu_dist )
{
	switch (p->type)
	{
		case PLANE:
			return plane_intersect(p, ray, cumu_dist);
		case SPHERE:
			return sphere_intersect(p, ray, cumu_dist);
	}
	return MISS;
}

cl_float4 get_normal(Primitive * p, cl_float4 point)
{
	switch (p->type)
	{
		case PLANE:
			return (p->normal);
		case SPHERE:
			return vector_scalar_mul(vector_sub(point, p->center),p->r_radius);
	}
	return create_float4(0, 0, 0, 0);
}

int raytrace(Ray * a_ray, Color * a_acc, float * a_dist, cl_float4 * point_intersect, int * result, Primitive * primitives, int n_primitives)
{
	*a_dist = FLT_MAX;
	int prim_index = -1;

	// find nearest intersection
	for ( int s = 0; s < n_primitives; s++ )
	{
		int res;
		if (res = intersect(&primitives[s], a_ray, a_dist))
		{			
			prim_index = s;
			* result = res;
		}
	}
	// no hit
	if (prim_index == -1) return -1;
	// handle hit
	if (primitives[prim_index].is_light)
	{
		*a_acc = primitives[prim_index].m_color;
	}
	else
	{
		*point_intersect = vector_add(a_ray->origin, (vector_scalar_mul(a_ray->direction, (*a_dist))));
		// trace lights
		for (int l = 0; l < n_primitives; l++)
		{
			if (primitives[l].is_light)
			{
				// point light source shadows
				float shade = 1.0f;
				float L_soft_length = soft_length(vector_sub(primitives[l].center, *point_intersect));
				cl_float4 L = soft_normalize(vector_sub(primitives[l].center, *point_intersect));
				if (primitives[l].type == SPHERE)
				{
					Ray r;
					r.origin = vector_add(*point_intersect, vector_scalar_mul(L, EPSILON));
					r.direction = L;
					int s = 0;
					while ( s < n_primitives )
					{
						if (&primitives[s] != &primitives[l] && !primitives[s].is_light && intersect(&primitives[s], &r, &L_soft_length))
						{
							shade = 0;
						}
						s++;
					}
				}
				// Calculate diffuse shading
				cl_float4 N = get_normal(&primitives[prim_index], *point_intersect);
				if (primitives[prim_index].m_diff > 0)
				{
					float dot_prod = soft_dot( N, L );
					if (dot_prod > 0)
					{
						float diff = dot_prod * primitives[prim_index].m_diff * shade;
						* a_acc = vector_add(*a_acc, vector_scalar_mul(vector_mul(primitives[prim_index].m_color, primitives[l].m_color), diff));
					}
				}
				// Calculate specular shading
				if (primitives[prim_index].m_spec > 0)
				{
					cl_float4 V = a_ray->direction;
					cl_float4 R = vector_sub(L, vector_scalar_mul(N, soft_dot ( L, N ) * 1.5f));
					float dot_prod = soft_dot ( V, R );
					if (dot_prod > 0)
					{
						float spec = pow( dot_prod, 20 ) * primitives[prim_index].m_spec * shade;
						* a_acc = vector_add(*a_acc, vector_scalar_mul(primitives[l].m_color, spec));
					}
				}
			}
		}
	}

	return prim_index;
}

int do_raytrace_no_ocl(Pixel * out_pixels, Primitive * prim_list, int n_primitives, int depth,
	int image_width, int image_height, float camera_x, float camera_y, float camera_z, float viewport_width, 
	float viewport_height)
{
		fprintf(stderr, "Starting NO-OCL raytrace..\n");
		double fstart = get_time();
		// Our viewport size can be different than the image size. This lets us calculate
		// the stepping within the viewport relative to the stepping within the image.
		// IE with a viewport width of 6.0f and an image width of 800, each pixel is 1/800 of 6.0f 
		// or 0.0075f.
		const float dx = viewport_width / image_width; // x stepping, left -> right
		const float dy = -viewport_height / image_height; // y stepping, top -> bottom
		// loop through every pixel
		for (int y = 0; y < image_height; y++)
		{
			for (int x = 0; x < image_width; x++)
			{
				const float sx = -(viewport_width / 2.0f) + x * dx; // this pixel's viewport x
				const float sy = (viewport_height / 2.0f) + y * dy; // this pixel's viewport y

				// Initializes the ray queue. OpenCL has no support for recursion, so recursive ray tracing calls
				// were replaced by a queue of rays that is processed in sequence. Since the recursive calls were
				// additive, this works.

				// note: we are trying to mirror the OpenCL code so we do not use recursion here
				int max_ray_count = (int) pow(2.0, depth);

				Ray * queue;
		
				queue = (Ray *) malloc(sizeof(Ray) * max_ray_count);

				if (queue == NULL)
				{
					fprintf(stderr,"Error: Failed to allocate queue memory.\n");
					return 0;
				}

				memset(queue, 0, sizeof(Ray) * max_ray_count);
				int rays_in_queue = 0;
				int front_ray_ptr = 0;
				int back_ray_ptr = 0;

				cl_float4 camera = create_float4(camera_x, camera_y, camera_z, 0);
				Color acc = create_float4(0, 0, 0, 0);

				// We use 3x supersampling to smooth out the edges in the image. This means each pixel actually
				// fires 9 initial rays, plus the recursion and refraction.
				for (int tx = -1; tx < 2; tx++ )
				{
					for (int ty = -1; ty < 2; ty++ )
					{
						// Create initial ray.
						cl_float4 dir = soft_normalize( vector_sub(create_float4(sx + dx * (tx / 2.0f), sy + dy * (ty / 2.0f), 0, 0), camera));
						Ray r;
						r.origin = camera;
						r.direction = dir;
						r.weight = 1.0f;
						r.depth = 0;
						r.origin_primitive = -1;
						r.type = ORIGIN;
						r.r_index = 1.0f;
						r.transparency = create_float4(1, 1, 1, 0);

						// Populate queue and start the processing loop.
						PUSH_RAY(queue, r, back_ray_ptr, rays_in_queue)

						while (rays_in_queue > 0)
						{
							float dist;
							Ray cur_ray;
							POP_RAY(queue, cur_ray, front_ray_ptr, rays_in_queue)
							Color ray_col = create_float4(0, 0, 0, 0);
							cl_float4 point_intersect;
							int result;
							// raytrace performs the actual tracing and returns useful information
							int prim_index = raytrace( &cur_ray, &ray_col, &dist, &point_intersect, &result, prim_list, n_primitives);
							// reflected/refracted rays have different modifiers on the color of the object
							switch ( cur_ray.type )
							{
								case ORIGIN:
									acc = vector_add(acc, vector_scalar_mul(ray_col,  cur_ray.weight));
									break;
								case REFLECTED:
									acc = vector_add(acc,
											vector_scalar_mul(
												vector_mul(vector_mul(ray_col, prim_list[cur_ray.origin_primitive].m_color),
													 cur_ray.transparency), cur_ray.weight));
									break;
								case REFRACTED:
									acc = vector_add(acc, vector_scalar_mul(vector_mul(ray_col, cur_ray.transparency), cur_ray.weight));
									break;
							}
							// handle reflection & refraction
							if (cur_ray.depth < depth)
							{
								// reflection
								float refl = prim_list[prim_index].m_refl;
								if (refl > 0.0f)
								{
									cl_float4 N = get_normal(&prim_list[prim_index], point_intersect);
									cl_float4 R = vector_sub(cur_ray.direction, vector_scalar_mul(N, 2.0f * soft_dot( cur_ray.direction, N )));
									Ray new_ray;
									new_ray.origin = vector_add(point_intersect, vector_scalar_mul(R, EPSILON));
									new_ray.direction = R;
									new_ray.depth = cur_ray.depth + 1;
									new_ray.weight = refl * cur_ray.weight;
									new_ray.type = REFLECTED;
									new_ray.origin_primitive = prim_index;
									new_ray.r_index = cur_ray.r_index;
									new_ray.transparency = cur_ray.transparency;
									PUSH_RAY(queue, new_ray, back_ray_ptr, rays_in_queue)
								}
								// refraction
								float refr = prim_list[prim_index].m_refr;
								if (refr > 0.0f)
								{
									float m_rindex = prim_list[prim_index].m_refr_index;
									float n = cur_ray.r_index / m_rindex;
									cl_float4 N = vector_scalar_mul(get_normal(&prim_list[prim_index], point_intersect), (float) result);
									float cosI = - soft_dot ( N, cur_ray.direction );
									float cosT2 = 1.0f - n * n * (1.0f - cosI * cosI);
									if (cosT2 > 0.0f)
									{
										cl_float4 T = vector_add(vector_scalar_mul(cur_ray.direction, n), vector_scalar_mul(N, (n * cosI - sqrt( cosT2 ))));
										Ray new_ray;
										new_ray.origin = vector_add(point_intersect, vector_scalar_mul(T, EPSILON));
										new_ray.direction = T;
										new_ray.depth = cur_ray.depth + 1;
										new_ray.weight = cur_ray.weight;
										new_ray.type = REFRACTED;
										new_ray.origin_primitive = prim_index;
										new_ray.r_index = m_rindex;
										new_ray.transparency = vector_mul(cur_ray.transparency, vector_exp(vector_scalar_mul(prim_list[prim_index].m_color, 0.15f * (-dist))));
										PUSH_RAY(queue, new_ray, back_ray_ptr, rays_in_queue)
									}
								}
							}
						}
					}
				}
				// Since we supersample 3x, we have to divide the total color by 9 to average it.
				cl_uchar4 temp;
				temp.s[0] = (cl_uchar) min(max(acc.s[0] * (256 / 9), 0.0f), 255.0f);
				temp.s[1] = (cl_uchar) min(max(acc.s[1] * (256 / 9), 0.0f), 255.0f);
				temp.s[2] = (cl_uchar) min(max(acc.s[2] * (256 / 9), 0.0f), 255.0f);
				temp.s[3] = 0;

				out_pixels[y * image_width + x] = temp;
				free(queue);
		}
	}

	double fend = get_time();
	fprintf(stderr, "Done\n");
	fprintf(stderr, "Elapsed time: %f s\n", fend - fstart);

	return 0;
}