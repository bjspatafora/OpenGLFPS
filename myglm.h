#ifndef MYGLM_H
#define MYGLM_H

#include <iostream>
#include <iomanip>
#include <sstream>

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/string_cast.hpp>

template < typename T, int R, int C, int DEC_PLACES=6, int SPACING=1 >
void pprint_mat(const T & m)
{
    int widths[C] = {0};
    for (int c = 0; c < C; ++c)
    {
        for (int r = 0; r < R; ++r)
        {
            std::ostringstream sscout;
            sscout << std::fixed << std::setprecision(DEC_PLACES)
                   << m[r][c];
            int len = sscout.str().length();
            if (len > widths[c]) widths[c] = len;
        }
    }
    std::string x = "";
    std::cout << '[';
    for (int r = 0; r < R; ++r)
    {
        std::cout << x; x = " ";
        std::cout << '[';
        for (int c = 0; c < C; ++c)
        {
            std::cout << std::setw(widths[c])
                      << std::fixed << std::setprecision(DEC_PLACES)
                      << m[c][r];
            if (c < C - 1) std::cout << std::setw(SPACING) << ' ';
        }
        std::cout << ']';
        if (r == R - 1) std::cout << ']';
        if (r < R - 1) std::cout << '\n';
    }
}
    
template < typename T, int R, int C=1, int DEC_PLACES=6, int SPACING=1 >
void pprint_vec(const T & m)
{
    int widths[C] = {0};
    for (int c = 0; c < C; ++c)
    {
        for (int r = 0; r < R; ++r)
        {
            std::ostringstream sscout;
            sscout << std::fixed << std::setprecision(DEC_PLACES)
                   << m[r];
            int len = sscout.str().length();
            if (len > widths[c]) widths[c] = len;
        }
    }
    std::string x = "";
    std::cout << '[';
    for (int r = 0; r < R; ++r)
    {
        std::cout << x; x = " ";
        std::cout << '[';
        for (int c = 0; c < C; ++c)
        {
            std::cout << std::setw(widths[c])
                      << std::fixed << std::setprecision(DEC_PLACES)
                      << m[r];
            if (c < C - 1) std::cout << std::setw(SPACING) << ' ';
        }
        std::cout << ']';
        if (r == R - 1) std::cout << ']';
        if (r < R - 1) std::cout << '\n';
        }
}

//============================================================================
// Printing for matrices
//============================================================================
inline
std::ostream & operator<<(std::ostream & cout, const glm::mat4 & m)
{
//pprint_mat< glm::mat4, 4, 4 >(m);
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::mat3 & m)
{
//pprint_mat< glm::mat3, 3, 3 >(m);
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::mat2 & m)
{
//pprint_mat< glm::mat2, 2, 2 >(m);
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::mat2x3 & m)
{
//pprint_mat< glm::mat2, 3, 2 >(m);
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::mat3x2 & m)
{
//pprint_mat< glm::mat2, 2, 3 >(m);
    cout << glm::to_string(m);
    return cout;
}

// No mat1x1, mat1x2, mat1x3, mat1x4, etc.
//============================================================================
// Printing for vectors
//============================================================================

inline
std::ostream & operator<<(std::ostream & cout, const glm::dvec2 & m)
{
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::vec2 & m)
{
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::ivec2 & m)
{
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::dvec3 & m)
{
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::vec3 & m)
{
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::ivec3 & m)
{
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::dvec4 & m)
{
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::vec4 & m)
{
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::ivec4 & m)
{
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::bvec2 & m)
{
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::bvec3 & m)
{
    cout << glm::to_string(m);
    return cout;
}

inline
std::ostream & operator<<(std::ostream & cout, const glm::bvec4 & m)
{
    cout << glm::to_string(m);
    return cout;
}

//============================================================================
// Scalar products when scalar is an int.
//============================================================================
inline
glm::vec4 operator*(int c, const glm::vec4 & v)
{
    return float(c) * v;
}

inline
glm::vec3 operator*(int c, const glm::vec3 & v)
{
    return float(c) * v;
}

inline
glm::vec2 operator*(int c, const glm::vec2 & v)
{
    return float(c) * v;
}

inline
glm::vec4 operator*(const glm::vec4 & v, int c)
{
    return float(c) * v;
}

inline
glm::vec3 operator*(const glm::vec3 & v, int c)
{
    return float(c) * v;
}

inline
glm::vec2 operator*(const glm::vec2 & v, int c)
{
    return float(c) * v;
}

//============================================================================
// Scalar divisions when scalar is an int.
//============================================================================
inline
glm::vec4 operator/(const glm::vec4 & v, int c)
{
    return float(1.0 / c) * v;
}

inline
glm::vec3 operator/(const glm::vec3 & v, int c)
{
    return float(1.0 / c) * v;
}

inline
glm::vec2 operator/(const glm::vec2 & v, int c)
{
    return float(1.0 / c) * v;
}

inline
const glm::vec4 & operator/=(glm::vec4 & v, int c)
{
    v /= float(c);
    return v;
}

//============================================================================
// Approximate comparison
//============================================================================
bool approxEqual(const glm::vec2 & x, const glm::vec2 & y)
{
    return glm::all(glm::epsilonEqual(x - y, glm::vec2(),
                                      glm::epsilon< float >()));
}

bool approxEqual(const glm::vec3 & x, const glm::vec3 & y)
{
    return glm::all(glm::epsilonEqual(x - y, glm::vec3(),
                                      glm::epsilon< float >()));
}

bool approxEqual(const glm::vec4 & x, const glm::vec4 & y)
{
    return glm::all(glm::epsilonEqual(x - y, glm::vec4(),
                                      glm::epsilon< float >()));
}

bool approxEqual(const glm::mat2 & x, const glm::mat2 & y)
{
    return glm::all(glm::epsilonEqual(x[0] - y[0], glm::vec2(),
                                      glm::epsilon< float >()))
        && glm::all(glm::epsilonEqual(x[1] - y[1], glm::vec2(),
                                      glm::epsilon< float >()));
}

bool approxEqual(const glm::mat3 & x, const glm::mat3 & y)
{
    return glm::all(glm::epsilonEqual(x[0] - y[0], glm::vec3(),
                                      glm::epsilon< float >()))
        && glm::all(glm::epsilonEqual(x[1] - y[1], glm::vec3(),
                                      glm::epsilon< float >()))
        && glm::all(glm::epsilonEqual(x[2] - y[2], glm::vec3(),
                                      glm::epsilon< float >()));
}

bool approxEqual(const glm::mat4 & x, const glm::mat4 & y)
{
    return glm::all(glm::epsilonEqual(x[0] - y[0], glm::vec4(),
                                      glm::epsilon< float >()))
        && glm::all(glm::epsilonEqual(x[1] - y[1], glm::vec4(),
                                      glm::epsilon< float >()))
        && glm::all(glm::epsilonEqual(x[2] - y[2], glm::vec4(),
                                      glm::epsilon< float >()))
        && glm::all(glm::epsilonEqual(x[3] - y[3], glm::vec4(),
                                      glm::epsilon< float >()));
}

#endif
