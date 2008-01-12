/***************************************************************************
* Summon
* Matt Rasmussen
* Vertex.h
*
***************************************************************************/


#ifndef SUMMON_VERTEX_H
#define SUMMON_VERTEX_H

#include <math.h>

namespace Summon
{

template <class T>
class Vertex2
{
public:
    Vertex2(T x = 0, T y = 0)  :
        x(x), y(y) {}  
   
    Vertex2(const Vertex2<T> &other) :
        x(other.x), y(other.y) {}
   
    template <class T2>
    Vertex2(const Vertex2<T> &v) :
        x(T(v.x)), y(T(v.y)) {}
   
    float Norm() const {
        return sqrt(x*x + y*y);
    }
   
    void Normalize() 
    {
        float norm = sqrt(x*x + y*y);
        x /= norm;
        y /= norm;
    }
   
    template <class T2>
    Vertex2<T> operator= (Vertex2<T2> v) {
          x = (T)v.x;
        y = (T)v.y;
        return Vertex2<T>(x, y);
    }
   
    T x;
    T y;
};

template <class T>
Vertex2<T> operator+ (const Vertex2<T> &u, const Vertex2<T> &v) {
    return Vertex2<T>(u.x + v.x, u.y + v.y);
}

template <class T>
Vertex2<T> operator- (const Vertex2<T> &u, const Vertex2<T> &v) {
    return Vertex2<T>(u.x - v.x, u.y - v.y);
}

template <class T>
Vertex2<T> operator* (const Vertex2<T> &v, T a) {
    return Vertex2<T>(v.x * a, v.y * a);
}

template <class T>
Vertex2<T> operator/ (const Vertex2<T> &v, T a) {
    return Vertex2<T>(v.x / a, v.y / a);
}

template <class T>
bool operator== (const Vertex2<T> &u, const Vertex2<T> &v) {
    return u.x == v.x && u.y == v.y;
}

template <class T>
bool operator!= (const Vertex2<T> &u, const Vertex2<T> &v) {
    return u.x != v.x || u.y != v.y;
}

// Vertex must be a unit vector
template <class T>
inline float vertex2angle(const Vertex2<T> &v)
{
    if (v.x >= 1.0)
        return 0.0;
    
    if (v.x <= -1.0)
        return M_PI;
        
    float angle = acos(v.x);
    
    if (v.y > 0)
        return angle;
    else
        return 2*M_PI - angle;
}


// define popular Vertex types
typedef Vertex2<float> Vertex2f;
typedef Vertex2<int> Vertex2i;

template <class T>
T DotProduct(const Vertex2<T> &a, const Vertex2<T> &b)
{
    return a.x*b.x + a.y*b.y;
}


template <class T>
bool InLeftHalfspace(const Vertex2<T> &a, const Vertex2<T> &b, 
                     const Vertex2<T> &p)  
{
    // define left if at a and facing towards b
     return (b.x-a.x) * (p.y-a.y) - (b.y-a.y) * (p.x-a.x) <= 0;
}


template <class T>
bool InTriangle(const Vertex2<T> &a, const Vertex2<T> &b, const Vertex2<T> &c, 
                const Vertex2<T> &pos)
{
    bool clockwise = InLeftHalfspace(b, a, c);
    if (clockwise)
    {
        return InLeftHalfspace(b, a, pos) && 
               InLeftHalfspace(c, b, pos) && 
               InLeftHalfspace(a, c, pos);
    } else {
        return InLeftHalfspace(a, b, pos) && 
               InLeftHalfspace(b, c, pos) && 
               InLeftHalfspace(c, a, pos);
    }
}

template <class T>
bool InQuad(const Vertex2<T> &a, const Vertex2<T> &b, 
            const Vertex2<T> &c, const Vertex2<T> &d,
            const Vertex2<T> &pos)
{
    bool clockwise = InLeftHalfspace(b, a, c);
    if (clockwise)
    {
        return InLeftHalfspace(b, a, pos) && 
               InLeftHalfspace(c, b, pos) && 
               InLeftHalfspace(d, c, pos) &&
               InLeftHalfspace(a, d, pos);
    } else {
        return InLeftHalfspace(a, b, pos) && 
               InLeftHalfspace(b, c, pos) && 
               InLeftHalfspace(c, d, pos) &&
               InLeftHalfspace(d, a, pos);
    }
}

} // namespace Summon

#endif
