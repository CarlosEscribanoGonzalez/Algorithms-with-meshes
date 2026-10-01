/*
 * SimpleMesh.hpp
 *
 * Written by Jose Miguel Espadero <josemiguel.espadero@urjc.es>
 *
 * This code is written as example for the FMF class of the
 * Master Universitario en Informatica Grafica, Juegos y Realidad Virtual.
 * Its purpose is to be didactic and easy to understand, not hard optimized.
 *
 * This file implements the storage and usual operations for a generic triangle mesh.
 * Storage is based just in coordinates of vertex and topology based on triangles.
 * No fields are implemented to storage normals, colors, texture coordinates, etc...
 */

#ifndef _SIMPLEMESH3D_
#define _SIMPLEMESH3D_

#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>

#include "vec3.hpp"

//Include support to read/write .ply file format (optional)
#include "tinyply.h"

using namespace std;

///Simple class to store an edge as 2 indexes to an external coordinates container
class SimpleEdge {
public:
    unsigned a;
    unsigned b;

    SimpleEdge ()
    {
    }

    SimpleEdge (const unsigned _a, const unsigned _b)
    {
        a = _a;
        b = _b;
    }

    /// Set the values of the indexes
    void set (const unsigned _a, const unsigned _b)
    {
        a = _a;
        b = _b;
    }

    /// Check if the edge contains one index
    inline bool contains(const unsigned index) const
    {
        return (a==index || b==index);
    }

    /// Build the reversed version of the edge
    inline SimpleEdge reversed() const
    {
        return SimpleEdge(b,a);
    }

    /// Change the orientation of the edge
    inline void reverseOrientation()
    {
        std::swap(a,b);
    }

    /// Check if two edges are equal
    inline bool operator == (const SimpleEdge &e) const
    {
        return a == e.a && b == e.b;
    }

    /// Check if two edges are different
    inline bool operator != (const SimpleEdge &e) const
    {
        return a != e.a || b != e.b;
    }

    /// Lexicographically less operator. Useful to declare sorted containers of edges
    inline bool operator < (const SimpleEdge &e) const
    {
        return (a < e.a) || ( (a == e.a) && (b < e.b) );
    }

}; //class SimpleEdge

/// Write the indexes of one SimpleEdge to a stream
std::ostream &operator<<(std::ostream &stream, const SimpleEdge &e)
{
  return stream << e.a << " " << e.b ;
}


///Simple class to store a triangle as 3 indexes to an external coordinates container
class SimpleTriangle {
public:
    unsigned a;
    unsigned b;
    unsigned c;

    /// Default constructor
    SimpleTriangle ()
    {
        a = 0;
        b = 0;
        c = 0;
    }

    /// Initializer constructor
    SimpleTriangle (const unsigned _a, const unsigned _b, const unsigned _c)
    {
        a = _a;
        b = _b;
        c = _c;
    }

    /// Change the values of the indexes
    void set (const unsigned _a, const unsigned _b, const unsigned _c)
    {
        a = _a;
        b = _b;
        c = _c;
    }

    /// Check if the triangle contains one index
    inline bool contains(const unsigned index) const
    {
        return (a==index || b==index || c==index);
    }

    /// Check if the triangle contains one edge
    inline bool contains(const SimpleEdge &e) const
    {
        return ((a==e.a && b==e.b)||(b==e.a && c==e.b)||(c==e.a && a==e.b));
    }

    /// Build the reversed version of the edge
    inline SimpleTriangle reversed() const
    {
        return SimpleTriangle(c,b,a);
    }

    /// Change the orientation of the face
    inline void reverseOrientation()
    {
        std::swap(a,c);
    }

    /// Return a vector with the 3 edges of this triangle
    std::vector<SimpleEdge> edges() const
    {
        return {SimpleEdge(a,b), SimpleEdge(b,c), SimpleEdge(c,a)};
    }

    /// Flip the common edge with other triangle. Both triangles are modified
    void edgeFlip(SimpleTriangle &t)
    {
        //      b a     <->      b      .
        //      /|\     <->     / \     .
        //     / | \    <->   c/   \a   .
        //   c<  |t >c  <->   <----->   .
        //     \ | /    <->   a\ t /c   .
        //      \|/     <->     \ /     .
        //      a b     <->      b      .

        //Check if edge ab is the shared edge with triangle t
        if (t.contains(c))
        {
            if (t.contains(b))
            {
                //shared edge is bc. Rotate indexes so new ab= old bc
                auto tmp=a;
                a=b;
                b=c;
                c=tmp;
            }
            else
            {
                //shared edge is ca. Rotate indexes so new ab= old ca
                auto tmp=a;
                a=c;
                c=b;
                b=tmp;
            }
        }

        //Check if edge t.a t.b is the shared edge with this triangle
        if (contains(t.c))
        {
            if (contains(t.b))
            {
                //shared edge is bc. Rotate indexes so new ab= old bc
                auto tmp=t.a;
                t.a=t.b;
                t.b=t.c;
                t.c=tmp;
            }
            else
            {
                //shared edge is ca. Rotate indexes so new ab= old ca
                auto tmp=t.a;
                t.a=t.c;
                t.c=t.b;
                t.b=tmp;
            }
        }

        //Sanity check: edge ab should be equal to edge t.b t.a
        if (a != t.b || b!= t.a || c == t.c)
            throw "edgeFlip(): Triangles do not share an edge.";

        //Exchange indexes to flip shared edge in both triangles
        a = t.c;
        t.a = c;

    }//void edgeFlip(SimpleTriangle &t)

    /// Return true if t is a permutation of this triangle with same orientation.
    inline bool isLike (const SimpleTriangle &t) const
    {
        //       c      isLike       b      isLike       a      .
        //      / \     isLike      / \     isLike      / \     .
        //     /   \    isLike     /   \    isLike     /   \    .
        //    a-----b   isLike    c-----a   isLike    b-----c   .
        return ((a==t.a && b==t.b && c==t.c) ||
                (a==t.b && b==t.c && c==t.a) ||
                (a==t.c && b==t.a && c==t.b) );

    }

    /// Lexicographically less operator. Useful to declare sorted containers of triangles
    inline bool operator < (const SimpleTriangle &t) const
    {
        return (a < t.a) || ((a == t.a) && (b < t.b)) || ((a == t.a) && (b == t.b) && (c < t.c));
    }
}; //class SimpleTriangle


/// Write the indexes of a SimpleTriangle to a stream
std::ostream &operator<<(std::ostream &stream, const SimpleTriangle &t)
{
  return stream << t.a << " " << t.b << " " << t.c;
}

/// A triangle mesh with std::vector containers for vec3 coordinates and SimpleTriangle facets.
class SimpleMesh {
public:
    /// A container of vec3 for coordinates of vertex
    std::vector<vec3> coordinates;
    /// A container of SimpleTriangle for topology
    std::vector<SimpleTriangle> triangles;

    /// Return the number of vertex in the mesh
    inline size_t numVertex() const
    {
        return coordinates.size();
    }

    /// Return the number of triangles in the mesh
    inline size_t numTriangles() const
    {
        return triangles.size();
    }

    /// Read a mesh from a file. Use filename to guess file format (PLY, OFF, OBJ)
    void readFile(const std::string &filename, bool debug=false);

    /// Read a mesh from a OFF stream
    void readStreamOFF(istream &is, bool debug=false);

    /// Read a mesh from a OBJ stream
    void readStreamOBJ(istream &is, bool debug=false);

    /// Read a mesh from a PLY stream
    void readStreamPLY(istream &is, bool debug=false);

    /// Write a mesh to a OFF file
    void writeFileOFF(const std::string &filename) const;

    /// Write a mesh to a OFF stream
    void writeStreamOFF (ostream &os) const;

    /// Write a mesh to a PLY file
    void writeFilePLY(const std::string &filename, bool binary=true) const;

    /// Write a mesh to a PLY stream
    void writeStreamPLY (ostream &os, bool binary=true) const;

    /// Write a mesh to a OBJ file
    void writeFileOBJ(const std::string &filename) const;

    /// Write a mesh to a OBJ stream
    void writeStreamOBJ (ostream &os) const;

    /// Write a mesh to a matlab file
    void writeFileMatlab (const std::string &filename) const;

    /** @brief Euclidean distance between any two vertex of the mesh (usually, an edge).
        @param[in] v1: Index of first vertex
        @param[in] v2: Index of second vertex
        @return Euclidean distance between two vertex.
    */
    inline double distance(const unsigned v1, const unsigned v2) const
    {
        return coordinates[v1].distance(coordinates[v2]);
    }

    /** @brief Squared Euclidean distance between any two vertex of the mesh (usually, an edge).
        @param[in] v1: Index of first vertex
        @param[in] v2: Index of second vertex
        @return Squared Euclidean distance between two vertex.
    */
    inline double distance2(const unsigned v1, const unsigned v2) const
    {
        return coordinates[v1].distance2(coordinates[v2]);
    }

    /** @brief Euclidean distance between any two vertex of the mesh (usually, an edge).
        @param[in] v1: Index of first vertex
        @param[in] v2: Index of second vertex
        @return Euclidean distance between two vertex.
    */
    inline double edgeLength(const unsigned v1, const unsigned v2) const
    {
        return coordinates[v1].distance(coordinates[v2]);
    }

    /** @brief Euclidean distance between both extremes of an edge.
        @param[in] e: SimpleEdge
        @return Euclidean distance between two vertex.
    */
    inline double edgeLength(const SimpleEdge &e) const
    {
        return coordinates[e.a].distance(coordinates[e.b]);
    }

    /** @brief Squared Euclidean distance between any two vertex of the mesh (usually, an edge).
        @param[in] v1: Index of first vertex
        @param[in] v2: Index of second vertex
        @return Squared Euclidean distance between two vertex.
    */
    inline double edgeLength2(const unsigned v1, const unsigned v2) const
    {
        return coordinates[v1].distance2(coordinates[v2]);
    }

    /** @brief Squared Euclidean distance between both extremes of an edge.
        @param[in] e: SimpleEdge
        @return Squared Euclidean distance between two extremes of an edge.
    */
    inline double edgeLength2(const SimpleEdge &e) const
    {
        return coordinates[e.a].distance2(coordinates[e.b]);
    }

    /** @brief Euclidean distance between a point and one edge of the mesh
        @param[in] p:  Coordinates of a point
        @param[in] v1: Index of first vertex
        @param[in] v2: Index of second vertex
        @return Euclidean distance between p and edge(v1,v2)
    */
    inline double edgeDistance(const vec3 &p, const unsigned v1, const unsigned v2) const
    {
        return p.distanceSegment(coordinates[v1], coordinates[v2]);
    }

    /** @brief Squared Euclidean distance between a point and one edge of the mesh
        @param[in] p:  Coordinates of a point
        @param[in] v1: Index of first vertex
        @param[in] v2: Index of second vertex
        @return Squared Euclidean distance between p and edge(v1,v2)
    */
    inline double edgeDistance2(const vec3 &p, const unsigned v1, const unsigned v2) const
    {
        return p.distanceSegment2(coordinates[v1], coordinates[v2]);
    }

    /// Compute the area of a SimpleTriangle
    inline double triangleArea(const SimpleTriangle &t) const
    {
        //The area is half of the module of cross product of two edges
        vec3 pv = (coordinates[t.c]-coordinates[t.a])^(coordinates[t.b]-coordinates[t.a]);
       return 0.5 * pv.module();
    }

    /// Compute the double of the area of a SimpleTriangle
    inline double triangleDoubleArea(const SimpleTriangle &t) const
    {
        //The double area is the module of cross product of two edges
        vec3 pv = (coordinates[t.c]-coordinates[t.a])^(coordinates[t.b]-coordinates[t.a]);
       return pv.module();
    }

    /// Compute the normal of a SimpleTriangle
    inline vec3 triangleNormal(const SimpleTriangle &t) const
    {
        //Compute the product vectorial of two edges and normalize it
        vec3 pv = (coordinates[t.c]-coordinates[t.a])^(coordinates[t.b]-coordinates[t.a]);
        pv.normalize();
        return pv;
    }

    /// Compute the circumradius of a triangle
    inline double trianglecircumRadius(const SimpleTriangle &t) const
    {
        //Compute the circumradius from 3 coordinates
        return circumRadius(coordinates[t.a],coordinates[t.b],coordinates[t.c]);
    }//vec3 trianglecircumRadius(const int faceIdx) const

    /// Compute the circumCenter (barycenter) of a triangle
    inline vec3 triangleCircumCenter(const SimpleTriangle &t) const
    {
        return circumCenter(coordinates[t.a], coordinates[t.b], coordinates[t.c]);
    }

    /// Compute the inCenter of a triangle
    inline vec3 triangleInCenter(const SimpleTriangle &t) const
    {
        return inCenter(coordinates[t.a], coordinates[t.b], coordinates[t.c]);
    }

    /// Compute the circumradius/shortestEdge ratio of a face. Less is better.
    /// Used by "Delaunay Refinement Mesh Generation", Jonathan R. Shewchuk, 1997
    inline double triangleShapeFactor1(const SimpleTriangle &t) const
    {
        double radius = trianglecircumRadius(t);
        double minL = triangleMinEdge(t);
        return radius / minL;
    }

    /// Compute the circumradius/shortestEdge ratio of a face. Less is better.
    inline double triangleShapeFactor1(unsigned t) const
    {
       return triangleShapeFactor1(triangles[t]);
    }

    /// Compute the shortestEdge/circumradius ratio of a face. High is better.
    /// Will return a value between 0.0 (zero area triangles) and 1.0 (equilateral triangles)
    inline double triangleShapeFactor(const SimpleTriangle &t) const
    {
        double radius = trianglecircumRadius(t);
        double minL = triangleMinEdge(t);

        if (radius == 0.0)
            return 0;

        return minL/(radius*sqrt(3));

    }

    /// Compute the shortestEdge/circumradius ratio of a face. High is better.
    inline double triangleShapeFactor(unsigned t) const
    {
       return triangleShapeFactor(triangles[t]);
    }

    /// Compute the centroid (averaged center) of a SimpleTriangle
    inline vec3 triangleCentroid(const SimpleTriangle &t) const
    {
        return (coordinates[t.a] + coordinates[t.b] + coordinates[t.c])/3.0;
    }

    /// Compute the longest edge length of a face
    double triangleMaxEdge(const SimpleTriangle &t) const
    {
        //Compute squared distance of edges
        double LC = coordinates[t.a].distance2(coordinates[t.b]);
        double LA = coordinates[t.b].distance2(coordinates[t.c]);
        double LB = coordinates[t.c].distance2(coordinates[t.a]);

        if (LA > LB)
            return sqrt(std::max(LA, LC));
        else
            return sqrt(std::max(LB, LC));

    }//double triangleMaxEdge(const SimpleTriangle &t) const

    /// Compute the shortest edge length of a SimpleTriangle
    double triangleMinEdge(const SimpleTriangle &t) const
    {
        //Compute squared distance of edges
        double LC = coordinates[t.a].distance2(coordinates[t.b]);
        double LA = coordinates[t.b].distance2(coordinates[t.c]);
        double LB = coordinates[t.c].distance2(coordinates[t.a]);

        if (LA < LB)
            return sqrt(std::min(LA, LC));
        else
            return sqrt(std::min(LB, LC));
    }//double triangleMinEdge(const SimpleTriangle &t) const


    /// Flip the common edge between two triangles. Both triangles are modified.
    /// An exception will be throw if triangles does not share one edge.
    inline void edgeFlip(const unsigned t1, const unsigned t2)
    {
        //       .      >       .     .
        //      / \     >      /|\    .
        //     /   \    >     / | \   .
        //    .-----.   >    .  |  .  .
        //     \   /    >     \ | /   .
        //      \ /     >      \|/    .
        //       .      >       .     .
        triangles[t1].edgeFlip(triangles[t2]);
    }//void edgeFlip(const unsigned t1, const unsigned t2)

    /** @brief Get the closest point to point p inside the triangle abc.
        @return The point inside triangle abc closest to point p
    */
    inline vec3 closestPointInTriangle(const SimpleTriangle &t, const vec3 &p) const
    {
        return p.closestPointInTriangle(coordinates[t.a], coordinates[t.b], coordinates[t.c]);
    }

    /// Check duplicated vertex (with distance threshold)
    std::vector<bool> checkDuplicatedVertex (const double minDistance = 0.0) const
    {
        double minDistance2 = minDistance * minDistance;
        //Reserve and initialize the result
        std::vector<bool> result(numVertex(), false);
        for (size_t i = 0; i< numVertex(); i++)
            //Avoid re-check vertex previously marked as duplicated
            if (result[i] == false)
                //Start at i+1 to avoid comparing each pair of vertex twice.
                for (size_t j=i+1; j< numVertex(); j++)
                    if (coordinates[i].distance2(coordinates[j]) <= minDistance2 )
                    {
                        //Set both vertex as duplicated
                        result[i] = true;
                        result[j] = true;
                    }
        return result;
    }// std::vector<bool> checkDuplicatedVertex (const double minDistance = 0.0) const

    /// Check obtuse triangles
    std::vector<bool> checkObtuseTriangles () const
    {
    //Reserve memory for the result
    std::vector<bool> result;
    result.reserve(numTriangles());

    //Iterate over all triangles (C++11 foreach style)
    for (const SimpleTriangle & t : triangles )
    {
        //Compute vectors for edges of the triangle
        auto e_ab=coordinates[t.b] - coordinates[t.a];
        auto e_bc=coordinates[t.c] - coordinates[t.b];
        auto e_ca=coordinates[t.a] - coordinates[t.c];

        //Normalize edges of the triangle
        //OPTIMIZED: because we only need to check the sign of the dotProduct
        //e_ab.normalize();
        //e_bc.normalize();
        //e_ca.normalize();

        //Compute cosines of each angle in the triangle.
        double cos_A = -(e_ab * e_ca);
        double cos_B = -(e_bc * e_ab);
        double cos_C = -(e_ca * e_bc);

        //Check directly the cosine (negative for obtuse angles)
        bool obtuse = cos_A < 0 || cos_B < 0 || cos_C < 0;
        result.push_back(obtuse);
    }//for

    return result;
    }//std::vector<bool> checkObtuseTriangles () const

    /// Check obtuse triangles (only greater angle)
    std::vector<bool> checkObtuseTriangles2 () const
    {
    //Reserve the result
    std::vector<bool> result;
    result.reserve(numTriangles());

    //Iterate over all triangles (C++11 style)
    for (const SimpleTriangle & t : triangles )
    {
        //get squared edge length of edges.
        double l0_2=coordinates[t.b].distance2(coordinates[t.a]);
        double l1_2=coordinates[t.c].distance2(coordinates[t.b]);
        double l2_2=coordinates[t.a].distance2(coordinates[t.c]);

        //Sort edges to ensure l0 is the longest edge.
        //The angle opposite to l0 will be the greatest of the triangle.
        if (l0_2 < l1_2) swap(l0_2, l1_2);
        if (l0_2 < l2_2) swap(l0_2, l2_2);

        //Pitagoras said 2500 years ago: for a right triangle, a^2 = b^2 + c^2
        //But if a^2 > b^2 + c^2 then triangle is obtuse...
        //Remember our distances are already squared
        bool obtuse = l0_2 > l1_2 + l2_2;
        result.push_back(obtuse);
    }//for

    return result;
    }//std::vector<bool> checkObtuseTriangles () const


    /// Count the number of triangles referencing each vertex
    std::vector<size_t> vertexValences() const
    {
        //Create a vector with the number of references of each vertex
        std::vector<size_t>vv(numVertex(), 0);

        //Check all the triangles in the mesh, updating the valences
        for (const SimpleTriangle &t : triangles)
        {
            vv[t.a]++;
            vv[t.b]++;
            vv[t.c]++;
        }
        return vv;
    }//std::vector<size_t> SimpleMesh::vertexValences() const

    /// Count the number of vertex non-referenced by any triangle
    size_t checkUnreferencedVertex (const bool verbose=false) const;

    /// Search and remove the vertex non-referenced by any triangle
    size_t removeUnreferencedVertex (const bool verbose=false);

    /// Remove triangles whose area is equal or smaller than a threshold
    /// Return the number of removed triangles
    // Note: This method may not preserve the conectivity of the mesh,
    // often will create holes and let unreferenced vertex in the mesh
    size_t removeSmallTriangles(const double minArea = 0.0);

    /// Scale the mesh coordinates by a factor
    void scaleCoordinates(const double scaleFactor)
    {
        for (vec3 &v : coordinates)
            v *= scaleFactor;
    }

    /// Compute the Axis-Aligned Bounding-Box for the coordinates of the mesh
    void computeAABB(vec3 &min, vec3 &max) const
    {
        if (coordinates.size() == 0)
        {
            cerr << __FUNCTION__ << " : Called from a empty mesh" << std::endl;
            return;
        }

        min = coordinates[0];
        max = coordinates[0];
        for (const vec3 &v : coordinates)
        {
          if (v.X < min.X)
            min.X = v.X;
          if (max.X < v.X)
            max.X = v.X;
          if (v.Y < min.Y)
            min.Y = v.Y;
          if (max.Y < v.Y)
            max.Y = v.Y;
          if (v.Z < min.Z)
            min.Z = v.Z;
          if (max.Z < v.Z)
            max.Z = v.Z;
        }
    }//void computeAABB(vec3 &min, vec3 &max) const

#ifdef _OPENMP
    /// Compute the Axis-Aligned Bounding-Box for the coordinates of the mesh
    void computeAABB_omp(vec3 &min, vec3 &max) const
    {
        if (coordinates.size() == 0)
        {
            cerr << __FUNCTION__ << " : Called from a empty mesh" << std::endl;
            return;
        }

        double min_X,min_Y,min_Z, max_X,max_Y,max_Z;
        //Unsupported in visual studio 2020
        //#pragma omp parallel for reduction(max:max_X,max_Y,max_Z) reduction(min:min_X,min_Y,min_Z) num_threads(4)
        for (size_t i = 0; i< coordinates.size(); ++i)
        {
          const vec3 &v = coordinates[i];
          min_X = min_X < v.X ? min_X : v.X;
          min_Y = min_Y < v.Y ? min_Y : v.Y;
          min_Z = min_Z < v.Z ? min_Z : v.Z;

          max_X = max_X > v.X ? max_X : v.X;
          max_Y = max_Y > v.Y ? max_Y : v.Y;
          max_Z = max_Z > v.Z ? max_Z : v.Z;
        }
        min.set(min_X, min_Y, min_Z);
        max.set(max_X, max_Y, max_Z);
    }//void computeAABB_omp(vec3 &min, vec3 &max) const
#endif

    /// Compute the median center (as the center of the AABB)
    vec3 computeAABBCenter() const
    {
        vec3 min(coordinates[0]), max(coordinates[0]);
        computeAABB(min, max);
        return (min+max)/2;
    }//vec3 computeAABBCenter() const

    /// Compute the centroid (average of vertex coordinates)
    vec3 computeCentroid() const
    {
        //Sum all coordinates
        vec3 centroid(0,0,0);
        for (const vec3 &v : coordinates)
            centroid += v;
        //Divide by the number of coordinates to compute the centroid
        centroid /= double(numVertex());
        return centroid;
    }//vec3 computeCentroid() const

    ///Build an adjacency list as a sorted list of neighbours per vertex
    void computeNeighbours(std::vector<std::vector<int> > &neighbours);

}; // class SimpleMesh

/// Read a mesh from a file. Use filename to guess file format (PLY, OFF, OBJ)
void SimpleMesh::readFile(const std::string &filename, bool debug)
{
    //Open the file as a stream
    ifstream inStream(filename.c_str());
    if (!inStream.is_open())
        throw filename + string(": Error opening the file");
    try
    {
        if (filename.length() > 3 && (! filename.compare(filename.length()-3, 3, "obj")))
        {
           readStreamOBJ(inStream, debug);
        }
#ifdef tinyply_h
        else if (filename.length() > 3 && (! filename.compare(filename.length()-3, 3, "ply")))
        {
           //Reopen stream as binary
           inStream.close();
           ifstream binStream(filename.c_str(), ios::binary);
           readStreamPLY(binStream, debug);
        }
#endif
        else
        {
            readStreamOFF(inStream, debug);
        }
    }
    catch (const string &str) { throw filename + " : " + str; }

}//void SimpleMesh::readFile(const std::string &filename, bool debug)

/// Read a mesh from a OFF stream
void SimpleMesh::readStreamOFF(istream &is, bool debug)
{
    //std::string dummy_buffer;
    std::string header;

    //Read header
    if (!(is >> header))
        throw std::string("error loading header");

    //Check if header ends with "OFF"
    if (header.length() < 3 || header.compare (header.length() - 3, 3, "OFF"))
    {
        cerr << "Header: " << header << endl;
        throw std::string("file is not in OFF file format.");
    }

    //Read the number of vertex
    int nv, nf, ne;
    if (!(is >> nv >> nf >> ne))
        throw string("error loading number of vertex, faces and edges");

    if (debug)
        cerr << "vertex: " << nv << " faces: " << nf << " edges: " << ne << std::endl;

    //Reserve memory in advance
    this->coordinates.resize(nv);
    this->triangles.resize(nf);

    //Read the values of vertex
    for (int i=0; i<nv ; i++)
    {
        double x, y, z;
        //Read values for X, Y, Z
        if (!(is >> x >> y >> z))
            throw string("error loading coordinates");

        //Save the values
        this->coordinates[i].set(x,y,z);
        if (debug)
            cerr << "vertex: " << i << " -> " << x << " " << y << " " << z << std::endl;
    }//for

    //Read the values of triangles
    for (int i=0; i<nf ; i++)
    {
        int n, a, b, c;
        //Read values for indexes
        if (!(is >> n >> a >> b >> c))
            throw string("error loading triangles");

        //Check it a triangle mesh
        if (n != 3)
        {
            cerr << "Face: " << i << std::endl;
            throw string("This reader only support triangle faces");
        }

        //Check some errors
        if (a<0 || b<0 || c <0 || a>=nv || b >= nv || c >= nv)
        {
            cerr << "Triangle: " << i << " -> " << a << " " << b << " " << c << std::endl;
            throw string("Invalid value for index");
        }

        //Save the values
        this->triangles[i].set(a,b,c);

        if (debug)
            cerr << "triangle: " << i << " -> " << a << " " << b << " " << c << std::endl;
    }//for

}//void SimpleMesh::readStreamOFF(istream &is, bool debug)

/// Read a mesh from a OBJ stream
void SimpleMesh::readStreamOBJ(istream &is, bool debug)
{

    std::string line;
    bool ok = true;

    //Read line by line
    while (std::getline(is, line))
    {
        std::istringstream iss(line);
        std::string command;

        //Ignore empty lines
        if (!(iss >> command ))
            continue;

        //Debug info:
        if (debug)
        {
            cerr << "Line: " << line << endl;
        };


        //Ignore comments
        if (command[0] == '#')
            continue;

        //Parse arguments
        ok=false;

        //Vertex coordinates
        if (command == "v")
        {
            double x, y, z;
            ok = !(iss >> x >> y >> z).fail();
            if (ok)
            {
                this->coordinates.push_back(vec3(x,y,z));
                continue;
            }
            else
                throw string("error loading coordinates");
        }

        if (command == "f")
        {
            int a, b, c;
            int ua, ub, uc;
            int na, nb, nc;
            char ch;
            //Try to read a line with texture and normal coordinates
            ok = !(iss >>a>>ch>>ua>>ch>>na  >>b>>ch>>ub>>ch>>nb  >>c>>ch>>uc>>ch>>nc).fail();
            if (!ok)
            {
                //Try without normal coordinates
               std::istringstream iss2(line);
               iss2 >> command;
               ok = !(iss2 >>a>>ch>>ua  >>b>>ch>>ub  >>c>>ch>>uc).fail();
            }
            if (!ok)
            {
                //Try just coordinate index
               std::istringstream iss3(line);
               iss3 >> command;
               ok = !(iss3 >> a >> b >> c).fail();
            }

            if (ok)
            {
                this->triangles.push_back(SimpleTriangle(a-1, b-1, c-1));

                //Triangulate faces with more than 3 vertex (only work with convex faces)
                b = c;
                while (iss >> c)
                {
                    this->triangles.push_back(SimpleTriangle(a-1, b-1, c-1));
                    b = c;
                }

                continue;
            }
            else
                throw string("error loading triangle indexes");
        }

        if (command == "g")
        {
            cerr << "Unsupported OBJ command: " << line << endl;
            continue;
        }

        if (command == "vn")
        {
            cerr << "Unsupported OBJ command: " << line << endl;
            continue;
        }

        if (command == "vt")
        {
            cerr << "Unsupported OBJ command: " << line << endl;
            continue;
        }

        //Show the line with the unknown command
        cerr << "Error parsing line: " << line << endl;

        //Ignore errors
        continue;

        //throw exception on errors
        //if (!ok) {
        //    throw string("Error parsing OBJ line: ") + line;
        //    break;
        //}

    }//while (std::getline(f, line))

}//void SimpleMesh::readStreamOFF(istream &is, bool debug)

#ifdef tinyply_h
/// Read a mesh from a PLY stream
void SimpleMesh::readStreamPLY(istream &is, bool debug)
{
    // Read the file and create a std::istringstream suitable
    // for the lib -- tinyply does not perform any file i/o.
    tinyply::PlyFile plyFile;
    plyFile.parse_header(is);

    if (debug)
    {
        for (auto e : plyFile.get_elements())
        {
            cout << "element - " << e.name << " (" << e.size << ")" << endl;
            for (auto p : e.properties)
                std::cout << "\tproperty - " << p.name << " ("
                          << tinyply::PropertyTable[p.propertyType].str << ")"
                          << std::endl;
        }

        for (auto c : plyFile.get_comments())
            std::cout << "Comment: " << c << std::endl;
    }//if (debug)


    // Tinyply 2.0 treats incoming data as untyped byte buffers. It's now
    // up to users to treat this data as they wish. See below for examples.
    std::shared_ptr<tinyply::PlyData> vertices, faces, texcoords;

    // The header information can be used to programmatically extract properties on elements
    // known to exist in the file header prior to reading the data. For brevity of this sample, properties
    // like vertex position are hard-coded:
    vertices = plyFile.request_properties_from_element("vertex", { "x", "y", "z" });
    faces = plyFile.request_properties_from_element("face", { "vertex_indices" });

    // Now populate the vectors...
    plyFile.read(is);

    if (debug)
    {
        std::cout << "\tRead " << vertices->count << " vertices "<< std::endl;
        std::cout << "\tRead " << faces->count << " faces (triangles) " << std::endl;
    }

    //Set coordinates to this mesh
    coordinates.resize(vertices->count);
    if (vertices->t == tinyply::Type::FLOAT32)
    {
        float *v = reinterpret_cast<float *>(vertices->buffer.get());
        for (size_t i=0; i< vertices->count; ++i)
            coordinates[i].set(double(v[3*i]),double(v[3*i+1]),double(v[3*i+2]));
    }
    else if (vertices->t == tinyply::Type::FLOAT64)
    {
        double *v = reinterpret_cast<double *>(vertices->buffer.get());
        for (size_t i=0; i< vertices->count; ++i)
            coordinates[i].set(v[3*i],v[3*i+1],v[3*i+2]);
    }

    //Set triangles to this mesh
    triangles.resize(faces->count);
    std::memcpy(triangles.data(), faces->buffer.get(), faces->buffer.size_bytes());
    unsigned *f = reinterpret_cast<unsigned *>(faces->buffer.get());
    for (size_t i=0; i< faces->count; ++i)
        triangles[i].set(f[3*i],f[3*i+1],f[3*i+2]);

}//void readStreamPLY(istream &is, bool debug)

///Write a mesh to a PLY file
void SimpleMesh::writeFilePLY(const std::string &filename, bool binary) const
{
    //Open the file as a stream
    ofstream os(filename.c_str(),std::ios::binary);
    if (!os.is_open())
        throw filename + string(": Error creating the file");

    writeStreamPLY(os, binary);

    os.close();
}//void SimpleMesh::writeFilePLY(const std::string &filename) const


///Write a mesh to a PLY stream
void SimpleMesh::writeStreamPLY (ostream &os, bool binary) const
{
    //Prepare coordinates in serialized float {x,y,z} format
    std::vector<float> verts;
    verts.reserve(3*coordinates.size());
    for (const auto &v : coordinates)
    {
        verts.push_back(float(v.X));
        verts.push_back(float(v.Y));
        verts.push_back(float(v.Z));
    }

    //Prepare triangle indexes in serialized uint32 {a,b,c} format
    std::vector<uint32_t> faces;
    faces.reserve(3*triangles.size());
    for (const SimpleTriangle &t : triangles)
    {
        faces.push_back(t.a);
        faces.push_back(t.b);
        faces.push_back(t.c);
    }

    //Dump to a stream with tinyply
    tinyply::PlyFile myFile;
    myFile.add_properties_to_element("vertex", { "x", "y", "z" },
                                     tinyply::Type::FLOAT32, coordinates.size(),
                                     reinterpret_cast<uint8_t*>(verts.data()),
                                     tinyply::Type::INVALID, 0);
    myFile.add_properties_to_element("face", { "vertex_indices" },
                                     tinyply::Type::UINT32, triangles.size(),
                                     reinterpret_cast<uint8_t*>(faces.data()),
                                     tinyply::Type::UINT8, 3);
    myFile.get_comments().push_back("generated by SimpleMesh+tinyply");
    myFile.write(os, binary);

}//void SimpleMesh::writeStreamPLY (ostream &os) const

#endif

///Write a mesh to a OFF file
void SimpleMesh::writeFileOFF(const std::string &filename) const
{
    //Open the file as a stream
    ofstream os(filename.c_str());
    if (!os.is_open())
        throw filename + string(": Error creating the file");

    //write the mesh as a stream
    writeStreamOFF(os);

    os.close();
}

///Write a mesh to a OFF stream
void SimpleMesh::writeStreamOFF (ostream &os) const {

    //Write the header
    os << "OFF" << "\n";
    os << coordinates.size() << " "  << triangles.size() << " " << 0 << "\n";

    //Write coordinates
    for (const vec3 &v : coordinates)
        os << v.X << " " << v.Y << " " << v.Z << "\n";

    //Write triangles
    for (const SimpleTriangle &t : triangles)
        os << "3 " << t.a << " " << t.b << " " << t.c << "\n";

}//void SimpleMesh::write_OFF (ostream &os) const {

///Write a mesh to a OBJ file
void SimpleMesh::writeFileOBJ(const std::string &filename) const
{
    //Open the file as a stream
    ofstream outStream(filename.c_str());
    if (!outStream.is_open())
        throw filename + string(": Error creating the file");

    writeStreamOBJ(outStream);

    outStream.close();
}

///Write a mesh to a OBJ stream
void SimpleMesh::writeStreamOBJ (ostream &os) const
{
    //Write coordinates
    for (const vec3 &v : coordinates)
        os << "v " << v.X << " " << v.Y << " " << v.Z << "\n";

    //Write triangle indexes (with base 1)
    for (const SimpleTriangle &t : triangles)
        os << "f " << t.a+1 << " " << t.b+1 << " " << t.c+1 << "\n";

}//void SimpleMesh::write_OBJ (ostream &os) const


/// Write a mesh to a matlab file
void SimpleMesh::writeFileMatlab (const std::string &filename) const
{
    //Open the file as a stream
    ofstream os(filename.c_str());
    if (!os.is_open())
        throw filename + string(": Error creating the file");

    os << "#Draw this mesh with the command:" << endl
       << "#trisurf (triangles, coordinates (:,1), coordinates (:,2), coordinates (:,3) )" << endl;

    //Write coordinates
    os << "coordinates=[ \n";
    for (const vec3 &v : coordinates)
        os << v.X << " , " << v.Y << " , " << v.Z << " ;\n";
    os << "]; \n";

    //Write triangles
    os << "triangles=[ \n";
    for (const SimpleTriangle &t : triangles)
        os << t.a+1 << " , " << t.b+1 << " , " << t.c+1 << " ;\n";
    os << "]; \n";

    os.close();

}//void SimpleMesh::writeFileMatlab (const std::string &filename) const

/// Dump a const std::vector<vec3> to an ASCII stream
std::ostream &operator<<(std::ostream &stream, const std::vector<vec3> &xyz)
{
    for (const auto &p : xyz)
        stream << p << endl;
    return stream;
}

/// Dump a const std::vector<SimpleTriangle> to an ASCII stream
std::ostream &operator<<(std::ostream &stream, const std::vector<SimpleTriangle> &tri)
{
    for (const auto &t : tri)
        stream << t << endl;
    return stream;
}

///Build an adjacency list as a sorted list of neighbours per vertex
void SimpleMesh::computeNeighbours(std::vector<std::vector<int> > &neighbours)
{
    //See more at https://www.quora.com/What-is-the-best-way-to-implement-graphs-in-C++
    //As alternative, we could use std::vector<std::set<int> >
    neighbours.clear();
    neighbours.resize(numVertex());
    for(const SimpleTriangle &t : triangles)
    {
        //Must insert both orientations, to not miss reverse edges in the boundary
        //But this will create duplicate entries in the internal edges.
        neighbours[t.a].push_back(t.b);
        neighbours[t.a].push_back(t.c);
        neighbours[t.b].push_back(t.a);
        neighbours[t.b].push_back(t.c);
        neighbours[t.c].push_back(t.a);
        neighbours[t.c].push_back(t.b);
    }

    //Sort and remove duplicate neighbours
    for (size_t i =0; i <numVertex(); i++)
    {
        std::sort(neighbours[i].begin(), neighbours[i].end() );
        auto n = std::unique(neighbours[i].begin(), neighbours[i].end() );
        neighbours[i].resize(n - neighbours[i].begin());
    }

}//void computeNeighbours( ...)


/// Count the number of vertex non-referenced by any triangle
size_t SimpleMesh::checkUnreferencedVertex (const bool verbose) const
{
    //Create a vector with the number of references of each vertex
    std::vector<size_t>valences = vertexValences();

    //Check Valences
    size_t numZeros=0;
    for (size_t i=0; i < valences.size(); i++)
    {
        if (valences[i] == 0)
        {
            numZeros++;
            if (verbose)
                cout << "Vertex " << i << " has valence 0"<< endl;
        }
    }//for

    return numZeros;
}//size_t checkUnreferencedVertex (const bool verbose=false) const

/// Search and remove the vertex non-referenced by any triangle
size_t SimpleMesh::removeUnreferencedVertex (const bool verbose)
{

    //Create a vector with the number of references of each vertex
    std::vector<size_t>valences = vertexValences();

    //Check Valences and create a correspondence map from original mesh to reduced mesh
    std::vector<int>vertexMap(numVertex(), -1);
    size_t numZeros=0;
    int numValid=0;
    bool doCopyCoordinates = false;

    for (size_t i=0; i < numVertex(); i++)
    {
        if (valences[i] > 0)
        {
            vertexMap[i]=numValid;

            //Copy vertex coordinates to new position
            if (doCopyCoordinates)
            {
                coordinates[numValid]=coordinates[i];
                if (verbose)
                    cout << "copy coordinates of vertex " << i << " to " << numValid << endl;
            }

            //Next valid vertex
            numValid++;
        }
        else
        {
            numZeros++;
            //Change flag to star to copy coordinates
            doCopyCoordinates = true;
            if (verbose)
                cout << "Vertex " << i << " has valence 0"<< endl;
        }

    }//for

    //Resize coordinate container
    coordinates.resize(numValid);

    if (verbose)
        for (size_t i = 0; i < vertexMap.size(); i++)
        {
            cout << "Vertex " << i << " mapped to " << vertexMap[i] << endl;
        }

    //Change the references to coordinates in each triangle using the map
    for (SimpleTriangle &t : triangles)
    {
        t.a = vertexMap[t.a];
        t.b = vertexMap[t.b];
        t.c = vertexMap[t.c];
    }

    return numZeros;
}//size_t solveUnreferencedVertex (const bool verbose=false)

/// Remove triangles whose area is equal or smaller than a threshold
/// Return the number of removed triangles
// Note: This method may not preserve the conectivity of the mesh,
// often will create holes and let unreferenced vertex in the mesh
size_t SimpleMesh::removeSmallTriangles(const double minArea)
{
    size_t nTriangles=triangles.size();

    //This is the new c++11 filter way. But it is not well readable...
    auto lambda = [this, minArea](const SimpleTriangle& t) { return triangleArea(t) < minArea; };
    triangles.erase(remove_if(triangles.begin(), triangles.end(), lambda), triangles.end());

    /* Filtering the old way...
    //Create a new list of triangles by filtering the current
    std::vector<SimpleTriangle> newTriangles;
    newTriangles.reserve(nTriangles);
    for (const SimpleTriangle & t : triangles )
    {
        double area = triangleArea(t);
        if (area >= minArea)
            newTriangles.push_back(t);
    }

    if (newTriangles.size() != nTriangles)
        triangles = newTriangles;
    */

    return nTriangles - triangles.size();
}//size_t removeSmallTriangle(const double areaThreshold = 0.0, const bool verbose=false)


#endif
