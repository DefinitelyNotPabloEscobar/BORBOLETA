////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_CAMERA_HPP
#define MGL_CAMERA_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace mgl {

class Camera;

///////////////////////////////////////////////////////////////////////// Camera

class Camera {
private:
  GLuint UboId;
  glm::mat4 ViewMatrix;
  glm::mat4 ProjectionMatrix;

public:
  Camera(GLuint bindingpoint);
  ~Camera();
  glm::mat4 getViewMatrix();
  void setViewMatrix(const glm::mat4 &viewmatrix);
  glm::mat4 getProjectionMatrix();
  void setProjectionMatrix(const glm::mat4 &projectionmatrix);
  void Update(GLuint bindingpoint);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace mgl

#endif /* MGL_CAMERA_HPP */
