/*
 * meshStatistic.cpp
 *
 * Written by Jose Miguel Espadero <josemiguel.espadero@urjc.es>
 *
 * This code is written as material for the FMF class of the
 * Master Universitario en Informatica Grafica, Juegos y Realidad Virtual.
 * Its purpose is to be didactic and easy to understand, not hard optimized.
 *
 * This file compute some statistics about a mesh and dump then to the console.
 * Also produce an output mesh with degenerate triangles remarked.
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
#define _USE_MATH_DEFINES
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <set>
#include <SimpleMesh.hpp>
#include <ColorMesh.hpp>

int main (int argc, char *argv[])
{
    try
    {
        //Set default input mesh filename
        std::string filename("meshes/bunny.ply");
        if (argc >1)
            filename = std::string(argv[1]);

        //Set default degenerate triangle shapeFactor threshold
        double shapeFactorTh = 1/(4*sqrt(3));
        if (argc > 2)
            shapeFactorTh = strtod(argv[2], nullptr);

        ///////////////////////////////////////////////////////////////////////
        //Read a mesh from given filename
        SimpleMesh mesh;
        cout << "Loading file " << filename << endl;
        mesh.readFile(filename);

        cout << "\nNum vertex: " << mesh.numVertex() << " Num triangles: " << mesh.numTriangles()
             << " Unreferenced vertex: " << mesh.checkUnreferencedVertex() << endl;


        //Compute minimum, maximum and average area per triangle, and total area for the mesh
        double minArea = INFINITY, maxArea = -INFINITY, averageArea=0, totalArea = 0;

        //Compute minimum and maximum angle for the mesh
        double minAngle = INFINITY, maxAngle = -INFINITY;

        //Compute minimum, maximum and average edge length
        double minEdgeLen = INFINITY, maxEdgeLen = -INFINITY, averageEdgeLen = 0;

        //Compute minimum, maximum and average shape factor
        double minShapeFactor = INFINITY, maxShapeFactor = -INFINITY, averageShapeFactor = 0;

        //////////////////////////////////////////////////////////////////////////////////
        //TODO 1.1:
        // Compute the values for minArea, maxArea, averageArea, totalArea
        // Compute the values for minAngle, maxAngle;
        // Compute the values for minEdgeLen, maxEdgeLen, averageEdgeLen;
        // Compute the values for minShapeFactor, maxShapeFactor, averageShapeFactor;
        map<SimpleTriangle, vector<double>> angles;
        map<SimpleTriangle, double> areas;
        map<SimpleTriangle, double> shapeFactors;
        for (const SimpleTriangle &t : mesh.triangles) {
            vec3 a = mesh.coordinates[t.a];
            vec3 b = mesh.coordinates[t.b];
            vec3 c = mesh.coordinates[t.c];
            vec3 ab = b - a; ab.normalize();
            vec3 bc = c - b; bc.normalize();
            vec3 ca = a - c; ca.normalize();
            //Areas:
            double area = triangleArea(a, b, c);
            areas[t] = area;
            minArea = min(minArea, area);
            maxArea = max(maxArea, area);
            totalArea += area;
            //Angles: 
            double angleA = acos(ab.dot(-ca));
            angles[t].push_back(angleA);
            double angleB = acos(bc.dot(-ab));
            angles[t].push_back(angleB);
            double angleC = M_PI - angleA - angleB;
            angles[t].push_back(angleC);
            minAngle = min({ minAngle, angleA, angleB, angleC });
            maxAngle = max({ maxAngle, angleA, angleB, angleC });
            //Edges:
            for (const SimpleEdge& e : t.edges()) {
                double length = mesh.edgeLength(e);
                averageEdgeLen += length;
                minEdgeLen = min(minEdgeLen, length);
                maxEdgeLen = max(maxEdgeLen, length);
            }
            //Shape factor:
            double factor = mesh.triangleShapeFactor(t);
            shapeFactors[t] = factor;
            averageShapeFactor += factor;
            minShapeFactor = min(minShapeFactor, factor);
            maxShapeFactor = max(maxShapeFactor, factor);
        }
        averageArea = totalArea / mesh.triangles.size();
        averageEdgeLen /= mesh.triangles.size() * 3;
        averageShapeFactor /= mesh.triangles.size();
        //END TODO 1.1

        // Dump statistics to console output
        cout << std::fixed << std::setprecision(4) <<
                "\nArea    min: " << minArea <<
                " max: " << maxArea  <<
                " average: " << averageArea  <<
                " total: " << totalArea <<
                "\nAngle   min: " << minAngle <<
                " max: " << maxAngle  <<
                "\nEdgeLen min: " << minEdgeLen <<
                " max: " << maxEdgeLen  <<
                " average: " << averageEdgeLen  <<
                "\nShapeF  min: " << minShapeFactor <<
                " max: " << maxShapeFactor  <<
                " average: " << averageShapeFactor  << endl;

        //////////////////////////////////////////////////////////////////////////////////
        //TODO OPTATIVE 1:
        //Compute Vertex area for each vertex and store in vertexAreas vector
        //Compute minimun and maximun vertex area in minVertexArea, maxVertexArea
        std::vector<double>vertexAreas;
        double sumVertexAreas = 0;
        double minVertexArea = INFINITY;
        double maxVertexArea = -INFINITY;
        vertexAreas.resize(mesh.numVertex(), 0);
        
        for (const SimpleTriangle& t : mesh.triangles) {
            vec3 a = mesh.coordinates[t.a];
            vec3 b = mesh.coordinates[t.b];
            vec3 c = mesh.coordinates[t.c];

            double angleA = angles[t][0];
            double angleB = angles[t][1];
            double angleC = angles[t][2];

            if (angleA > M_PI_2 || angleB > M_PI_2 || angleC > M_PI_2) {
                double areaT = areas[t];
                vertexAreas[t.a] += angleA > M_PI_2 ? areaT / 2 : areaT / 4;
                vertexAreas[t.b] += angleB > M_PI_2 ? areaT / 2 : areaT / 4;
                vertexAreas[t.c] += angleC > M_PI_2 ? areaT / 2 : areaT / 4;
            }
            else {
                double ab2 = pow((b - a).module(), 2);
                double bc2 = pow((c - b).module(), 2);
                double ca2 = pow((a - c).module(), 2);
                double cotA = 1.0 / tan(angleA);
                double cotB = 1.0 / tan(angleB);
                double cotC = 1.0 / tan(angleC);

                vertexAreas[t.a] += (1.0 / 8.0) * (ab2 * cotC + ca2 * cotB);
                vertexAreas[t.b] += (1.0 / 8.0) * (bc2 * cotA + ab2 * cotC);
                vertexAreas[t.c] += (1.0 / 8.0) * (ca2 * cotB + bc2 * cotA);
            }
        }
        for (const double& area : vertexAreas) {
            minVertexArea = min(minVertexArea, area);
            maxVertexArea = max(maxVertexArea, area);
            sumVertexAreas += area;
        }
        //END TODO OPTATIVE 1

        //Check values for vertex areas and compute sum of areas:
        if (vertexAreas.size() != mesh.numVertex() )
        {
          cout << "VertexAreas OPTATIVE PART NOT DONE" << endl;
        }
        else
        {
          cout << "VertexA min: " << minVertexArea <<
                  " max: " << maxVertexArea  <<
                  " average: " << sumVertexAreas / mesh.numVertex()  <<
                  " total: " << sumVertexAreas << endl;
        }


        //////////////////////////////////////////////////////////////////////////////////
        //TODO 1.2:
        //Write triangles with ShapeFactor (radius / minEdge) greater than shapeFactorTh
        //Use messages formated as: "Triangle nnn has ShapeFactor xxx"
        map<int, double> lowShapeFactorTriangles;
        for (int i = 0; i < mesh.numTriangles(); i++) {
            double shapeFactor = shapeFactors[mesh.triangles[i]];
            if (shapeFactor < shapeFactorTh) {
                lowShapeFactorTriangles[i] = shapeFactor;
            }
        }
        cout << "\n" << lowShapeFactorTriangles.size() << " triangles with ShapeFactor < " << shapeFactorTh << endl;
        for (const auto& pair : lowShapeFactorTriangles) {
            cout << "Triangle " << pair.first << " has ShapeFactor " << pair.second << endl;
        }
        //END TODO 1.2

        //////////////////////////////////////////////////////////////////////////////////
        //TODO 1.3:
        //Create a colorMesh where faces with ShapeFactor greater than shapeFactorTh
        //have their vertex colored in red. Save it to file named output_statistic.ply
        //and visualize it with meshlab or another external viewer.
        ColorMesh outputMesh;
        outputMesh.coordinates = mesh.coordinates;
        outputMesh.triangles = mesh.triangles;
        outputMesh.colors.resize(mesh.numVertex());
        for (int i = 0; i < outputMesh.triangles.size(); i++) {
            if (lowShapeFactorTriangles.count(i)) {
                SimpleTriangle t = outputMesh.triangles[i];
                outputMesh.colors[t.a].set(1, 0, 0);
                outputMesh.colors[t.b].set(1, 0, 0);
                outputMesh.colors[t.c].set(1, 0, 0);
            }
        }
        string outputFileName = "output/meshStatistic.ply";
        cout << "\nSaving output to " << outputFileName << endl;
        outputMesh.writeFilePLY(outputFileName);
        //END TODO 1.3
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

