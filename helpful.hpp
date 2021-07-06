#pragma once

#include <limits>
#include <numeric>
#include <algorithm>

//common constants
constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;
constexpr double two_pi = 2*pi;

//utility functions
inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

template<typename T>
inline double mean(const T& vals) {
    return std::accumulate(vals.begin(), vals.end(), 0.0) / vals.size();
}

//should make is so this doesn't requre a double to be returned (TODO)
template<typename T>
inline double sum(const T& vals) {
    return std::accumulate(vals.begin(), vals.end(), 0.0);
}

template<typename T>
[[maybe_unused]] inline double st_dev(const T& vals) {
    const double mean = std::accumulate(vals.begin(), vals.end(), 0.0) / vals.size();
    const double sq_sum = std::inner_product( vals.begin(), vals.end(), vals.begin(), 0.0,
                                              [](const double &x, const double &y){return x+y; },
                                              [mean](const double &x, const double &y) {return (x-mean)*(y-mean);}   );
    return sqrt(sq_sum / (vals.size() - 1));
}

template<typename T>
inline double st_dev(const T& vals, const double mean) {
    const double sq_sum = std::inner_product( vals.begin(), vals.end(), vals.begin(), 0.0,
                                              [](const double &x, const double &y){return x+y; },
                                              [mean](const double &x, const double &y) {return (x-mean)*(y-mean);}   );
    return sqrt(sq_sum / (vals.size() - 1));
}

template<typename T>
inline auto min_arr(const T& vals) {
    return *std::min_element(vals.begin(), vals.end());
}

template<typename T>
inline auto max_arr(const T& vals) {
    return *std::max_element(vals.begin(), vals.end());
}

//could make for efficient - https://riptutorial.com/cplusplus/example/19185/using-std--nth-element-to-find-the-median--or-other-quantiles- (TODO)
template<typename T>
inline auto median(T vals) {
    std::sort(vals.begin(), vals.end());
    const auto size = vals.size();
    const bool is_even = (size % 2) == 0;
    if (is_even) {
        return (vals[size/2] + vals[size/2 - 1]) / 2;
    } else {
        return vals[size/2];
    }
}