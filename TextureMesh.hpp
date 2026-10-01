/*
 * TextureMesh.hpp
 *
 * Written by Jose Miguel Espadero <josemiguel.espadero@urjc.es>
 *
 * This code is written as material for the FMF class of the
 * Master Universitario en Informatica Grafica, Juegos y Realidad Virtual.
 * Its purpose is to be didactic and easy to understand, not hard optimized.
 *
 * This file extends the SimpleMesh class to add two fields to contain the 
 * list of edges in the boundary and the list of edges internal to the mesh.
 *
 * Year: 2025
 *
 */

#ifndef _TEXTUREMESH3_
#define _TEXTUREMESH3_

#include <SimpleMesh.hpp>

///Simple class to store a float UV coordinate
class uvCoord {
public:
    float u;
    float v;

    uvCoord ()
    {
    }

    uvCoord (const float _u, const float _v)
    {
        u = _u;
        v = _v;
    }

    /// Set the values of the indexes
    void set (const float _u, const float _v)
    {
        u = _u;
        v = _v;
    }

}; //class uvCoord

///Extension of SimpleMesh class to add a list of per-vertex texture coordinates container.
/** Note that there must be one UV value for each vertex coordinate. */
class TextureMesh : public SimpleMesh
{
public:

    ///List of wedge texture coordinates
    std::vector<uvCoord> UV;

    //Filename of texture file
    std::string textureFile;

    /// Read a mesh from a file. Use filename to guess file format
    void readFile(const std::string &filename, bool debug=false);

    /// Read a mesh from a COFF stream
    void readStreamPLY(istream &is, bool debug=false);

    ///Write a mesh to a PLY file
    void writeFilePLY(const std::string &filename, bool binary=true) const;

    ///Write a mesh to a PLY stream
    void writeStreamPLY (ostream &os, bool binary=true) const;

    /// Write a mesh to a OBJ file
    void writeFileOBJ(const std::string &filename) const;

    /// Write a mesh to a OBJ stream
    void writeStreamOBJ (ostream &os, const std::string &mtlFilename) const;

}; // TextureMesh


/// Read a TextureMesh from a file. Use filename to guess file format (PLY, OBJ)
void TextureMesh::readFile(const std::string &filename, bool debug)
{
    //Open the file as a stream
    ifstream inStream(filename.c_str());
    if (!inStream.is_open())
        throw filename + string(": Error opening the file");
    try
    {
#ifdef tinyply_h
        if (filename.length() > 3 && (! filename.compare(filename.length()-3, 3, "ply")))
        {
           //Reopen stream as binary
           inStream.close();
           ifstream binStream(filename.c_str(), ios::binary);
           TextureMesh::readStreamPLY(binStream, debug);
        }
        else
#endif
        {
            TextureMesh::readStreamOBJ(inStream, debug);
        }
    }
    catch (const string &str) { throw filename + " : " + str; }

}//void TextureMesh::readFile(const std::string &filename, bool debug)

#ifdef tinyply_h
/// Read a TextureMesh from a PLY stream
void TextureMesh::readStreamPLY(istream &is, bool debug)
{
    // Read the file and create a std::istringstream suitable
    // for the lib -- tinyply does not perform any file i/o.
    tinyply::PlyFile plyFile;
    plyFile.parse_header(is);

    if (debug)
    {
        for (auto e : plyFile.get_elements())
        {
            std::cout << "element - " << e.name << " (" << e.size << ")" << std::endl;
            for (auto p : e.properties)
                std::cout << "\tproperty - " << p.name << " ("
                          << tinyply::PropertyTable[p.propertyType].str << ")"
                          << std::endl;
        }

        for (auto c : plyFile.get_comments())
            std::cout << "Comment: " << c << std::endl;
    }//if (debug)

    //This is a sample of a ply header for a texture-per-wedge file
    //ply
    //format binary_little_endian 1.0
    //comment VCGLIB generated
    //comment TextureFile UVchecker.jpg
    //element vertex NNNNN
    //property float x
    //property float y
    //property float z
    //element face NNNN
    //property list uchar int vertex_indices
    //property list uchar float texcoord
    //property int texnumber
    //end_header

    //This is a sample of a ply file for a texture-per-vertex file
    //ply
    //format ascii 1.0
    //comment TextureFile UVchecker.jpg
    //element vertex 6
    //property float x
    //property float y
    //property float z
    //property float texture_u
    //property float texture_v
    //element face 4
    //property list uchar int vertex_indices
    //end_header
    //0 -0.942809 0 0.666667 0.000000
    //0 1.88562 0 0.333333 1.000000
    //0.816495 0.471406 0 1.000000 1.000000
    //-0.816495 0.471406 0 0.000000 0.666667
    //1.63299 -0.942809 0 1.000000 0.333333
    //-1.63299 -0.942809 0 0.000000 0.000000
    //3 2 3 0
    //3 3 2 1
    //3 2 0 4
    //3 0 3 5


    // Tinyply 2.0 treats incoming data as untyped byte buffers. It's now
    // up to users to treat this data as they wish. See below for examples.
    std::shared_ptr<tinyply::PlyData> vertices, faces, texcoords;

    // The header information can be used to programmatically extract properties on elements
    // known to exist in the file header prior to reading the data. For brevity of this sample, properties
    // like vertex position are hard-coded:
    vertices = plyFile.request_properties_from_element("vertex", { "x", "y", "z" });
    faces = plyFile.request_properties_from_element("face", { "vertex_indices" });
    texcoords = plyFile.request_properties_from_element("vertex", { "texture_u", "texture_v"});

    // Now populate the vectors...
    plyFile.read(is);

    if (debug)
    {
        std::cout << "\tRead " << vertices->count << " vertices "<< endl;
        std::cout << "\tRead " << faces->count << " faces (triangles) " << endl;
        std::cout << "\tRead " << texcoords->count << " UV "<< endl;
    }

    if (vertices->count != texcoords->count)
        throw string("Number of UV coordinates is not equal to number of vertex coordinates");

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

    //Set UV coordinates to this mesh. Convert from float[2] to UVCoord
    UV.resize(texcoords->count, uvCoord());
    float *uvs = reinterpret_cast<float *>(texcoords->buffer.get());
    for (size_t i=0; i<texcoords->count; i++)
        UV[i].set(uvs[2*i],uvs[2*i+1]);

    /* TINYPLY 1.0
    // Define containers to hold the extracted data. The type must match
    // the property type given in the header. Tinyply will interally allocate the
    // the appropriate amount of memory.
    std::vector<float> verts;
    std::vector<uint32_t> faces;
    std::vector<float> uvs;
    size_t vertexCount=0, faceCount=0, uvCount=0;

    // The count returns the number of instances of the property group. The vectors
    // above will be resized into a multiple of the property group size as
    // they are "flattened"... i.e. verts = {x, y, z, x, y, z, ...}
    vertexCount = plyReader.request_properties_from_element("vertex", { "x", "y", "z" }, verts);

    // Read UV coordinates in a texture-per-vertex schema
    uvCount = plyReader.request_properties_from_element("vertex", { "texture_u", "texture_v"}, uvs);

    // For properties that are list types, it is possibly to specify the expected count (ideal if a
    // consumer of this library knows the layout of their format a-priori). Otherwise, tinyply
    // defers allocation of memory until the first instance of the property has been found
    // as implemented in file.read(ss)
    faceCount = plyReader.request_properties_from_element("face", { "vertex_indices" }, faces, 3);

    // Read UV coordinates in a texture-per-wedge schema
    //uvCount = plyReader.request_properties_from_element("face", { "texcoord" }, uvs, 2);

    // Now populate the vectors...
    plyReader.read(is);

    if (debug)
    {
        cout << "\tRead " << vertexCount << " total vertices (" << verts.size() << " properties)." << endl;
        cout << "\tRead " << uvCount << " total UV (" << uvs.size() << " properties)." << endl;
        cout << "\tRead " << faceCount << " total faces (triangles) (" << faces.size() << " properties)." << endl;
    }

    if (uvCount != vertexCount)
        throw string("Number of UV coordinates is not equal to number of vertex coordinates");
    //*/

}//void TextureMesh::readStreamPLY(istream &is, bool debug)

///Write a TextureMesh to a PLY file
void TextureMesh::writeFilePLY(const std::string &filename, bool binary) const
{
    //Open the file as a stream
    ofstream os(filename.c_str(),std::ios::binary);
    if (!os.is_open())
        throw filename + string(": Error creating the file");

    TextureMesh::writeStreamPLY(os, binary);

    os.close();
}//void TextureMesh::writeFilePLY(const std::string &filename) const


///Write a TextureMesh to a PLY stream
void TextureMesh::writeStreamPLY (ostream &os, bool binary) const
{
    if (coordinates.size() != UV.size())
        throw string("Number of UV coordinates is not equal to number of vertex coordinates");

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

    //Prepare UV coordinates per wedge in serialized float {u,v} format
    std::vector<float> uvs;
    uvs.reserve(2*UV.size());
    for (const uvCoord &uv : UV)
    {
        uvs.push_back(uv.u);
        uvs.push_back(uv.v);
    }


    //This is a sample of a ply header for a texture-per-wedge file
    //ply
    //format binary_little_endian 1.0
    //comment VCGLIB generated
    //comment TextureFile UVchecker.jpg
    //element vertex NNNNN
    //property float x
    //property float y
    //property float z
    //element face NNNN
    //property list uchar int vertex_indices
    //property list uchar float texcoord
    //property int texnumber
    //end_header

    //This is a sample of a ply file for a texture-per-vertex file
    //ply
    //format ascii 1.0
    //comment TextureFile UVchecker.jpg
    //element vertex 6
    //property float x
    //property float y
    //property float z
    //property float texture_u
    //property float texture_v
    //element face 4
    //property list uchar int vertex_indices
    //end_header
    //0 -0.942809 0 0.666667 0.000000
    //0 1.88562 0 0.333333 1.000000
    //0.816495 0.471406 0 1.000000 1.000000
    //-0.816495 0.471406 0 0.000000 0.666667
    //1.63299 -0.942809 0 1.000000 0.333333
    //-1.63299 -0.942809 0 0.000000 0.000000
    //3 2 3 0
    //3 3 2 1
    //3 2 0 4
    //3 0 3 5

    //Dump to a stream with tinyply 2.0
    tinyply::PlyFile myFile;
    myFile.add_properties_to_element("vertex", { "x", "y", "z" },
                                     tinyply::Type::FLOAT32, coordinates.size(),
                                     reinterpret_cast<uint8_t*>(verts.data()),
                                     tinyply::Type::INVALID, 0);
    myFile.add_properties_to_element("vertex", { "texture_u", "texture_v"},
                                     tinyply::Type::FLOAT32, coordinates.size(),
                                     reinterpret_cast<uint8_t*>(uvs.data()),
                                     tinyply::Type::INVALID, 0);
    myFile.add_properties_to_element("face", { "vertex_indices" },
                                     tinyply::Type::UINT32, triangles.size(),
                                     reinterpret_cast<uint8_t*>(faces.data()),
                                     tinyply::Type::UINT8, 3);
    myFile.get_comments().push_back(std::string("TextureFile ")+textureFile);
    myFile.get_comments().push_back("generated by SimpleMesh+tinyply");
    myFile.write(os, binary);

    /* Dump to a stream with tinyply 1.0
    myFile.add_properties_to_element("vertex", { "x", "y", "z" }, verts);
    myFile.add_properties_to_element("vertex", { "texture_u", "texture_v"}, uvs);
    myFile.add_properties_to_element("face", { "vertex_indices" }, faces, 3, PlyProperty::Type::UINT8);
    //This is for and texture-per-wedge schema
    //myFile.add_properties_to_element("face", { "texcoord" }, uvs, 6, PlyProperty::Type::UINT8);
    //myFile.add_properties_to_element("face", { "texnumber" }, texnumber);

    myFile.comments.push_back(std::string("TextureFile ")+textureFile);
    myFile.comments.push_back("generated by tinyply");
    myFile.write(os, binary);
    //*/

}//void TextureMesh::writeStreamPLY (ostream &os) const
#endif

///Write a mesh to a OBJ file
void TextureMesh::writeFileOBJ(const std::string &filename) const
{
    //Prepare a .mtl material file to use the texture file
    std::string mtlFilename = filename+".mtl";
    ofstream mtlStream(mtlFilename.c_str());
    if (!mtlStream.is_open())
        throw mtlFilename + string(": Error creating the .mtl file");

    mtlStream <<
    "newmtl texture\n"
    "  Ka 1.000 1.000 1.000\n"
    "  Kd 1.000 1.000 1.000\n"
    "  d 1.0\n"
    "  illum 2\n"
    "  map_Ka " << textureFile << endl;

    //Open the file as a stream
    ofstream outStream(filename.c_str());
    if (!outStream.is_open())
        throw filename + string(": Error creating the file");

    TextureMesh::writeStreamOBJ(outStream, mtlFilename);

    outStream.close();

}//void TextureMesh::writeFileOBJ(const std::string &filename) const

///Write a TextureMesh to a OBJ stream
void TextureMesh::writeStreamOBJ (ostream &os, const std::string &mtlFilename) const
{
    if (coordinates.size() != UV.size())
        throw string("Number of UV coordinates is not equal to number of vertex coordinates");

    //Write header with material information
    os << "mtllib " << mtlFilename << endl
       << "usemtl texture" << endl;

    //Write coordinates and UV coordinates
    for (size_t i = 0; i < coordinates.size(); i++)
    {
        os << "v " << coordinates[i].X << " " << coordinates[i].Y << " " << coordinates[i].Z << endl
           << "vt " << UV[i].u << " "<< UV[i].v << endl;
    }

    //Write triangle indexes (with base 1)
    for (const SimpleTriangle &t : triangles)
       os << "f " << t.a+1 << "/" << t.a+1 << " "
                  << t.b+1 << "/" << t.b+1 << " "
                  << t.c+1 << "/" << t.c+1 << endl;


}//void TextureMesh::writeStreamOBJ (ostream &os) const


#endif
