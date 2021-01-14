#ifndef RAYTRACER_PROBABILITY_H
#define RAYTRACER_PROBABILITY_H

#include <math.h>

inline double exp_cdf(double x, double lambda) {
    return 1 - exp(-lambda*x);
}

inline double rand_exp(double lambda) {
    return -1/lambda*log(random_double());
}

inline double Henyey_Greensteing_pdf(double g, double theta) {
    return 1/(4*M_PI) * (1-g*g)/( pow(1+g*g - 2*g*cos(theta), 3/2.0f));
}

double rand_Henyey_Greensteing(double g) {
    while (true) {
        auto x = random_double(0, 2 * M_PI);
        auto y = random_double(0, 0.48);
        if (y < Henyey_Greensteing_pdf(g, x)) {
            return x;
        }
    }
}



#endif //RAYTRACER_PROBABILITY_H
