/*
 * ColorMesh.hpp
 *
 * Written by Jose Miguel Espadero <josemiguel.espadero@urjc.es>
 *
 * This code is written as example for the FMF class of the
 * Master Universitario en Informatica Grafica, Juegos y Realidad Virtual.
 * Its purpose is to be didactic and easy to understand, not hard optimized.
 *
 * This file extends the SimpleMesh class to add a field to storage one
 * RGB color per vertex. The WriteFileXXX functions are overrided to save
 * the RGB information.
 */

#ifndef _COLORMESH3D_
#define _COLORMESH3D_

#include <SimpleMesh.hpp>
using namespace std;

///Simple class to store a RGB color as 3 float values
class RGBColor
{
    public:
    float r, g, b;

    /// Default constructor. Initialice to white
    inline RGBColor ()
    {
        r = 1.0;
        g = 1.0;
        b = 1.0;
    }

    /// set RGB values from 3 components
    inline void set(float _r, float _g ,float _b)
    {
        r = _r;
        g = _g;
        b = _b;
    }

    /// set RGB values for an array
    inline void set(const float *rgb)
    {
        r = rgb[0];
        g = rgb[1];
        b = rgb[2];
    }

    /// Build a color based from a value in a temperature scale
    void setTemperature(float t, float tmin=0.0, float tmax=1.0)
    {
        //Read more about colormaps:
        //http://cresspahl.blogspot.com.es/2012/03/expanded-control-of-octaves-colormap.html
        //https://jakevdp.github.io/blog/2014/10/16/how-bad-is-your-colormap/

        //Set default color as white
        r = 1.0;
        g = 1.0;
        b = 1.0;

        float dt = tmax - tmin;

        //Clamp value of t to the tmin..tmax interval
        if (t < tmin)
           t = tmin;
        if (t > tmax)
           t = tmax;

        if (t < (tmin + 0.25f * dt)) {
           r = 0;
           g = 4 * (t - tmin) / dt;
        } else if (t < (tmin + 0.5 * dt)) {
           r = 0;
           b = 1 + 4 * (tmin + 0.25f * dt - t) / dt;
        } else if (t < (tmin + 0.75f * dt)) {
           r = 4 * (t - tmin - 0.5f * dt) / dt;
           b = 0;
        } else {
           g = 1 + 4 * (tmin + 0.75f * dt - t) / dt;
           b = 0;
        }
    }// void setTemperature(float t, ...

    /// Build a color based from a value in a greyscale colormap
    void setGrey(float t, float tmin=0.0, float tmax=1.0)
    {

        float dt = tmax - tmin;

        //Clamp value of t to the tmin..tmax interval
        if (t < tmin)
           t = tmin;
        if (t > tmax)
           t = tmax;

        r=g=b=(t-tmin)/dt;
    }// void setGrey(float t, ...

}; //class RGBColor

/// Extension of SimpleMesh to add a container to store a RGBColor per vertex
class ColorMesh : public SimpleMesh
{
    public:

    ///A container for a RGB color per vertex
    std::vector<RGBColor> colors;

    /// Read a mesh from a file. Use filename to guess file format
    void readFile(const std::string &filename, bool debug=false);

    /// Read a mesh from a COFF stream
    void readStreamOFF(istream &is, bool debug=false);

    /// Read a mesh from a COFF stream
    void readStreamPLY(istream &is, bool debug=false);

    /// Write a mesh to a COFF file
    void writeFileOFF(const std::string &filename) const;

    /// Write a mesh to a COFF stream
    void writeStreamOFF (ostream &os) const;

    ///Write a mesh to a PLY file
    void writeFilePLY(const std::string &filename, bool binary=true) const;

    ///Write a mesh to a PLY stream
    void writeStreamPLY(ostream &os, bool binary=true) const;

    /// Write a mesh to a OBJ file
    void writeFileOBJ(const std::string &filename) const;

    /// Write a mesh to a OBJ stream
    void writeStreamOBJ (ostream &os) const;

    /// Write a mesh to a matlab file
    void writeFileMatlab (const std::string &filename) const;

}; //class ColorMesh

/// Read a mesh from a file. Use filename to guess file format
void ColorMesh::readFile(const std::string &filename, bool debug)
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
            readStreamPLY(binStream, debug);
        }
        else
#endif
        {
            readStreamOFF(inStream, debug);
        }
    }
    catch (const string &str) { throw filename + " : " + str; }

}//void ColorMesh::readFile(const std::string &filename, bool debug)

/// Read a mesh from a OFF stream
void ColorMesh::readStreamOFF(istream &is, bool debug)
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

    bool hasColor = false;
    hasColor = (header[0] == 'C');
    if (debug)
        cerr << "File has color info." << std::endl;

    //Read the number of vertex
    int nv, nf, ne;
    if (!(is >> nv >> nf >> ne))
        throw string("error loading number of vertex, faces and edges");

    if (debug)
        cerr << "vertex: " << nv << " faces: " << nf << " edges: " << ne << std::endl;

    //Reserve memory in advance
    this->coordinates.resize(nv);
    this->triangles.resize(nf);
    this->colors.resize(nv);

    //Read the values of vertex
    for (int i=0; i<nv ; i++)
    {
        float x, y, z;
        //Read values for X, Y, Z
        if (!(is >> x >> y >> z))
            throw string("error loading coordinates");

        //Save the values of coordinate
        this->coordinates[i].set(x,y,z);

        if (hasColor)
        {
            float r, g, b, a;
            //Read values for RGB
            if (!(is >> r >> g >> b >> a))
                throw string("error loading colors");

            //Save the values of vertex color
            this->colors[i].set(r,g,b);
        }
        else
        {
            //Set vertex color to white
            this->colors[i].set(1,1,1);
        }

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

}//void ColorMesh::readStreamOFF(istream &is, bool debug)

#ifdef tinyply_h
/// Read a mesh from a PLY stream
void ColorMesh::readStreamPLY(istream &is, bool debug)
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
  std::shared_ptr<tinyply::PlyData> vertices, faces, rgbs;

  // The header information can be used to programmatically extract properties on elements
  // known to exist in the file header prior to reading the data. For brevity of this sample, properties
  // like vertex position are hard-coded:
  vertices = plyFile.request_properties_from_element("vertex", {"x","y","z" });
  faces = plyFile.request_properties_from_element("face", { "vertex_indices" });
  bool PLYhaveRGB=true;
  try
  {
    rgbs = plyFile.request_properties_from_element("vertex", {"red","green","blue"});
  }
  catch (std::invalid_argument &)
  {
    PLYhaveRGB=false;
  }

  // Now populate the vectors...
  plyFile.read(is);

  if (debug)
  {
      std::cout << "\tRead " << vertices->count << " vertices "<< std::endl;
      std::cout << "\tRead " << faces->count << " faces (triangles) " << std::endl;
      std::cout << "\tRead " << rgbs->count << " colors " << std::endl;
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

  //Set colors to this mesh. Convert from uint8_t RGB to float RGB
  this->colors.resize(coordinates.size(), RGBColor());
  if (PLYhaveRGB)
  {
    this->colors.resize(rgbs->count, RGBColor());
    uint8_t *urgb = rgbs->buffer.get();
    for (size_t i=0; i<rgbs->count; i++)
        colors[i].set(urgb[3*i]/255.0f,urgb[3*i+1]/255.0f,urgb[3*i+2]/255.0f);
  }

}//void ColorMesh::readStreamPLY(istream &is, bool debug)

///Write a mesh to a PLY file
void ColorMesh::writeFilePLY(const std::string &filename, bool binary) const
{
    //Open the file as a stream
    ofstream os(filename.c_str(),std::ios::binary);
    if (!os.is_open())
        throw filename + string(": Error creating the file");

    writeStreamPLY(os, binary);

    os.close();
}//void ColorMesh::writeFilePLY(const std::string &filename) const


///Write a mesh to a PLY stream
void ColorMesh::writeStreamPLY (ostream &os, bool binary) const
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

  //Prepare rgbs in serialized uint8_t {r,g,b} format
  std::vector<uint8_t> urgb;
  urgb.reserve(3*colors.size());
  for (const RGBColor &rgb : colors)
  {
      urgb.push_back(uint8_t(255*rgb.r));
      urgb.push_back(uint8_t(255*rgb.g));
      urgb.push_back(uint8_t(255*rgb.b));
  }

  //Dump to a stream with tinyply
  tinyply::PlyFile myFile;
  myFile.add_properties_to_element("vertex", { "x", "y", "z" },
                                   tinyply::Type::FLOAT32, coordinates.size(),
                                   reinterpret_cast<uint8_t*>(verts.data()),
                                   tinyply::Type::INVALID, 0);
  myFile.add_properties_to_element("vertex", { "red", "green", "blue"},
                                   tinyply::Type::UINT8, urgb.size(),urgb.data(),
                                   tinyply::Type::INVALID, 0);
  myFile.add_properties_to_element("face", { "vertex_indices" },
                                   tinyply::Type::UINT32, triangles.size(),
                                   reinterpret_cast<uint8_t*>(faces.data()),
                                   tinyply::Type::UINT8, 3);
  myFile.get_comments().push_back("generated by SimpleMesh+tinyply");
  myFile.write(os, binary);

}//void ColorMesh::writeStreamPLY (ostream &os) const
#endif

///Write a mesh to a OFF file
void ColorMesh::writeFileOFF(const std::string &filename) const
{
    //Open the file as a stream
    ofstream os(filename.c_str());
    if (!os.is_open())
        throw filename + string(": Error creating the file");

    writeStreamOFF(os);

    os.close();
}//void ColorMesh::writeFileOFF(const std::string &filename) const


///Write a mesh to a OFF stream
void ColorMesh::writeStreamOFF (ostream &os) const {

    //Write the header
    os << "COFF" << "\n";
    os << coordinates.size() << " "  << triangles.size() << " " << 0 << "\n";

    //Write coordinates and colors
    for (size_t i = 0; i < coordinates.size(); i++)
    {
        os << coordinates[i].X << " " << coordinates[i].Y << " " << coordinates[i].Z << " ";
        os << colors[i].r << " " << colors[i].g << " " << colors[i].b << " 1" << "\n";
    }

    //Write triangles
    for (const SimpleTriangle &t : triangles)
        os << "3 " << t.a << " " << t.b << " " << t.c << "\n";

}//void ColorMesh::write_OFF (ostream &os) const {

///Write a mesh to a OBJ file
void ColorMesh::writeFileOBJ(const std::string &filename) const
{
    //Open the file as a stream
    ofstream outStream(filename.c_str());
    if (!outStream.is_open())
        throw filename + string(": Error creating the file");

    writeStreamOBJ(outStream);

    outStream.close();

}//void ColorMesh::writeFileOBJ(const std::string &filename) const

///Write a mesh to a OBJ stream
void ColorMesh::writeStreamOBJ (ostream &os) const
{
    //Write coordinates and colors
    for (size_t i = 0; i < coordinates.size(); i++)
    {
        os << "v " << coordinates[i].X << " " << coordinates[i].Y << " " << coordinates[i].Z
           << " "  << colors[i].r << " " << colors[i].g << " " << colors[i].b << "\n";
    }

    //Write triangle indexes (with base 1)
    for (const SimpleTriangle &t : triangles)
        os << "f " << t.a+1 << " " << t.b+1 << " " << t.c+1 << "\n";

}//void ColorMesh::writeStreamOBJ (ostream &os) const


/// Write a mesh to a matlab file
void ColorMesh::writeFileMatlab (const std::string &filename) const
{
    //Open the file as a stream
    ofstream os(filename.c_str());
    if (!os.is_open())
        throw filename + string(": Error creating the file");

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

    //Write colors
    os << "colors=[ \n";
    for (const RGBColor &c : colors)
        os << c.r << " , " << c.g << " , " << c.b << " ;\n";
    os << "]; \n";

    os.close();

}//void ColorMesh::writeFileMatlab (const std::string &filename) const

#endif
