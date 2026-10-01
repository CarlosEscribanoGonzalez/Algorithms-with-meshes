## Overview
Collection of algorithms with triangle meshes, written in C++ and organized as four Visual Studio projects. They cover mesh statistics, boundary detection with distance fields, and UV parameterization using barycentric mapping, using Eigen for both dense and sparse linear algebra.

## Projects

**meshStatistic**

* Statistics over the whole mesh:
  * Minimum, maximum and average triangle area, plus total surface area
  * Minimum and maximum internal angles
  * Minimum, maximum and average edge length
  * Minimum, maximum and average shape factor
  * Mixed Voronoi area per vertex
* Output of a vertex-colored mesh with poorly shaped triangles (those whose shape factor is below a threshold) highlighted in red

**meshBoundary**

* Classifies edges as boundary or internal
* Boundary edges are reordered into ordered closed loops
* Calculates surface distance from every vertex to the nearest boundary vertex through multi-source Dijkstra's algorithm
* Temperature-colored output, going from red at the boundary to blue far from it
* Supports closed meshes without boundary, where all vertices are painted white
* Supports meshes with multiple boundaries
* Implementation is optimized, and has proven to be capable of processing meshes with ~1,000,000 vertices in less than 5 seconds (Release mode)

**meshTexture-Dense**

* UV parameterization of meshes topologically equivalent to a disk, flattening the 3D surface onto a unit square
* Boundary extracted and ordered automatically
* Generates a textured mesh, viewable in MeshLab with a checker texture to inspect the result
* Limitation: memory grows quadratically with the number of vertices because it relies on dense matrices

**meshTexture-Sparse**

* Same functionality as the dense version, rewritten to scale to much larger meshes thanks to sparse matrices
* Drastically reduced memory usage and computation time; meshes with ~50,000 vertices are processed in under two seconds

## Usage
* Open any of the `.vcxproj` projects in Visual Studio
* Run the project (the input mesh can be changed by editing the path in the source code)
* Open the files generated in the `output` folder in any mesh visualizer to see the results

## Requirements
* Visual Studio
* MeshLab or any other mesh visualizer
