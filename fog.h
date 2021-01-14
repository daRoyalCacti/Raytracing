
#ifndef RAYTRACER_FOG_H
#define RAYTRACER_FOG_H

//https://www.astro.umd.edu/~jph/HG_note.pdf
//https://blog.demofox.org/2014/06/22/analytic-fog-density/
//https://computergraphics.stackexchange.com/questions/227/how-are-volumetric-effects-handled-in-raytracing
bool basic_fog(const ray start, const vec3 end, ray &scattered) {
    const double lambda = 0.05;  //determines how often particles scatter - 'density of the fog'
    const double g1 = 0.3, g2 = 0.4;   //determines how particles scatter
    const auto length = (start.orig-end).length();

    const auto pos = rand_exp(lambda);

    if (pos > length)   //the ray should scatter after the ray has collided
        return false;

    const auto theta = rand_Henyey_Greensteing(g1);
    const auto phi = rand_Henyey_Greensteing(g2);
    scattered = ray(start.at(pos), vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(phi)), start.tm + pos);
    return true;
}

bool no_fog(const ray start, const vec3 end, ray &scattered) {
    return false;
}

#endif //RAYTRACER_FOG_H
