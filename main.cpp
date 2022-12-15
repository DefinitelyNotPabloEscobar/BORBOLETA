////////////////////////////////////////////////////////////////////////////////
//
//  Loading meshes from external files
//
//
//
// INTRODUCES:
// MODEL DATA, ASSIMP, mglMesh.hpp
//
////////////////////////////////////////////////////////////////////////////////


// TODO : MUTIPLE SHADERS   SHADER LIST!!!

#include <iostream>
#include <iomanip>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "mgl/mgl.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>	

#include <irrKlang.h>

////////////////////////////////////////////////////////////////////////// SOUND

irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();

////////////////////////////////////////////////////////////////////////// MYAPP

struct Mesh_obj
{
	mgl::Mesh* Mesh = nullptr;
	mgl::ShaderProgram* Shaders = nullptr;
	Mesh_obj* next_pointer = nullptr;
	Mesh_obj() : Mesh(nullptr), Shaders(nullptr), next_pointer(nullptr) {};

} Mesh_obj;


class MyApp : public mgl::App {

public:
  void initCallback(GLFWwindow *win) override;
  void displayCallback(GLFWwindow *win, double elapsed) override;
  void windowSizeCallback(GLFWwindow *win, int width, int height) override;
  void windowCloseCallback(GLFWwindow* win) override;
  void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
private:
  const GLuint POSITION = 0, COLOR = 1, UBO_BP = 0;

  glm::vec3 axis_x = { 1.0f, 0.0f, 0.0f };
  glm::vec3 axis_y = { 0.0f, 1.0f, 0.0f };
  glm::vec3 axis_z = { 0.0f, 0.0f, 1.0f };

  // CAMERA1
  float alfa = 0.0f;
  float beta = 0.0f;
  int accelaration_x = 0;
  int accelaration_y = 0;
  float r = 10.f;
  float zoom = 2.f;
  bool zooming = true;
  bool projection_camera1 = true;

  // CAMERA2
  float alfa2 = 0.0f;
  float beta2 = 0.0f;
  int accelaration_x2 = 0;
  int accelaration_y2 = 0;
  float r2 = 10.f;
  float zoom2 = 2.f;
  bool projection_camera2 = true;

  bool OLD_P_CLICKED = false;
  bool OLD_C_CLICKED = false;

  bool camera1_on = true;

  glm::mat4 c1_ChangingViewMatrix;
  glm::mat4 c2_ChangingViewMatrix;

  double xpos, ypos = 0;
  double old_xpos, old_ypos = 0;
  int button, action, mods;

  mgl::ShaderProgram* Shaders = nullptr;
  mgl::Camera* Camera = nullptr;
  mgl::Camera* Camera2 = nullptr;
  GLint ModelMatrixId;

  static const int MESH_SIZE = 7;

  struct Mesh_obj *Head;
  struct Mesh_obj *Tail;

  void createMeshes();
  void createShaderPrograms();
  void createCamera();
  void drawScene();
  void processMouseMovement(GLFWwindow* win);
  void processKeyInput(GLFWwindow* win);
  void draw_meshs();
};



///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {

  std::string mesh_dir = "../assets/";

  std::string names[MESH_SIZE];

  names[0] = "blue_triangle.obj";
  names[1] = "pink_triangle.obj";
  names[2] = "orange_triangle.obj";
  names[3] = "paralelogram.obj";
  names[4] = "purple_triangle.obj";
  names[5] = "red_triangle.obj";
  names[6] = "cube.obj";  //I had to use this 

  Head = new struct Mesh_obj;
  Tail = new struct Mesh_obj;
  Head->Mesh = new mgl::Mesh();
  Head->Mesh->joinIdenticalVertices();
  Head->Mesh->create(mesh_dir + names[0]);
  Tail = Head;

  for (int i = 1; i < MESH_SIZE; i++) {

	  struct Mesh_obj* obj = new struct Mesh_obj;
	  obj->Mesh = new mgl::Mesh();
	  obj->Mesh->joinIdenticalVertices();
	  obj->Mesh->create(mesh_dir + names[i]);
	  Tail->next_pointer = obj;
	  Tail = obj;
  }



}


///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
  
  // CHNAGE HERE EVERY MESH HAS ITS ONW SHADER
  Shaders = new mgl::ShaderProgram();
  Shaders->addShader(GL_VERTEX_SHADER, "cube-vs.glsl");
  Shaders->addShader(GL_FRAGMENT_SHADER, "cube-fs.glsl");

  Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
  if (Head->Mesh->hasNormals()) {
    Shaders->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
  }
  if (Head->Mesh->hasTexcoords()) {
    Shaders->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
  }
  if (Head->Mesh->hasTangentsAndBitangents()) {
    Shaders->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
  }

  Shaders->addUniform(mgl::MODEL_MATRIX);
  Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
  Shaders->create();

  ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;
}

///////////////////////////////////////////////////////////////////////// CAMERA

glm::mat4 ModelMatrix(1.0f);

// Eye(5,5,5) Center(0,0,0) Up(0,1,0)
const glm::mat4 ViewMatrix1 =
glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));

// Eye(-5,-5,-5) Center(0,0,0) Up(0,1,0)
const glm::mat4 ViewMatrix2 =
glm::lookAt(glm::vec3(-5.0f, -5.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));

// Orthographic LeftRight(-2,2) BottomTop(-2,2) NearFar(1,10)
glm::mat4 ProjectionMatrix1 =
glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 15.0f);

// Perspective Fovy(30) Aspect(640/480) NearZ(1) FarZ(30)
glm::mat4 ProjectionMatrix2 =
glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 1.0f, 30.0f);

void updateMatrices(float ratio) {
	ProjectionMatrix1 = glm::ortho(-2.f * ratio, 2.f * ratio, -2.0f, 2.0f, 1.0f, 15.0f);
	ProjectionMatrix2 = glm::perspective(glm::radians(30.0f), ratio, 1.0f, 30.0f);
}

void MyApp::createCamera() {

  Camera2 = new mgl::Camera(UBO_BP);
  Camera = new mgl::Camera(UBO_BP);
  Camera->setViewMatrix(ViewMatrix1);
  Camera->setProjectionMatrix(ProjectionMatrix2);

}

/////////////////////////////////////////////////////////////////////////// DRAW

const glm::mat4 ChangingModelMatrix = ModelMatrix;

void MyApp::drawScene() {

  if (camera1_on) {

	  /*
	  float x = r * glm::cos(glm::radians(alfa)) * glm::sin(glm::radians(beta));
	  float z = r * glm::sin(glm::radians(alfa)) * glm::sin(glm::radians(beta));
	  float y = r * glm::cos(glm::radians(beta));
	  */

	  glm::vec4 q_vec = { 7.0f, 7.0f, 7.0f, 0.0f };
	  glm::quat q_quart = { 0.f, 7.0f, 7.0f, 7.0f };
	  glm::quat qy = glm::angleAxis(glm::radians(-alfa), axis_y);
	  glm::quat qx = glm::angleAxis(glm::radians(beta), axis_x);
	  glm::quat qtotal = qy * qx;
	  glm::quat q1 = qx * q_quart * glm::inverse(qx);
	  glm::quat q2 = qy * q1 * glm::inverse(qy);
	  glm::vec4 vf4 = glm::rotate(qtotal, q_vec);
	  //std::cout << " vf4 =" << glm::to_string(vf4) << std::endl;
	  //std::cout << " q2 =" << glm::to_string(q2) << std::endl;
	  glm::vec3 vf3 = { vf4.x, vf4.y, vf4.z };
	  glm::vec3 vf3_a = { q2.x, q2.y, q2.z };
	  //std::cout << " vf3 =" << glm::to_string(vf3) << std::endl;
	  //std::cout << " vf3_a =" << glm::to_string(vf3_a) << std::endl;

	  const glm::mat4 ChangingViewMatrix =
		  glm::lookAt(vf3_a, glm::vec3(0.0f, 0.0f, 0.0f),
			  glm::vec3(0.0f, 1.0f, 0.0f));

	  Camera->setViewMatrix(ChangingViewMatrix * glm::scale(glm::vec3(1.0f * zoom)));
	  if (projection_camera1)
		  Camera->setProjectionMatrix(ProjectionMatrix2);
	  else
		  Camera->setProjectionMatrix(ProjectionMatrix1);

  }
  else {

	  /*
	  float x = r2 * glm::cos(glm::radians(alfa2)) * glm::sin(glm::radians(beta2));
	  float z = r2 * glm::sin(glm::radians(alfa2)) * glm::sin(glm::radians(beta2));
	  float y = r2 * glm::cos(glm::radians(beta2));
	  */

	  glm::vec4 q_vec = { 10.0f, 10.0f, 10.0f, 0.0f };
	  glm::quat q_quart = { 0.f, 10.0f, 10.0f, 10.0f };
	  glm::quat qy = glm::angleAxis(glm::radians(-alfa2), axis_y);
	  glm::quat qx = glm::angleAxis(glm::radians(beta2), axis_x);
	  glm::quat qtotal = qy * qx;
	  glm::quat q1 = qx * q_quart * glm::inverse(qx);
	  glm::quat q2 = qy * q1 * glm::inverse(qy);
	  glm::vec4 vf4 = glm::rotate(qtotal, q_vec);
	  glm::vec3 vf3 = { vf4.x, vf4.y, vf4.z };
	  glm::vec3 vf3_a = { q2.x, q2.y, q2.z };
	  std::cout << " vf3 =" << glm::to_string(vf3) << std::endl;
	  std::cout << " vf3_a =" << glm::to_string(vf3_a) << std::endl;

	  const glm::mat4 ChangingViewMatrix =
		  glm::lookAt(vf3_a, glm::vec3(0.0f, 0.0f, 0.0f),
			  glm::vec3(0.0f, 1.0f, 0.0f));

	  Camera2->setViewMatrix(ChangingViewMatrix * glm::scale(glm::vec3(1.0f * zoom2)));
	  if (projection_camera2)
		  Camera2->setProjectionMatrix(ProjectionMatrix2);
	  else
		  Camera2->setProjectionMatrix(ProjectionMatrix1);
  }

  draw_meshs();
}


void MyApp :: draw_meshs() {
	Shaders->bind();
	glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(ChangingModelMatrix));
	for (struct Mesh_obj* obj = Head; obj != nullptr; obj = obj->next_pointer) {
		obj->Mesh->update_position(glm::rotate(glm::radians(34.f), glm::vec3(0.f, 1.f, 0.f)));
		obj->Mesh->draw();
	}
	Shaders->unbind();
}

void MyApp::processKeyInput(GLFWwindow* win) {

	int state = glfwGetKey(win, GLFW_KEY_P);
	if (state == GLFW_PRESS && !OLD_P_CLICKED)
	{
		if (camera1_on)
			projection_camera1 = !projection_camera1;
		else
			projection_camera2 = !projection_camera2;
	}
	if (state == GLFW_PRESS)
		OLD_P_CLICKED = true;
	else
		OLD_P_CLICKED = false;

	int state_camera = glfwGetKey(win, GLFW_KEY_C);
	if (state_camera == GLFW_PRESS && !OLD_C_CLICKED)
	{
		camera1_on = !camera1_on;
		if (camera1_on) {
			//Camera2 = nullptr;
			Camera->Update(UBO_BP);
		}
		else {
			//Camera = nullptr;
			Camera2->Update(UBO_BP);
		}
	}
	if (state_camera == GLFW_PRESS)
		OLD_C_CLICKED = true;
	else
		OLD_C_CLICKED = false;
}

void MyApp::scrollCallback(GLFWwindow * window, double xoffset, double yoffset) {

	if (camera1_on)
		zoom += (float)yoffset * 0.1f;
	else
		zoom2 += (float)yoffset * 0.1f;
}

void MyApp::processMouseMovement(GLFWwindow * win) {

	glfwGetCursorPos(win, &xpos, &ypos);
	double scrollX = 0; double scrollY = 0;

	if (camera1_on) {

		int state = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT);

		if (state == GLFW_PRESS) {
			accelaration_x = 0;
			accelaration_y = 0;
			double diffx = xpos - old_xpos;
			double diffy = ypos - old_ypos;

			alfa += (float)diffx * 0.1;
			accelaration_x += (int)diffx;
			beta -= (float)diffy * 0.1;
			accelaration_y -= (int)diffy;
		}
		// So we dont overflow it
		if (alfa >= 360.0f || alfa <= -360.0f) {
			alfa = 0.0f;
		}
		if (beta >= 360.0f || beta <= -360.0f) {
			beta = 0.0f;
		}
		old_xpos = xpos;
		old_ypos = ypos;

		alfa += (float)accelaration_x / 10;
		beta += (float)accelaration_y / 10;

		if (accelaration_x > 0)
			accelaration_x -= 1;
		else if (accelaration_x < 0)
			accelaration_x += 1;

		if (accelaration_y > 0)
			accelaration_y -= 1;
		else if (accelaration_y < 0)
			accelaration_y += 1;
	}
	else {
		int state = glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT);

		if (state == GLFW_PRESS) {
			accelaration_x2 = 0;
			accelaration_y2 = 0;
			double diffx = xpos - old_xpos;
			double diffy = ypos - old_ypos;

			alfa2 += (float)diffx * 0.1;
			accelaration_x2 += (int)diffx;
			beta2 -= (float)diffy * 0.1;
			accelaration_y2 -= (int)diffy;
		}
		// So we dont overflow it
		if (alfa2 >= 360.0f || alfa2 <= -360.0f) {
			alfa2 = 0.0f;
		}
		if (beta2 >= 360.0f || beta2 <= -360.0f) {
			beta2 = 0.0f;
		}
		old_xpos = xpos;
		old_ypos = ypos;

		alfa2 += (float)accelaration_x2 / 10;
		beta2 += (float)accelaration_y2 / 10;

		if (accelaration_x2 > 0)
			accelaration_x2 -= 1;
		else if (accelaration_x2 < 0)
			accelaration_x2 += 1;

		if (accelaration_y2 > 0)
			accelaration_y2 -= 1;
		else if (accelaration_y2 < 0)
			accelaration_y2 += 1;
	}
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow *win) {

  createMeshes();
  createShaderPrograms(); // after mesh;
  createCamera();
  //SoundEngine->play2D("../assets/surrender.mp3", true);

}

void MyApp::windowSizeCallback(GLFWwindow *win, int winx, int winy) {
	if (winy == 0) {
		winy = 1;
	}

  glViewport(0, 0, winx, winy);
  // change projection matrices to maintain aspect ratio
  float ratio = (float)winx / (float)winy;

  updateMatrices(ratio);

}

void MyApp::displayCallback(GLFWwindow *win, double elapsed) { 
	processMouseMovement(win);
	processKeyInput(win);
	drawScene(); 
}

void MyApp::windowCloseCallback(GLFWwindow* win) {
	for (struct Mesh_obj* obj = Head; obj != nullptr; obj = obj->next_pointer) {
		obj->Mesh->destroyBufferObjects();
	}
}

/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char *argv[]) {
  mgl::Engine &engine = mgl::Engine::getInstance();
  engine.setApp(new MyApp());
  engine.setOpenGL(4, 6);
  engine.setWindow(1600, 1200, "OUI OUI OUI OUI OUI", 0, 1);
  engine.init();
  engine.run();
  exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
