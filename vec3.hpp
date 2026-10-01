/*
 * vec3.hpp
 *
 * Written by Marcos Garcia <marcos.garcia@urjc.es>
 * Written by Jose Miguel Espadero <josemiguel.espadero@urjc.es>
 *
 * This code is written as example for the FMF class of the
 * Master Universitario en Informatica Grafica, Juegos y Realidad Virtual.
 * Its purpose is to be didactic and easy to understand, not hard optimized.
 *
 * This file provides a simple implementation for storage and usual operations with
 * 3D point in space. Coordinates are stored as double values.
 *
 */

#ifndef __vec3__
#define __vec3__

#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#endif

///Define _USE_MATH_DEFINES before including cmath to expose math constants as M_PI
#define _USE_MATH_DEFINES
#include <cmath>
#include <climits>
#include <cfloat>
#include <iostream>

#ifndef INFINITY
///A constant to be used as "infinite" value.
#define INFINITY std::numeric_limits<double>::infinity()
#endif

//Check if project is configured with EIGEN library support
#ifdef EIGEN3_FOUND
#include <Eigen/Core>
#endif


///Simple class to store and operate with 3D vector. Coordinates are stored as double values.
class vec3
{
public:

  ///X coordinate value
  double X;
  ///Y coordinate value
  double Y;
  ///Z coordinate value
  double Z;

  /** @brief Default constructor
  */
  inline vec3 () = default;

  /** @brief Constructor with initialization from 3 values

      @param[in]  x: Initial x coordinate
      @param[in]  y: Initial y coordinate
      @param[in]  z: Initial z coordinate
  */
  inline vec3 (const double x, const double y, const double z)
  {
    X=x; Y=y; Z=z;
  }


  /** @brief Constructor with initialization from 3 values
      @param[in]  p: pointer to x coordinate
  */
  inline explicit vec3 (const double *p)
  {
    X=p[0];  Y=p[1]; Z=p[2];
  }


  /** @brief Copy constructor
      @param[in]  p: Initial vec3
  */
  inline vec3 (const vec3 &p)
  {
    X=p.X;  Y=p.Y;  Z=p.Z;
  }

#ifdef EIGEN3_FOUND
  /** @brief From Eigen::RowVector3d constructor
      @param[in]  p: Initial vec3
  */
  inline vec3 (const Eigen::RowVector3d &p)
  {
    X=p[0];  Y=p[1];  Z=p[2];
  }

  /** @brief To Eigen::RowVector3d conversion
  */
  explicit operator Eigen::RowVector3d() const
  {
    Eigen::RowVector3d v;
    return (v << X, Y, Z).finished();
  }
#endif

  /** @brief Sum two vec3
      @param[in] p: second vec3 operator. First operator is *this
      @return Sum
  */
  inline vec3 operator+ (const vec3 &p) const
  {
      return vec3(X+p.X,Y+p.Y,Z+p.Z);
  }

  /** @brief Subtract a vec3 from *this

      @param[in] p: vec3 to subtract. First operator is *this
      @return Difference of two vec3
  */
  inline vec3 operator- (const vec3 &p) const
  {
      return vec3(X-p.X,Y-p.Y,Z-p.Z);
  }

  /** @brief Unary minus operator. Change the orientation of a vec3
      @return Change the orientation of the vec3
  */
  inline vec3 operator- () const
  {
    return vec3(-X,-Y,-Z);
  }

  /** @brief Cross product of two vec3
      @param[in] p: second vec3 cross operator. First operator is *this
      @return Cross product (a vec3 value)
  */
  inline vec3 cross (const vec3 &p) const
  {
    return vec3((Y*p.Z - Z*p.Y), (Z*p.X - X*p.Z), (X*p.Y - Y*p.X));
  }


  /** @brief Cross product of two vec3
      @param[in] p: second vec3 cross operator. First operator is *this
      @return Cross product (a vec3 value)
  */
  inline vec3 operator^ (const vec3 &p) const
  {
      return vec3((Y*p.Z - Z*p.Y), (Z*p.X - X*p.Z), (X*p.Y - Y*p.X));
  }


  /** @brief Dot product of two vec3
      @param[in] p: second vec3 dot operator. First operator is *this
      @return Dot product (A scalar value)
  */
  inline double dot (const vec3 &p) const
  {
    return (X*p.X + Y*p.Y + Z*p.Z);
  }


  /** @brief Dot product of two vec3
      @param[in] p: second vec3 dot operator. First operator is *this
      @return Dot product (A scalar value)
  */
  inline double operator* (const vec3 &p) const
  {
    return (X*p.X + Y*p.Y + Z*p.Z);
  }

  /** @brief Asignation operator
      @param[in] p: vec3 to be assigned to *this
  */
  inline void operator= (const vec3 &p)
  {
    X=p.X; Y=p.Y; Z=p.Z;
  }


  /** @brief Product of a vec3 and a scalar
      @param[in] p: second vec3 product operator. First operator is *this
      @return Scalar product (A scaled version of *this)
  */
  inline vec3 operator* (const int number) const
  {
    return vec3(X*number, Y*number, Z*number);
  }


  /** @brief Product of a vec3 and a scalar
      @param[in] p: second vec3 product operator. First operator is *this
      @return Scalar product (A scaled version of *this)
  */
  inline vec3 operator* (const double number) const
  {
      return vec3(X*number, Y*number, Z*number);
  }

  /** @brief Product of a vec3 and a scalar
      @param[in] p: second vec3 product operator. First operator is *this
      @return Scalar product (A scaled version of *this)
  */
  inline vec3 operator* (const short number) const
  {
      return vec3(X*number, Y*number, Z*number);
  }

  /** @brief Division of a vec3 by a scalar
      @param[in] p: second vec3 product operator. First operator is *this
      @return Scalar product (A scaled version of *this)
  */
  inline vec3 operator/ (const double number) const
  {
        return vec3( X / number, Y / number, Z / number);
  }

  /** @brief Sum a vec3 to *this in-place
      @param[in] p: second vec3 operator. First operator is *this
      @return None, but *this is modified to contain the sum
  */
  inline void operator+= (const vec3 &p)
  {
    X+=p.X; Y+=p.Y; Z+=p.Z;
  }


  /** @brief Subtract a vec3 from *this in-place
      @param[in] p: second vec3 operator. First operator is *this
      @return None, but *this is modified to contain the difference
  */
  inline void operator-= (const vec3 &p)
  {
    X-=p.X; Y-=p.Y; Z-=p.Z;
  }

  /** @brief Product of a vec3 and a scalar in-place
      @param[in] p: second scalar product operator. First operator is *this
      @return None, but *this is scaled
  */
  inline void operator*= (const double f)
  {
    X *= f; Y *= f; Z *= f;
  }


  /** @brief Division of a vec3 by a scalar in-place
      @param[in] p: scalar divisor operator. First operator is *this
      @return None, but *this is scaled
  */
  inline void operator/= (const double f)
  {
    X /= f; Y /= f; Z /= f;
  }

  /** @brief Cross product of two vec3 in-place
      @param[in] p: second vec3 cross operator. First operator is *this
      @return None, but *this is modified to store the cross product
  */
  inline void operator^= (const vec3 &p)
  {
    //Compute temporal values
    double a=(this->Y*p.Z - this->Z*p.Y);
    double b=(this->Z*p.X - this->X*p.Z);
    double c=(this->X*p.Y - this->Y*p.X);
    //set to this vector
    X=a; Y=b; Z=c;
  }

  /** @brief Check if two vec3 are the same coordinate by coordinate
      @param[in] p: second vec3 == operator. First operator is *this
      @return True if all coordinates are equal
  */
  inline bool operator== (const vec3 &p) const
  {
    return((X==p.X)&&(Y==p.Y)&&(Z==p.Z));
  }

  /** @brief Normalize a vec3 to convert *this into an unary vector
      @return The module of the vector before of normalization
  */
  inline double normalize()
  {
      double mod=sqrt(X*X + Y*Y + Z*Z);
      if(mod == 0)
          return 0;

      double imod=1.0/mod;
      this->X*=imod;
      this->Y*=imod;
      this->Z*=imod;
      return mod;
  }

  /** @brief Module of a vec3
      @return The module of the vector
  */
  inline double module () const
  {
    return double(sqrt(X*X + Y*Y + Z*Z));
  }

  /** @brief Squared module of a vec3
      @return The squared module of the vector
  */
  inline double module2 () const
  {
    return (X*X + Y*Y + Z*Z);
  }

  /** @brief Set vec3 value from 3 coordinates
  */
  inline void set(const double x, const double y, const double z)
  {
    X=x;  Y=y; Z=z;
  }

  /** @brief Inicialize this vec3 to (0,0,0)
  */
  inline void setZero()
  { 
     X = 0; Y = 0; Z = 0; 
  }

  /** @brief Inicialize this vec3 to (+inf, +inf, +inf)
  */
  inline void setPosInf()
  {
     X = INFINITY; Y = INFINITY; Z = INFINITY;
  }

  /** @brief Inicialize this vec3 to (-inf, -inf, -inf)
  */
  inline void setNegInf()
  {
     X = -INFINITY; Y = -INFINITY; Z = -INFINITY;
  }

  /** @brief Inspector of a coordinate
      @pre 0 =< i <= 2
      @param[in] i: coordinate index (0=X, 1=Y, 2=Z)
      @return Value of the coordinate
  */
  inline double operator[] (const unsigned i) const
  {
      if (i==0)
          return X;
      else if (i==1)
          return Y;
      else if (i==2)
          return Z;
      else
          throw "Invalid index accessing to vec3::operator[]";
  }

  /** @brief Reference a coordinate
      @pre 0 =< i <= 2
      @param[in] i: coordinate index (0=X, 1=Y, 2=Z)
      @return Reference to the coordinate
  */
  inline double& operator[] (const unsigned i)
  {
      if (i==0)
          return X;
      else if (i==1)
          return Y;
      else if (i==2)
          return Z;
      else
          throw "Invalid index accessing to vec3::operator[]";
  }


  /** @brief Euclidean distance between two points
      @param[in] p: second vec3 operator. First operator is *this
      @return Euclidean distance between two points.
  */
  inline double distance (const vec3 &p) const
  {
    return double(sqrt(((p.X-X)*(p.X-X)) + ((p.Y-Y)*(p.Y-Y)) + ((p.Z-Z)*(p.Z-Z))));
  }

  /** @brief Squared Euclidean distance between two points.
      @param[in] p: second vec3 operator. First operator is *this
      @return Squared Euclidean distance between two points.
  */
  inline double distance2 (const vec3 &p) const
  {
    return  ((p.X-X)*(p.X-X)) + ((p.Y-Y)*(p.Y-Y)) + ((p.Z-Z)*(p.Z-Z));
  }

  /** @brief Euclidean distance between this point and segment ab.
      @param[in] vec3 a: first extreme of segment ab
      @param[in] vec3 b: second extreme of segment ab
      @return Euclidean distance between point and segment ab.
  */
  inline double distanceSegment(const vec3 &a, const vec3 &b) const
  {
    return sqrt(distanceSegment2(a,b));
  }//double distanceSegment(const vec3 &a, const vec3 &b) const

  /** @brief Squared Euclidean distance between this point and segment ab.
      @param[in] vec3 a: first extreme of segment ab
      @param[in] vec3 b: second extreme of segment ab
      @return Squared Euclidean distance between point and segment ab.
  */
  inline double distanceSegment2(const vec3 &a, const vec3 &b) const
  {
    vec3 ab = b - a;
    vec3 ap = *this - a;
    vec3 bp = *this - b;
    //Project vector ap over segment ab
    double e = ap * ab;
    // Check if this point projects outside of ab, near of point a
    if (e <= 0.0) return ap * ap;
    double f = ab *ab;
    // Check if this point projects outside of ab, near of point b
    if (e >= f) return bp * bp;
    // Handle cases where point projects onto ab
    return ap * ap - e * e / f;
  }//double distanceSegment2(const vec3 &a, const vec3 &b) const

  /** @brief Get the closest point in a segment ab.
      @param[in] vec3 a: first extreme of segment ab
      @param[in] vec3 b: second extreme of segment ab
      @return The point inside segment ab closest to this point
  */
  inline vec3 closestPointInSegment(const vec3 &a, const vec3 &b) const
  {
    vec3 ab = b - a;
    //Project vector (p-a) over segment ab
    double t = (*this - a) * ab;
    // Check if this point projects outside of ab, near of point a
    if (t <= 0.0)
      return a;
    //Compute squared length of segment ab
    double f = ab *ab;
    // Check if this point projects outside of ab, near of point b
    if (t >= f)
      return b;
    // Handle case where point projects onto ab
    return a + ab*(t/f);
  }//double closestPointInSegment(const vec3 &a, const vec3 &b) const

  /** @brief Get the closest point to *this in a triangle abc.
      @return The point inside triangle abc closest to this point
  */
  inline vec3 closestPointInTriangle(const vec3 &a, const vec3 &b, const vec3 &c) const
  {
    //See Real-time Collision Detection book, pages 136..142

    // Check if P in vertex region outside A
    vec3 ab = b - a;
    vec3 ac = c - a;
    vec3 ap = *this - a;
    double d1 = ab * ap;
    double d2 = ac * ap;
    if (d1 <= 0.0 && d2 <= 0.0)
      return a; // barycentric coordinates (1,0,0)

    // Check if P in vertex region outside B
    vec3 bp = *this - b;
    double d3 = (ab * bp);
    double d4 = (ac * bp);
    if (d3 >= 0.0 && d4 <= d3)
      return b; // barycentric coordinates (0,1,0)

    // Check if P in edge region of AB, if so return projection of P onto AB
    double vc = d1*d4 - d3*d2;
    if (vc <= 0.0 && d1 >= 0.0 && d3 <= 0.0)
    {
      double v = d1 / (d1 - d3);
      return a + ab * v; // barycentric coordinates (1-v,v,0)
    }

    // Check if P in vertex region outside C
    vec3 cp = *this - c;
    double d5 = ab * cp;
    double d6 = ac * cp;
    if (d6 >= 0.0 && d5 <= d6)
      return c; // barycentric coordinates (0,0,1)

    // Check if P in edge region of AC, if so return projection of P onto AC
    double vb = d5*d2 - d1*d6;
    if (vb <= 0.0 && d2 >= 0.0 && d6 <= 0.0)
    {
      double w = d2 / (d2 - d6);
      return a + ac * w; // barycentric coordinates (1-w,0,w)
    }

    // Check if P in edge region of BC, if so return projection of P onto BC
    double va = d3*d6 - d5*d4;
    if (va <= 0.0 && (d4 - d3) >= 0.0 && (d5 - d6) >= 0.0)
    {
      double w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
      return b + (c - b) * w; // barycentric coordinates (0,1-w,w)
    }

    // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
    double denom = 1.0 / (va + vb + vc);
    double v = vb * denom;
    double w = vc * denom;
    return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w

  }//vec3 closestPointInTriangle(const vec3 &a, const vec3 &b, const vec3 &c) const


  /*! @brief Compute the barycentric coordinates of *this respect to triangle a,b,c. See also interpolate()
      @param[in] a : coordinates of vertex
      @param[in] b : coordinates of vertex
      @param[in] c : coordinates of vertex
      @return Barycentric coordinates of this point respect points a,b,c
  */
  vec3 barycentric(const vec3 &a, const vec3 &b, const vec3 &c) const
  {
      //See Real-time Collision Detection book, pages 46..52
      //See https://gamedev.stackexchange.com/questions/23743/

      // Compute vectors relative to point a
      vec3 v0 = b - a;
      vec3 v1 = c - a;
      vec3 v2 = *this - a;
      // Compute dot products
      double d00 = v0 * v0;
      double d01 = v0 * v1;
      double d11 = v1 * v1;
      double d20 = v2 * v0;
      double d21 = v2 * v1;
      double denom = 1.0 / (d00 * d11 - d01 * d01); // denom = 2·|v0|·|v1|·triangleArea(a,b,c)

      //Optimization note: If multiple points are tested against the
      //same triangle, the terms d00, d01, d11, denom can be cached.

      // Compute barycentric coordinates u,w using Cramer's rule
      double v = (d11 * d20 - d01 * d21) * denom;
      double w = (d00 * d21 - d01 * d20) * denom;

      return vec3(1.0 - v - w, v, w);
  }//bool vec3::barycentric(a,b,c)

  /*! @brief Barycentric interpolation of 3 scalars using *this as barycentric coordinates. See also barycentric()
      @param[in] a : scalar
      @param[in] b : scalar
      @param[in] c : scalar
      @return Barycentric interpolation of a,b,c at this point
  */
  inline double interpolate(const double a, const double b, const double c) const
  {
      return a*X + b*Y + c*Z;
  }

  /*! @brief Barycentric interpolation of 3 vec3 using *this as barycentric coordinates. See also barycentric()
      @param[in] a : point
      @param[in] b : point
      @param[in] c : point
      @return Barycentric interpolation of a,b,c at this point
  */
  inline vec3 interpolate(const vec3 &a, const vec3 &b, const vec3 &c) const
  {
      return a*X + b*Y + c*Z;
  }

  /*! @brief Check if a point is inside of a triangle. The 4 points must lay on the same plane
      @param[in] a : point
      @param[in] b : point
      @param[in] c : point
      @return    True if point is inside of the triangle.
  */
  inline bool inTriangle(const vec3 &a, const vec3 &b, const vec3 &c) const
  {

    // Compute barycentric coordinates of this point respect to triangle abc
    vec3 bar = this->barycentric(a,b,c);

    // Check if point is inside the triangle
    return (bar.X >= 0) && (bar.Y >= 0) && (bar.Z >= 0) && (bar.X+bar.Y+bar.Z <= 1.0);
  }//bool inTriangle(a,b,c)

};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////IMPLEMENTACION/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef USE_COIN

 inline vec3::vec3(const SbVec3f &vector)
{
  _X=(double)vector[0];
  _Y=(double)vector[1];
  _Z=(double)vector[2];
}

 inline vec3::vec3(const SbVec3f *vector)
{
  if (vector)
    {
      _X=(double)*vector[0];
      _Y=(double)*vector[1];
      _Z=(double)*vector[2];
    }
}


inline void vec3::operator = (const SbVec3f &vector)
{
  _X=(double)vector[0];
  _Y=(double)vector[1];
  _Z=(double)vector[2];
}


inline vec3::operator SbVec3f () const
{
  return SbVec3f(_X, _Y, _Z);
}

#endif

/** @brief Product of a scalar by a vec3
    @param[in] n: Scalar factor
    @param[in] p: vec3 factor
    @return Scalar product n*p (A scaled version of p)
*/
inline vec3 operator*(const double n, const vec3 &p)
{
          return vec3(n*p.X, n*p.Y, n*p.Z);
}

/** @brief Cross product of two vec3
    @param[in] a: First vec3 cross operator.
    @param[in] b: Second vec3 cross operator.
    @return Cross product (a vec3 value)
*/
inline vec3 cross (const vec3 &a, const vec3 &b)
{
  return vec3((a.Y*b.Z - a.Z*b.Y), (a.Z*b.X - a.X*b.Z), (a.X*b.Y - a.Y*b.X));
}


/** @brief Dot product of two vec3
    @param[in] a: First vec3 dot operator.
    @param[in] b: Second vec3 dot operator.
    @return Dot product (A scalar value)
*/
inline double dot (const vec3 &a, const vec3 &b)
{
  return (a.X*b.X + a.Y*b.Y + a.Z*b.Z);
}


/*! @brief Dump a vec3 to an ASCII stream
    @param[in] stream : output ostream
    @param[in] p : const vec3 to be written
    @return    reference to output stream
*/
std::ostream &operator<<(std::ostream &stream, const vec3 &p)
{
  return stream << p.X << " " << p.Y << " " << p.Z;
}

/*! @brief Compute the area of a triangle given by 3 points a,b,c
    @param[in] a : Coordinate of vertex a
    @param[in] b : Coordinate of vertex b
    @param[in] c : Coordinate of vertex c
    @return    Triangle area
*/
double triangleArea(const vec3 &a, const vec3 &b, const vec3 &c )
{
  //Area is half of the module of the cross product of two side vectors
  return 0.5 * ((c-a)^(b-a)).module();
}

/*! @brief Compute the double area of a triangle given by 3 points a,b,c
    @param[in] a : Coordinate of vertex a
    @param[in] b : Coordinate of vertex b
    @param[in] c : Coordinate of vertex c
    @return    2 * Triangle Area
*/
double triangleDoubleArea(const vec3 &a, const vec3 &b, const vec3 &c )
{
  //Area is half of the module of the cross product of two side vectors
  return ((c-a)^(b-a)).module();
}


/*! @brief Compute the circumradius of 3 points
    @param[in] a : point
    @param[in] b : point
    @param[in] c : point
    @return Radius of the circumference that pass by points a, b, c
*/
double circumRadius(const vec3 &a, const vec3 &b, const vec3 &c )
{
    //See http://mathworld.wolfram.com/Circumradius.html

    //Compute the distances
    double la=b.distance(c);
    double lb=a.distance(c);
    double lc=a.distance(b);

    double s = 0.5 * (la+lb+lc); //semiperimeter
    double r = la*lb*lc / (4 * sqrt(s*(la+lb-s)*(la+lc-s)*(lb+lc-s))); //circumradius

    return r;
}//double circumRadius(a,b,c)


/*! @brief Compute the circumcenter of 3 points ( = triangle barycenter). Slow but Robust version
    @param[in] a : point
    @param[in] b : point
    @param[in] c : point
    @return Center of the circumference that pass by points a, b, c
*/
vec3 circumCenter(const vec3 &a, const vec3 &b, const vec3 &c )
{
    vec3 ab = b - a;
    vec3 ac = c - a;

    double a00 = -2.0 * ( ab * a );
    double a01 = -2.0 * ( ab * b );
    double a02 = -2.0 * ( ab * c );
    double b0 = a.module2() - b.module2();

    double a10 = -2.0 * ( ac * a );
    double a11 = -2.0 * ( ac * b );
    double a12 = -2.0 * ( ac * c );
    double b1 = a.module2() - c.module2();


    //Compute baricentric coordinates of the circumcenter
    double div = -a11*a00 + a11*a02 - a10*a02 + a00*a12 + a01*a10 - a01*a12;
    double alpha = -(-a11*a02+a01*a12-a12*b0+b1*a02+a11*b0-a01*b1) / div;
    double beta  =  (a10*b0-a10*a02-a12*b0+a00*a12+b1*a02-a00*b1)  / div;
    double gamma =  (-a11*a00-a10*b0+a00*b1+a11*b0+a01*a10-a01*b1) / div;

    //Compute the center applying the baricentric formula
    vec3 center = a*alpha + b*beta + c*gamma;

    return center;
}//vec3 circumCenter(a,b,c)


/*! @brief Compute the circumcenter of 3 points ( = triangle barycenter). Fast but non-Robust version
    @param[in] a : point
    @param[in] b : point
    @param[in] c : point
    @return Center of the circumference that pass by a, b, c
*/
vec3 circumCenter_f(const vec3 &a, const vec3 &b, const vec3 &c )
{
    vec3 ab = b - a;
    vec3 ac = c - a;
    vec3 abXac = ab.cross(ac);

    // Compute circumsphere center (relative to vertex a)
    vec3 ccs = (abXac.cross(ab)*ac.module2() + ac.cross(abXac)*ab.module2()) / (2*abXac.module2()) ;

    // return circumCenter
    return a  +  ccs ;
}//vec3 circumCenter_f(a,b,c)


/*! @brief Compute the incenter of 3 points.
    @param[in] a : point
    @param[in] b : point
    @param[in] c : point
    @return  Center of the inscribed circle to triangle a, b, c
*/
vec3 inCenter(const vec3 &a, const vec3 &b, const vec3 &c )
{
    double ab = a.distance(b);
    double bc = b.distance(c);
    double ca = c.distance(a);

    //Avoid division-by-zero in zero-perimeter triangles
    double perimeter = ab + bc + ca;
    if (perimeter < 0.0001)
        return (a+b+c)/3.0;

    //http://mathworld.wolfram.com/Incenter.html
    return (a*bc + b*ca + c*ab)/perimeter;
}


/*! @brief Compute the inradius of 3 points.
    @param[in] a : point
    @param[in] b : point
    @param[in] c : point
    @return  Radius of the inscribed circle to triangle a, b, c
*/
double inRadius(const vec3 &a, const vec3 &b, const vec3 &c )
{
    //See http://mathworld.wolfram.com/inradius.html

    //Compute the distances
    double la=b.distance(c);
    double lb=a.distance(c);
    double lc=a.distance(b);

    //Avoid division-by-zero in zero-perimeter triangles
    double s = la+lb+lc;
    if (s < 0.000001)
        return 0.0;

    double r = 0.5 * sqrt((lb+lc-la)*(la-lb+lc)*(la+lb-lc)/s);

    return double(r);
}

/*! @brief Linear interpolation of two vec3.
    @param[in] a : point
    @param[in] b : point
    @param[in] t : Interpolation weight
    @return value of a + t*(b - a)
*/
vec3 lerp(const vec3 &a, const vec3 &b, const double t )
{
    return a + t * (b - a);
}



#endif
