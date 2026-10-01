/*
 * meshBoundary.cpp
 *
 * Written by Jose Miguel Espadero <josemiguel.espadero@urjc.es>
 *
 * This code is written as material for the FMF class of the
 * Master Universitario en Informatica Grafica, Juegos y Realidad Virtual.
 * Its purpose is to be didactic and easy to understand, not hard optimized.
 *
 * This file computes the list of internal and external edges of a mesh
 * and writes the boundary (list of external edges). Then create a copy
 * of the mesh over a colorMesh and assign the red color to the vertex
 * in the boundary.
 * 
 * //TODO: Fill-in your name and email
 * Name of alumn: Carlos Escribano González
 * Email of alumn: c.escribano.2021@alumnos.urjc.es
 * Year: 2025
 * 
 */

#ifdef _MSC_VER
#pragma warning(error: 4101)
#endif

#define _CRT_NONSTDC_NO_DEPRECATE
#include <iostream>
#include <cmath>
#include <SimpleMesh.hpp>
#include <ColorMesh.hpp>
#include <chrono>
#include <queue>
#include <unordered_set>
using namespace std::chrono;

void orderExternalEdges(const SimpleMesh & mesh, std::vector<SimpleEdge>& externalEdges) {
    unordered_set<int> orderedVertices;
    vector<SimpleEdge> tempExternalEdges;
    tempExternalEdges.resize(externalEdges.size());
    int numOrdered = 0;
    unordered_map<int, SimpleEdge> externalEdgeMap; //Stores edges using their first vertex as the key
    for (int i = 0; i < externalEdges.size(); i++) {
        externalEdgeMap[externalEdges[i].a] = externalEdges[i];
    }
    //The algorithm searches for new boundaries while not all edges have been sorted
    while (numOrdered < externalEdges.size()) {
        vec3 lowestXCoordinates{ INFINITY, INFINITY, INFINITY };
        int lowestXIndex = 0;
        for (int i = 0; i < externalEdges.size(); i++) {
            if (orderedVertices.count(externalEdges[i].a)) continue; //Ordered edges aren't taken into account
            vec3 current = mesh.coordinates[externalEdges[i].a];
            if (tie(current.X, current.Y, current.Z) < tie(lowestXCoordinates.X, lowestXCoordinates.Y, lowestXCoordinates.Z)) {
                lowestXCoordinates = current;
                lowestXIndex = i;
            }
        }
        //The path is created using the map 
        tempExternalEdges[numOrdered] = externalEdges[lowestXIndex];
        orderedVertices.insert(tempExternalEdges[numOrdered].a);
        int firstVertex = tempExternalEdges[numOrdered].a;
        numOrdered++;
        for (int i = numOrdered; i < externalEdges.size(); i++) {
            tempExternalEdges[i] = externalEdgeMap[tempExternalEdges[i - 1].b];
            orderedVertices.insert(tempExternalEdges[i].a);
            numOrdered++;
            if (tempExternalEdges[i].b == firstVertex) break;
        }
    }
    externalEdges = move(tempExternalEdges);
}

/// Update the contents of externalEdges and internalEdges 
void updateEdgeLists(const SimpleMesh &mesh, 
                     std::vector<SimpleEdge> &externalEdges,
                     std::vector<SimpleEdge> &internalEdges )
{
    //TODO 2.1: Implement the body of the updateEdgeLists() method
    //First, all internal edges are identified, while external ones are kept in boundaryMap
    unordered_map<int, unordered_set<int>> boundaryMap;
    boundaryMap.reserve(mesh.numVertex());
    for (const SimpleTriangle& t : mesh.triangles) {
        for (const SimpleEdge& edge : t.edges()) {
            if (boundaryMap[edge.a].count(edge.b))
                cout << "Warning: edge [" << edge.a << "->" << edge.b << "] is duplicated. Mesh isn't manifold." << endl;
            
            if (boundaryMap[edge.b].count(edge.a)) {
                boundaryMap[edge.b].erase(edge.a);
                internalEdges.push_back(edge);
            }
            else boundaryMap[edge.a].insert(edge.b);
        } 
    }
    //External edges are extracted from boundaryMap
    for (const auto& item : boundaryMap) {
        if (item.second.empty()) continue;
        for (int b : item.second) {
            SimpleEdge edge;
            edge.set(item.first, b);
            externalEdges.push_back(edge);
        }
    }
    if (!externalEdges.empty()) orderExternalEdges(mesh, externalEdges);
    //END TODO 2.1
}

void calculateBoundDist(const SimpleMesh& mesh, vector<double>& boundDist, 
    const vector<unordered_set<int>> &neighbours, const vector<int>& frontierVertices) 
{
    //For Dijkstra from multiple origin points a priority queue will be used. It will prioritize lower distance elements
    priority_queue<
        pair<double, int>, 
        vector<pair<double, int>>, 
        std::greater<pair<double, int>>
    > queue;
    //Bound vertex are added with dist = 0
    for (const int& v : frontierVertices) {
        boundDist[v] = 0;
        queue.push(make_pair(boundDist[v], v));
    }
    //Vertex are extracted one by one
    while (!queue.empty()) {
        int currentVertex = queue.top().second;
        double currentDist = queue.top().first;
        queue.pop();
        if (boundDist[currentVertex] < currentDist) continue; //Evaluation is finished if stored distance is lower than currentDist
        for (const int& neighbour : neighbours[currentVertex]) {
            double dist = boundDist[currentVertex] + mesh.edgeLength(currentVertex, neighbour);
            if (dist < boundDist[neighbour]) { //If the result is lower than stored value boundDist is updated
                boundDist[neighbour] = dist;
                queue.push(make_pair(boundDist[neighbour], neighbour));
            }
        }
    }
}


int main (int argc, char *argv[])
{
    try
    {
        // Set default input mesh filename
        //std::string filename("meshes/16Triangles.off");
        //std::string filename("meshes/hexagon3.off");
        //std::string filename("meshes/hexagon2.off");
        //std::string filename("meshes/mannequin.ply");
        //std::string filename("meshes/mannequin2.ply");
        //std::string filename("meshes/mask2.ply");
        //std::string filename("meshes/knot-hole.ply");
        //std::string filename("meshes/Nefertiti.990kv.ply");
        //std::string filename("meshes/angel_kneeling.150kv.ply");
        std::string filename("meshes/bunny.ply");

        if (argc > 1)
            filename = std::string(argv[1]);

        ///////////////////////////////////////////////////////////////////////
        //Read a mesh and write a mesh
        SimpleMesh mesh;        
        cout << "Loading file " << filename << endl;
        mesh.readFile(filename, false);

        cout << "Num vertex: " << mesh.numVertex() << " Num triangles: " << mesh.numTriangles()
             << " Unreferenced vertex: " << mesh.checkUnreferencedVertex() << endl;

        //Init time measures used for profiling
        high_resolution_clock::time_point clock0 = high_resolution_clock::now();

        ///////////////////////////////////////////////////////////////////////

        //Compute the list of external and internal edges
        //Implement the body of the updateEdgeLists() method .
        std::vector<SimpleEdge> externalEdges;
        std::vector<SimpleEdge> internalEdges;
        updateEdgeLists(mesh, externalEdges, internalEdges);
 
        cout << "Done updateEdgeLists() " << duration<float>(high_resolution_clock::now() - clock0).count() << " seconds" << endl;
        cout << externalEdges.size() << " boundary edges" << endl;
        cout << internalEdges.size() << " internal edges" << endl;

        //Write external boundary and compute boundary length
        if (externalEdges.size() > 0) {
            double boundaryLength = 0.0;
            cout << "Edges in the boundary:" << endl;
            for (const SimpleEdge& e : externalEdges)
            {
                cout << "[" << e.a << "->" << e.b << "] ";
                boundaryLength += mesh.edgeLength(e);
            }
            cout << endl;
            cout << "Boundary length: " << boundaryLength << endl;
        }
        //Compute the Euler Characteristic for this mesh
        //For a conected mesh, the value means:
        //2 -> The mesh is a closed surface with no holes (sphere-like topology)
        //1 -> The mesh has one hole (disk-like topology)
        //0 -> The mesh has one handle (torus-like topology)
        //-2 -> The mesh has two holes (tube-like topology)
        int eulerCharacteristic = mesh.numVertex() + mesh.numTriangles()
                                - externalEdges.size() - internalEdges.size();
        cout << "Euler Characteristic: " << eulerCharacteristic << endl;


        //Vector to store the distance from a vertex to the nearest vertex in the boundary
        std::vector<double> boundDist;
        //Index of the vertex with max distance to boundary
        unsigned deepestVertex = 0;

        //TODO 2.2:
        //Compute the distance from each vertex to the nearest vertex in the boundary (euclidean distance measured along edges)
        //and store it in the boundDist vector.
        //Store in deepestVertex the index of the vertex with the maximum distance to boundary
        if (!externalEdges.empty()) {
            boundDist.resize(mesh.numVertex(), INFINITY);
            vector<std::unordered_set<int>> neighbours;
            neighbours.resize(mesh.numVertex());
            for (const SimpleTriangle& t : mesh.triangles) {
                neighbours[t.a].insert(t.b);
                neighbours[t.a].insert(t.c);
                neighbours[t.b].insert(t.a);
                neighbours[t.b].insert(t.c);
                neighbours[t.c].insert(t.a);
                neighbours[t.c].insert(t.b);
            }
            vector<int> externalVertices;
            for (const SimpleEdge& e : externalEdges) {
                externalVertices.push_back(e.a);
            }
            calculateBoundDist(mesh, boundDist, neighbours, externalVertices);
            for (int i = 0; i < boundDist.size(); i++) {
                if (boundDist[i] > boundDist[deepestVertex]) deepestVertex = i;
            }

            //END TODO 2.2
            cout << "Done boundDist() " << duration<float>(high_resolution_clock::now() - clock0).count() << " seconds" << endl;

            //Dump the index and distance for the deepestVertex
            cout << "maxDistance to boundary: " << boundDist[deepestVertex] << " at vertex: " << deepestVertex << endl;

            //Dump distances to boundary (for small meshes) for debugging
            if (boundDist.size() < 40)
            {
                cout << "Distances to boundary: " << endl;
                for (size_t i = 0; i < boundDist.size(); i++)
                    cout << "vertex: " << i << " : " << boundDist[i] << endl;
                cout << endl;
            }
        }

        std::string outputFilename="output/boundary.ply";
        cout << "Saving output to " << outputFilename << endl;

        //TODO 2.3:
        //Create a color mesh where the color of each vertex shows its distance to boundary
        //Save the color mesh to a PLY file named "output_boundary.ply"
        //See meshColor.cpp or meshColor2.cpp to see an how-to example
        ColorMesh outputMesh;
        outputMesh.triangles = move(mesh.triangles);
        outputMesh.coordinates = move(mesh.coordinates);
        outputMesh.colors.resize(outputMesh.numVertex());
        if (!externalEdges.empty()) {
            double maxDist = boundDist[deepestVertex];
            for (int i = 0; i < outputMesh.numVertex(); i++) {
                outputMesh.colors[i].setTemperature(-boundDist[i], -maxDist, 0);
            }
        }
        outputMesh.writeFilePLY(outputFilename);
        //END TODO 2.3
        cout << "Done colorize by distance to boundary " << duration<float>(high_resolution_clock::now() - clock0).count() << " seconds" << endl;

        //Visualize the file with an external viewer
#ifdef WIN32
        //string viewcmd = "\"C:\\Program Files (x86)\\VCG\\MeshLab\\meshlab.exe\"";
        string viewcmd = "C:/meshlab/meshlab_32.exe";
#else
        string viewcmd = "meshlab >/dev/null 2>&1 ";
#endif
        string cmd = viewcmd+" "+outputFilename;
        cout << "Executing external command: " << cmd << endl;
        return system(cmd.c_str());
    }

    catch (const string &str) { std::cerr << "EXCEPTION: " << str << std::endl; }
    catch (const char *str) { std::cerr << "EXCEPTION: " << str << std::endl; }
    catch (std::exception& e)    { std::cerr << "EXCEPTION: " << e.what() << std::endl;  }
    catch (...) { std::cerr << "EXCEPTION (unknow)" << std::endl; }

#ifdef WIN32
    cout << "Press Return to end the program" <<endl;
    cin.get();
#else
#endif

    return 0;
}

