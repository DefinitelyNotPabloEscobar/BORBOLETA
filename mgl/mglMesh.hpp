////////////////////////////////////////////////////////////////////////////////
//
// Mesh Loader Class
//
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_MESH_HPP
#define MGL_MESH_HPP

#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace mgl {

class Mesh;

////////////////////////////////////////////////////////////////////// IDrawable

class IDrawable {
public:
  virtual void draw(void) = 0;
};

/////////////////////////////////////////////////////////////////////////// Mesh

class Mesh : public IDrawable {

public:
  static const GLuint INDEX = 0;
  static const GLuint POSITION = 1;
  static const GLuint NORMAL = 2;
  static const GLuint TEXCOORD = 3;
  static const GLuint TANGENT = 4;

  explicit Mesh();
  ~Mesh();

  void setAssimpFlags(unsigned int flags);
  void joinIdenticalVertices();
  void generateNormals();
  void generateSmoothNormals();
  void generateTexcoords();
  void calculateTangentSpace();
  void flipUVs();

  void create(const std::string &filename);
  void draw() override;

  bool hasNormals();
  bool hasTexcoords();
  bool hasTangentsAndBitangents();
  void destroyBufferObjects();

private:
  GLuint VaoId;
  unsigned int AssimpFlags;
  bool NormalsLoaded, TexcoordsLoaded, TangentsAndBitangentsLoaded;

  struct MeshData {
    unsigned int nIndices = 0;
    unsigned int baseIndex = 0;
    unsigned int baseVertex = 0;
  };
  std::vector<MeshData> Meshes;

  std::vector<glm::vec3> Positions;
  std::vector<glm::vec3> Normals;
  std::vector<glm::vec2> Texcoords;
  std::vector<glm::vec3> Tangents;
  std::vector<unsigned int> Indices;

  void processScene(const aiScene *scene);
  void processMesh(const aiMesh *mesh);
  void createBufferObjects();
};

////////////////////////////////////////////////////////////////////////////////
} // namespace mgl

#endif /* MGL_MESH_HPP  */
