/***************************************************************************
* Summon
* Matt Rasmussen
* types.h
*
***************************************************************************/


#ifndef VIS_TYPES_H
#define VIS_TYPES_H

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
   
   float Norm() {
      return sqrt(x*x + y*y);
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




typedef Vertex2<float> Vertex2f;
typedef Vertex2<int> Vertex2i;

/*
template <class T>
class Size
{
public:
   Size(T w, T h) :
      w(w), h(h) {}
   T w;
   T h;
};

typedef Size<float> Sizef;
typedef Size<int> Sizei;
*/

}

#endif
