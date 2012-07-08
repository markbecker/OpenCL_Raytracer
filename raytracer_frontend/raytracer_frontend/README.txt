raytracer is a visual studio 2010 solution. only confirmed compilation on release + x64 settings. everything else probably wont compile.
raytracer_frontend is an Eclipse project folder, if you use netbeans figure it out yourself.

the raytracer exe + kernel + def.scn need to be in the execution directory of the frontend for it to work.

the java code is basically uncommented...

for test runs, you want to do the following:

FOR ALL RUNS, IMPORTANT: SET THE CAMERA Z to -7.0 AND CAMERA Y TO 0.25 OR THE DEFAULT SCENE WILL NOT RENDER PROPERLY

- OpenCL, CPU & GPU (I suggest installing intel's SDK if you have an intel CPU, use the platform option to choose it, it is a lot faster on intel CPU than AMD's)
    - WG size - 64, 128, 256, 512 (CPU ONLY, GPU max is 256 iirc)
    - you can do a tracedepth runthrough (0, 1, 2, 3, 4, 5), but dont bother running it at each WG size
    - you can turn built-in normalize, built-in dot, built-in len, and native sqrt on. on my computer they slow it down (!), this should definitely be mentioned

- No-OpenCL - there is no WG size with this so the only thing that affects it is tracedepth. do 0 through 5 if you want.

things that will affect the image itself:
- camera x/y/z, image size, viewport size

you can play with these if you want.