## Overview
Collection of algorithms with triangle meshes, written in C++ and organized as four Visual Studio projects. They cover mesh statistics, boundary detection with distance fields, and UV parameterization using barycentric mapping, using Eigen for both dense and sparse linear algebra.

## Projects
_**meshStatistic**_
* Statistics over the whole mesh:
  * Minimum, maximum and average triangle area, plus total surface area
  * Minimum and maximum internal angles
  * Minimum, maximum and average edge length
  * Minimum, maximum and average shape factor
  * Mixed Voronoi area per vertex
* Output of a vertex-colored mesh with poorly shaped triangles (those whose shape factor is below a threshold) highlighted in red
<p align = "center"> 
 <img width="259" height="400" alt="athena mesh" src="https://github.com/user-attachments/assets/7f7485c8-470b-45e6-b46a-c26209e1470e" />
 <img width="444" height="400" alt="Bunny mesh" src="https://github.com/user-attachments/assets/d0b4183a-69de-4a88-ab79-75b98eb40fd8" />
</p>

_**meshBoundary**_
* Classifies edges as boundary or internal
* Boundary edges are reordered into ordered closed loops
* Calculates surface distance from every vertex to the nearest boundary vertex through multi-source Dijkstra's algorithm
* Temperature-colored output, going from red at the boundary to blue far from it
* Supports closed meshes without boundary, where all vertices are painted white
* Supports meshes with multiple boundaries
* Implementation is optimized, and has proven to be capable of processing meshes with ~1,000,000 vertices in less than 5 seconds (Release mode)
<p align = "center">
 <img width="325" height="400" alt="angel mesh" src="https://github.com/user-attachments/assets/384e1f80-e997-4163-88d9-6ce1778a319c" />
 <img width="320" height="400" alt="nefertiti mesh" src="https://github.com/user-attachments/assets/a77eb1f2-ba2c-4d23-a473-ad95b7057a43" />
</p>

_**meshTexture-Dense**_
* UV parameterization of meshes topologically equivalent to a disk, flattening the 3D surface onto a unit square
* Boundary extracted and ordered automatically
* Generates a textured mesh, viewable in MeshLab with a checker texture to inspect the result
* Limitation: memory grows quadratically with the number of vertices because it relies on dense matrices

_**meshTexture-Sparse**_
* Same functionality as the dense version, rewritten to scale to much larger meshes thanks to sparse matrices
* Drastically reduced memory usage and computation time; meshes with ~50,000 vertices are processed in under two seconds
<p align = "center">
 <img width="330" height="400" alt="mannequin mesh" src="https://github.com/user-attachments/assets/306092f9-19ec-4420-bd56-fcb2530ac8ba" />
 <img width="418" height="400" alt="lion mesh" src="https://github.com/user-attachments/assets/b85e8056-f3bc-4c2f-aca1-55d85acf4a33" />
</p>

## Usage
* Open any of the `.vcxproj` projects in Visual Studio
* Run the project (the input mesh can be changed by editing the path in the source code)
* Open the files generated in the `output` folder in any mesh visualizer to see the results

## Requirements
* Visual Studio
* MeshLab or any other mesh visualizer
