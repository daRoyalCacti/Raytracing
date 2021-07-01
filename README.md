# Raytracing

This is a CPU path tracer based on; [Raytracing in one weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html), [Raytracing the next week](https://raytracing.github.io/books/RayTracingTheNextWeek.html), 
and [Raytracing the rest of your life](https://raytracing.github.io/books/RayTracingTheRestOfYourLife.html)
(a GPU implemention using CUDA was also done. See [Raytracing_GPU](https://github.com/daRoyalCacti/Raytracing_GPU)).
## From raytracing in one weekend
As such it has the following features
* Antialiasing (inherit to the path tracing method)  
* custom classes
    * custom vector 3 class
    * custom ray class
    * custom camera (including defocus blur using the [thin lens approximation](https://en.wikipedia.org/wiki/Thin_lens))
* ray intersections with
    * spheres
    * moving spheres (allows for motion blur)
    * rectangles
* Materials
    * Diffusive
    * Metal
    * Dielectric
    * Constant density medium (similar to smoke)
* Motion blur
* Bounding volume hierarchy using axis aligned bounding boxes (plans to upgrade this to [SAH](https://psgraphics.blogspot.com/2016/03/a-simple-sah-bvh-build.html))
* Image texture (and image texture mapping)
* Perlin noise (for generating textures are scenes)
* Positionable lights
* Transformation of instances (translation and rotation)
* Importance sampling
    * Currently only spheres and rectangles 
* Orthonormal basis generation
* Probability density classes

## Eigen
[Eigen 3](https://eigen.tuxfamily.org/index.php?title=Main_Page) was implemented to replace the vector 3 class as the original implementation was quite naive.
However it was found that eigen was slower and thus it was removed. 
This is also in a [stack overflow post](https://stackoverflow.com/questions/58071344/is-eigen-slow-at-multiplying-small-matrices) where it was found that eigen is slow for small matrices due to some additional overhead.
The eigen implementation can be seen in the [eigen branch](https://github.com/daRoyalCacti/Raytracing/tree/eigen)

## Participating medium using Henyey-Greenstein Phase function
Support was added for participating medium which scatters by the [Henyey-Greenstein Phase function](https://www.astro.umd.edu/~jph/HG_note.pdf)
It is an 1D phase function -- the most naturally occurring type ([pbrt](https://www.pbrt.org/) [section 11.2](https://www.pbr-book.org/3ed-2018/Volume_Scattering/Phase_Functions#PhaseHG)).
It is also isotropic (again see [pbrt](https://www.pbrt.org/) [section 11.2](https://www.pbr-book.org/3ed-2018/Volume_Scattering/Phase_Functions#PhaseHG)) which makes it particularly easy to use.
Although this comes at a const of not being entirely realistic (see [oceanoptics](https://www.oceanopticsbook.info/view/scattering/level-2/the-henyey-greenstein-phase-function))

The phase function has a constant parameter g, which can vary from -1 to 1.
Positive g gives forward scattering (with g=1 giving pure forward scattering - no change in direction), negative g gives backward scattering ang g=0 gives isotropic scattering (see [realistic image synthesis using photon mapping](https://www.amazon.com/Realistic-Image-Synthesis-Photon-Mapping/dp/1568811470)).
g should be closer to 1 for fog and closer to 0 for smoke (see [Realisitc ray tracing](https://www.amazon.com/Realistic-Tracing-Second-Peter-Shirley/dp/1568814615)).

A method was developed for generating collision times with media of non-constant density. 
This involves generating from the pdf developped [here](https://www.astro.umd.edu/~jph/HG_note.pdf) (also in the 'Explainations' folder).
This is based off the fact that exponential distributions are the only distribution that are [memoryless](https://en.wikipedia.org/wiki/Memoryless) and as such are the only candidated for generating collision times
Once the collision time has been determined, the phase function can be used to generate the scattered ray.  

The Henyey-Greenstein Phase function is a pdf of an angle.
This angle can be found by sample from the pdf using the [acceptance-rejection method](https://en.wikipedia.org/wiki/Rejection_sampling). 
The sampling region used can be seen in the [desmos link](https://www.desmos.com/calculator/84ypsmsy95).

This angle is between the input ray direction and the ouput ray direction (again see [pbrt](https://www.pbrt.org/) [section 11.2](https://www.pbr-book.org/3ed-2018/Volume_Scattering/Phase_Functions#PhaseHG)).
As such there are infinitely many possible rays to get from this angle.
The method used was to generate a random vector and then to rotate about that vector by the found angle.
The general way of doing this using [quaternions](https://www.3dgep.com/understanding-quaternions/#Rotations) was not used and instead the result from [kennesaw state univeristy](https://ksuweb.kennesaw.edu/~plaval//math4490/rotgen.pdf) was used.

## Triangle meshes
Triangle meshes are generally read from file.
To do this [assimp](https://assimp.org/) was used similarly to what is found at [learnopengl.com](https://learnopengl.com/Model-Loading/Model).
All triangle meshes are loaded into a bvh because there would rarely be a case where there would not be a performace boost from doing so due to most meshes having a high triangle count.

Triangle collisions we calculated using the [Moller-Trumbore intersection algorithm](https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm).
Texture coordinates were found using a [Barycentric coordinate system](https://en.wikipedia.org/wiki/Barycentric_coordinate_system),
as by the discussion given on [computer graphics stack exchange](https://computergraphics.stackexchange.com/questions/1866/how-to-map-square-texture-to-triangle).
These were computed efficiently using the method given in [Real-time collisoin detection](http://realtimecollisiondetection.net/), which the method being transcribed in [game dev stack exchange](https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates).

## Multithreading
Multi-threading was simply added using [open mp](https://www.openmp.org/)'s parallel for.
This is enough to maintain constant 100% CPU usage.

It is planned to add SIMD in the future.

## Images
All images created are available in [/images](https://github.com/daRoyalCacti/Raytracing_GPU/tree/master/images). Some noteworthy images are seen below.

<img src="https://raw.githubusercontent.com/daRoyalCacti/Raytracing/master/images/cornell_box.png" width="400" height="400" alt="cornell box">

<img src="https://raw.githubusercontent.com/daRoyalCacti/Raytracing/master/images/cornell_box_sphere.png" width="400" height="400" alt="cornell box with sphere">

<img src="https://raw.githubusercontent.com/daRoyalCacti/Raytracing/master/images/conell_box_foggy.png" width="400" height="400" alt="cornell box foggy">

<img src="https://raw.githubusercontent.com/daRoyalCacti/Raytracing/master/images/raytracing_in_one_weekend_main_image.png" width="800" height="450" alt="raytracing in one weekend main image">

<img src="https://raw.githubusercontent.com/daRoyalCacti/Raytracing/master/images/raytracing_the_next_week_main_image.png" width="400" height="400" alt="raytracing the next week main image">

