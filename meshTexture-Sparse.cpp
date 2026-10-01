/*
 * meshTexture-Sparse.cpp
 *
 * Written by Jose Miguel Espadero <josemiguel.espadero@urjc.es>
 *
 * This code is written as material for the FMF class of the
 * Master Universitario en Informatica Grafica, Juegos y Realidad Virtual.
 * Its purpose is to be didactic and easy to understand, not hard optimized.
 * 
 * //TODO: Fill-in your name and email
 * Name of alumn: Carlos Escribano González
 * Email of alumn: c.escribano.2021@alumnos.urjc.es
 * Year: 2025
 * 
 */

// This file is another solution to the meshTexture exercise.
// Use a sparse matrix, and a sparse solver to solve the system. It can successfully
// compute huge mesh efficiently.

#define _CRT_NONSTDC_NO_DEPRECATE
#define M_PI 3.14159265358979323846
#include <iostream>
#include <iomanip>
#include <chrono>
#include <unordered_set>
using namespace std::chrono;

#include <SimpleMesh.hpp>
#include <TextureMesh.hpp>

//Check if Eigen  (a standar Matrix Library) is included. If not,
//you can get it at http://eigen.tuxfamily.org

// <Eigen/Dense> is the module for dense (traditional) matrix and vector.
// You can get a quick reference for using Eigen dense objects at:
// http://eigen.tuxfamily.org/dox/group__QuickRefPage.html
#include <Eigen/Dense>

// <Eigen/Sparse> is the module for sparse matrix and vectors, which
// are used when most of elements of the matrix will store a 0.0 value.
// You can get a quick reference for using sparse objects at:
// http://eigen.tuxfamily.org/dox/group__SparseQuickRefPage.html
#include <Eigen/Sparse>
using Eigen::SparseMatrix;

/// Write an Eigen Matrix to a matlab file
void exportDenseToMatlab (const Eigen::MatrixXd &m, const std::string &filename, const std::string matrixName ="A")
{
    cout << "Export matrix "<< matrixName << " to file: " << filename << std::endl;

    //Open the file as a stream
    ofstream os(filename.c_str());
    if (!os.is_open())
        throw filename + string(": Error creating the file");


    os << "# name: "<<matrixName << std::endl
       << "# type: matrix" << std::endl
       << "# rows: " << m.rows() << std::endl
       << "# columns: " << m.cols() << std::endl;
    Eigen::IOFormat fmt(Eigen::StreamPrecision, Eigen::DontAlignCols, " ", "\n", "", "", "", "\n");
    os << m.format(fmt);

    os.close();
    std::cout << "To import "<< matrixName <<" into matlab use the command: load(\""<<filename<<"\")"<<std::endl;

}//void exportDenseToMatlab (const &MatrixXd m, const std::string &filename)

/// Write a sparse Eigen Matrix to a matlab file
void exportSparseToMatlab (const Eigen::SparseMatrix<double> &m, const std::string &filename, const std::string matrixName ="A")
{
    cout << "Export sparse matrix "<< matrixName << " to file: " << filename << std::endl;

    //Open the file as a stream
    ofstream os(filename.c_str());
    if (!os.is_open())
        throw filename + string(": Error creating the file");

    os << "# name: "<<matrixName << std::endl
       << "# type: sparse matrix" << std::endl
       << "# nnz: " << m.nonZeros() << std::endl
       << "# rows: " << m.rows() << std::endl
       << "# columns: " << m.cols() << std::endl;

    for (int k=0; k<m.outerSize(); ++k)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(m,k); it; ++it)
        {
            os << 1+it.row() << " " << 1+it.col() << " " << it.value() << std::endl;
        }
    }
    os.close();
    std::cout << "To import "<< matrixName <<" into matlab use the command: load(\""<<filename<<"\")"<<std::endl;
}//void exportSparseToMatlab (const Eigen::SparseMatrix<double> &m, ...

void orderExternalEdges(const SimpleMesh& mesh, std::vector<SimpleEdge>& externalEdges) {
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
    externalEdges = tempExternalEdges;
}

/// Update the contents of externalEdges and internalEdges 
void updateEdgeLists(const SimpleMesh& mesh,
    std::vector<SimpleEdge>& externalEdges,
    std::vector<SimpleEdge>& internalEdges)
{
    //TODO 2.1: Copy the body of the updateEdgeLists() method from meshBoundary
    //First, all internal edges are identified, while external ones are kept in boundaryMap
    unordered_map<int, unordered_set<int>> boundaryMap;
    for (const SimpleTriangle& t : mesh.triangles) {
        for (const SimpleEdge& edge : t.edges()) {
            if (boundaryMap[edge.a].count(edge.b))
                cout << "Aviso: arista [" << edge.a << "->" << edge.b << "] repetida. La malla no es manifold." << endl;

            if (boundaryMap[edge.b].count(edge.a)) {
                boundaryMap[edge.b].erase(edge.a);
                internalEdges.push_back(edge);
            }
            else boundaryMap[edge.a].insert(edge.b);
        }
    }
    //External edges are extracted from boundaryMap
    for (const auto& item : boundaryMap) {
        if (item.second.size() == 0) continue;
        for (int b : item.second) {
            SimpleEdge edge;
            edge.set(item.first, b);
            externalEdges.push_back(edge);
        }
    }
    if (externalEdges.size() != 0) orderExternalEdges(mesh, externalEdges);
    //END TODO 2.1
}//void updateEdgeLists()

void addCotInMap(map<pair<int, int>, double> &resultMap, unsigned int a, unsigned int b, double cot) {
    //Cotangents are added to a map that simulates triplets, using vertex index as the value
    resultMap[make_pair(a, b)] += cot;
    resultMap[make_pair(b, a)] += cot;
    resultMap[make_pair(a, a)] -= cot;
    resultMap[make_pair(b, b)] -= cot;
}

int main (int argc, char *argv[])
{
    //Set dumpMatrix to true for debugging. Writting matrix to file can take some time
    bool dumpMatrix = false;

    try
    {
        // Set default input mesh filename
        //std::string filename("meshes/16Triangles.off");  //Minimal case test
        //std::string filename("meshes/mask2.ply");      //Easy case test
        //std::string filename("meshes/mannequin2.ply"); //Medium case test
        //std::string filename("meshes/gargoyle50k.ply"); //Really hard for dense matrix
        std::string filename("meshes/maxplanck.45kv.ply");
        if (argc > 1)
            filename = std::string(argv[1]);

        ///////////////////////////////////////////////////////////////////////
        //Step 1.
        //Read an input mesh
        SimpleMesh mesh;
        cout << "Loading file " << filename << endl;
        mesh.readFile(filename, false);

        cout << "Num vertex: " << mesh.numVertex() << " Num triangles: " << mesh.numTriangles()
             << " Unreferenced vertex: " << mesh.checkUnreferencedVertex() << endl;

        //Set dumpMatrix to true for debugging. Writting matrix to file can take some time
        dumpMatrix = dumpMatrix || (mesh.numVertex() < 40);

        //Time measure
        high_resolution_clock::time_point clock0 = high_resolution_clock::now();

        ///////////////////////////////////////////////////////////////////////
        //Step 2.
        //Compute edge list and show it. This step should work if you correctly
        //finished the meshBoundary exercise.
        std::vector<SimpleEdge> externalEdges;
        std::vector<SimpleEdge> internalEdges;
        updateEdgeLists(mesh, externalEdges, internalEdges);

        //Count num of internal and external vertex
        size_t numVertex = mesh.coordinates.size();
        size_t numOfExternalVertex = externalEdges.size();
        //size_t numOfInternalVertex = numVertex - numOfExternalVertex;


        //Dump external edges
        cout << numOfExternalVertex << " vertex in external boundary: " << endl;
        if (numOfExternalVertex < 80)
        {
            for (auto &e : externalEdges)
            {
                cout << "[" << e.a << "->" << e.b << "] ";
            }
            cout << endl;
        }

        //Build the list of external vertex
        std::vector<size_t>externalVertex;
        externalVertex.reserve(externalEdges.size());
        for (auto &e : externalEdges)
        {
            externalVertex.push_back(e.a);
        }

        //Optimization: Keep a lookup table to ask if one vertex is external
        std::vector<bool>isExternal(numVertex, false);
        for(size_t i=0; i< numOfExternalVertex; i++)
        {
            isExternal[externalVertex[i]] = true;
        }

        cout << "Done compute Boundary. " << duration<float>(high_resolution_clock::now() - clock0).count() << " seconds" << endl;

        ///////////////////////////////////////////////////////////////////////
        //Step 3.
        //Build Laplace matrix (step 1)
        cout << "Computing Laplacian matrix (sparse):" << endl;

        //We will use a sparse  matrix. If you want to use sparse matrix
        //you have to check http://eigen.tuxfamily.org/dox/group__TutorialSparse.html
        //and declare a sparse matrix instead.
        //Eigen::MatrixXd meshMatrix(numVertex, numVertex);
        Eigen::SparseMatrix<double>meshMatrix (numVertex, numVertex);

        //TODO 3.1: Build the cotangent Laplacian matrix
        //
        //      /\         .  Lij = cot(α) + cot(β) , if vertex i neighbour of j
        //     /β \        .
        //    /    \       .  Lii = -Sum Lij , diagonal element is the sum of row
        //   /      \      .
        // vi--------vj    .
        //   \      /      .
        //    \    /       .
        //     \α /        .
        //      \/         .
        //
        map<pair<int, int>, double> resultMap; //This map will be useful for generating triplets, and will be converted to a matrix
        for (const SimpleTriangle &t : mesh.triangles) {
            vec3 ab = mesh.coordinates[t.b] - mesh.coordinates[t.a];
            vec3 bc = mesh.coordinates[t.c] - mesh.coordinates[t.b];
            vec3 ca = mesh.coordinates[t.a] - mesh.coordinates[t.c];
            ab.normalize();
            bc.normalize();
            ca.normalize();

            double angleA = acos(ab.dot(-ca));
            double angleB = acos(bc.dot(-ab));
            double angleC = M_PI - angleA - angleB;

            double cotA = 1 / tan(angleA);
            double cotB = 1 / tan(angleB);
            double cotC = 1 / tan(angleC);

            addCotInMap(resultMap, t.a, t.b, cotC);
            addCotInMap(resultMap, t.b, t.c, cotA);
            addCotInMap(resultMap, t.c, t.a, cotB);
        }
        //END TODO 3.1

        cout << "Done Laplacian matrix (sparse). " << duration<float>(high_resolution_clock::now() - clock0).count() << " seconds" << endl;

        if (dumpMatrix)
            exportSparseToMatlab(meshMatrix, "laplacian.mat", "L");

        ///////////////////////////////////////////////////////////////////////
        //Step 5.1
        //Build system matrix
        cout << "Computing System matrix:" << endl;

        //TODO 3.2: Patch Laplace matrix to generate a valid system of equations 
        //Triplets are generated from the map while the patching process is performed
        vector<Eigen::Triplet<double>> triplets;
        for (auto& item : resultMap) {
            int i = item.first.first;
            int j = item.first.second;
            double value = item.second; 
            if (isExternal[i]) triplets.emplace_back(i, j, i == j ? 1 : 0);
            else triplets.emplace_back(i, j, value);
        }
        meshMatrix.setFromTriplets(triplets.begin(), triplets.end()); //Sparse matrix is generated
        //END TODO 3.2

        cout << "Done System matrix. " << duration<float>(high_resolution_clock::now() - clock0).count() << " seconds" << endl;

        if (dumpMatrix)
            exportSparseToMatlab(meshMatrix, "systemMatrix.mat", "A");

        ///////////////////////////////////////////////////////////////////////
        //Step 5.2
        //Build UV values for external vertex, using the boundary of a square.
        cout << "Computing contour conditions:" << endl;

        //Note that this is a matrix of numvertex rows and 2 columns
        Eigen::MatrixX2d UV_0(numVertex,2);
        UV_0.setZero();
        
        //TODO 3.3: Build a set of valid UV values for external vertex, mapping
        //the vertex to the boundary of a square of side unit.
        //This step's procedure is the same as for a dense matrix
        double step = 4.0 / numOfExternalVertex; //Assuming a square whose sides' lenght is 1
        for (int i = 0; i < numOfExternalVertex; i++) {
            double t = step * i;
            double u, v = 0;
            if (t < 1) {
                u = t;
                v = 0;
            }
            else if (t < 2) {
                u = 1;
                v = t - 1;
            }
            else if (t < 3) {
                u = 3 - t;
                v = 1;
            }
            else {
                u = 0;
                v = 4 - t;
            }
            int vertexIndex = externalVertex[i];
            UV_0(vertexIndex, 0) = u;
            UV_0(vertexIndex, 1) = v;
        }
        //END TODO 3.3

        cout << "Done contour conditions. " << duration<float>(high_resolution_clock::now() - clock0).count() << " seconds" << endl;

        if (dumpMatrix)
            exportDenseToMatlab(UV_0, "boundaryUVO.mat", "UV0");

        ///////////////////////////////////////////////////////////////////////
        //Step 6

        //We will use Eigen to solve the matrix from here. 
        cout << "Solving the system using Eigen (sparse):" << endl;

        //Solve Ax = b; where A = meshMatrix, x=UV,  and b = UV_0
        Eigen::MatrixX2d UV;
		//TODO 3.4: Solve the system using the sparse matrix. Leave solution in UV
        //Check http://eigen.tuxfamily.org/dox/group__TopicSparseSystems.html
        Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
        solver.compute(meshMatrix);
        UV = solver.solve(UV_0);
        //END TODO 3.4
        cout << "Done solving the system. " << duration<float>(high_resolution_clock::now() - clock0).count() << " seconds" << endl;

        //Dump the computed solution to the system
        if (dumpMatrix)
            exportDenseToMatlab(UV, "solutionUV.mat", "UV");

        ///////////////////////////////////////////////////////////////////////
        //Step 6.4 

        //Create a planar mesh (reverse UV mesh)
        SimpleMesh planarMesh;
        //Use UV values as geometrical coordinates and same triangles that input mesh
        planarMesh.coordinates.resize(numVertex);
        for (size_t i=0; i< numVertex; i++)
        {
            planarMesh.coordinates[i].set(UV(i,0), UV(i,1), 0);
        }
        planarMesh.triangles = mesh.triangles;

        //string output_UVMesh1="output/UVMesh1.ply";
        //cout << "Saving parameterization mesh to " << output_UVMesh1 << endl;
        ////planarMesh.writeFileOBJ(output_UVMesh1);
        //planarMesh.writeFilePLY(output_UVMesh1);


        //Create a TextureMesh mesh with UV coordinates
        TextureMesh textureMesh;
        textureMesh.coordinates = mesh.coordinates;
        textureMesh.triangles= mesh.triangles;

        //Set image filename to be used as texture
        textureMesh.textureFile= "UVchecker.jpg";

        //Set UV as texture-per-vertex coordinates
        textureMesh.UV.resize(numVertex);
        for (size_t i=0; i< numVertex; i++)
            textureMesh.UV[i].set(float(UV(i,0)), float(UV(i,1)));

        //Dump textureMesh to file (.obj or .ply)
        string output_UVMesh2="output/meshTexture_sparse.ply";
        cout << "Saving texture mesh to " << output_UVMesh2 << endl;
        //textureMesh.writeFileOBJ(output_UVMesh2);
        textureMesh.writeFilePLY(output_UVMesh2);

        //Visualize the file with an external viewer
#ifdef WIN32
        //string viewcmd = "\"C:\\Program Files (x86)\\VCG\\MeshLab\\meshlab.exe\"";
        string viewcmd = "C:/meshlab/meshlab_32.exe";
#else
        string viewcmd = "meshlab >/dev/null 2>&1 ";
#endif
        string cmd = viewcmd+" "+output_UVMesh2;
        cout << "Executing external command: " << cmd << endl;
        return system(cmd.c_str());

    }//try
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

