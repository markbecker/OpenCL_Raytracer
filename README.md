<table border=0>
  <tr>
    <td valign="top" colspan="2"><h1>OpenCL_Raytracer</h1>
      A Raytracer utilizing OpenCL.<br />
      Written by Mark Becker, Cameron Brown
      <hr>
      <ul>
        <font size='4'><b>Project description</b></font>
        <ul>
          General-purpose computing on graphics processing units (GPGPU) is a relatively new field <br />
          that uses the graphical processing unit’s (GPU) processing architecture. GPUs are normally <br />
          used for computer graphic computation. We are using this many-core architecture to perform <br />
          computations on very large data sets using the GPU’s parallelization capabilities.<br />
          <br />
          We designed and developed a ray tracer that demonstrates the OpenCL framework. <br />
          A suite of benchmark tests were developed to compare relative run times. Benchmarks were <br />
          run on these systems using native C code on the CPU, and OpenCL using both the CPU <br />
          and GPU. The project had three main parts: building the ray tracer, utilizing the OpenCL <br />
          framework, creating the benchmarks.
        </ul>
        <br/>
        <font size='4'><b>Developed and tested on...</b></font>
        <ul>
          <li>Mark's Desktop:
            <ul>
              <li>OS - Windows 7 Professional 64-bit
              <li>CPU - Intel Core i5 760 @ 2.80GHz
              <li>MB - ASUS P7P55D-E Pro
              <li>VC - ATI Radeon HD 5750
            </ul>
        </ul>
        <ul>
          <li>Cameron's Desktop:
            <ul>
              <li>OS - Windows 7 Professional 64-bit
              <li>CPU - Intel Core i7 2500K @ 4ghz
              <li>VC - ATI Radeon HD 6950
            </ul>
        </ul>
      </ul>
      <hr>
      <ul>
        <font size='4'><b>What is a Ray tracer?</b></font>
        <ul>
          Ray tracing is a technique that generates an image by tracing the path of light through <br />
          pixels in an view plane and simulating the effects of its intersections with virtual objects.<br />
          Ray tracing is capable of producing an image of photo-realistic quality. usually higher than<br />
          that of scan-line rendering methods. Unfortunately this level of realism comes at a greater<br />
          computational cost.<br />
          <li><a href="http://en.wikipedia.org/wiki/Ray_tracing_(graphics)" target=blank>More info from Wikipedia</a>
        </ul>
        <br/>
        <font size='4'><b>What is OpenCL?</b></font>
        <ul>
          OpenCL (Open Computing Language) is an industry standard framework for programming computers<br />
          composed of a combination of CPUs, GPUs, and other processors. OpenCL includes a<br />
          language (based on C99) for writing kernels (functions that execute on OpenCL devices) and<br />
          APIs that are used to define and control the platforms. OpenCL provides parallel computing<br />
          using task-based and data-based parallelism.<br />
          <li><a href="http://en.wikipedia.org/wiki/OpenCL" target=blank>More info from Wikipedia</a>
        </ul>
      </ul>
      <hr>
      <ul>
        <font size='4'><b>Run and build details...</b></font>
        <ul>
          <li>Write up coming soon
        </ul>
        <br/>
        <font size='4'><b>System Requirements</b></font>
        <ul>
          <li>Windows OS
          <li>Visual Studio 2010 or above to compile
          <li>ATI Video card - Radeon 5500 or above
          <li>AMD's APP SDK installed and working
          <li>Intel's OpenCL SDK
        </ul>
      </ul>
      <hr></td>
  </tr>
  <tr>
    <td valign="top"> Environment Adjuster GUI:<br />
      <a href="https://github.com/markbecker/OpenCL_Raytracer/raw/master/screenshot1.png" target=blank> <img border=1 src="https://github.com/markbecker/OpenCL_Raytracer/raw/master/screenshot1.png" width="179" height="423" alt="Screenshot 1"/></a><br />
      <a href="https://github.com/markbecker/OpenCL_Raytracer/raw/master/screenshot1.png" target=blank>Click for larger</a><br />
      <br /></td>
    <td valign="top"> Output GUI:<br />
      <a href="https://github.com/markbecker/OpenCL_Raytracer/raw/master/screenshot3.png" target=blank> <img border=1 src="https://github.com/markbecker/OpenCL_Raytracer/raw/master/screenshot3.png" width="490" height="519" alt="Screenshot 1"/></a><br />
      <a href="https://github.com/markbecker/OpenCL_Raytracer/raw/master/screenshot3.png" target=blank>Click for larger</a><br />
      <br /></td>
  </tr>
  <tr>
    <td valign="top" colspan="2"> Primitive Adjuster GUI:<br />
      <a href="https://github.com/markbecker/OpenCL_Raytracer/raw/master/screenshot2.png" target=blank> <img border=1 src="https://github.com/markbecker/OpenCL_Raytracer/raw/master/screenshot2.png" width="660" height="390" alt="Screenshot 2"/></a><br />
      <a href="https://github.com/markbecker/OpenCL_Raytracer/raw/master/screenshot2.png" target=blank>Click for larger</a><br />
      <br /></td>
  </tr>
</table>
