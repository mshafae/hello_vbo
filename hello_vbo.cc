//
// Michael Shafae
// mshafae at fullerton.edu
//
// A really simple GLFW3 demo that renders an animated and
// colored tetrahedron which the edges outlined in black.
//

#include <iostream>

#include <GL/glew.h>

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#define MSGLError() _msglError(stderr, __FILE__, __LINE__)

bool _MSGLError(FILE* out, const char* filename, int line) {
  bool ret = false;
  GLenum err = glGetError();
  while (err != GL_NO_ERROR) {
    ret = true;
    fprintf(out, "GL ERROR:%s:%d: %s\n", filename, line,
            static_cast<const unsigned char*>(gluErrorString(err)));
    err = glGetError();
  }
  return (ret);
}

void MSGLVersion() {
  fprintf(stderr, "Vendor: %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "Renderer: %s\n", glGetString(GL_RENDERER));
  fprintf(stderr, "OpenGL Version: %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "GLSL Version: %s\n",
          glGetString(GL_SHADING_LANGUAGE_VERSION));
}

static void ErrorCallback(int error, const char* description) {
  fprintf(stderr, "GLFW Error: %s\n", description);
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                        int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
      default:
        fprintf(stderr, "Key pressed: %c:%d\n", key, key);
        break;
    }
  }
}

static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    fprintf(stderr, "Right mouse button pressed.\n");
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    fprintf(stderr, "Left mouse button pressed.\n");
  }
}

static void MouseCursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
  fprintf(stderr, "Mouse at (%f, %f)\n", xpos, ypos);
}

static void CursorEnteredCallback(GLFWwindow* window, int entered){
  if (entered == GLFW_TRUE) {
    fprintf(stderr, "Cursor entered the window.\n");
  } else {
    fprintf(stderr, "Cursor exited the window.\n");
  }
}

void DrawTriangle() {
  glBegin(GL_TRIANGLES);
  glColor3f(1.0F, 0.0F, 0.0F);
  glVertex3f(-0.6F, -0.4F, 0.0F);
  glColor3f(0.0F, 1.0F, 0.0F);
  glVertex3f(0.6F, -0.4F, 0.0F);
  glColor3f(0.0F, 0.0F, 1.0F);
  glVertex3f(0.0F, 0.6F, 0.0F);
  glEnd();
}

void DrawTetrahedron() {
  glBegin(GL_TRIANGLE_STRIP);
  glColor3f(1.0F, 1.0F, 1.0F);
  glVertex3f(0.0F, 1.0F, 0.0F);  // a
  glColor3f(1.0F, 0.0F, 0.0F);
  glVertex3f(-1.0F, 0.0F, 1.0F);  // b
  glColor3f(0.0F, 1.0F, 0.0F);
  glVertex3f(1.0F, 0.0F, 1.0F);  // c
  glColor3f(0.0F, 0.0F, 1.0F);
  glVertex3f(0.0F, 0.0F, -1.0F);  // d
  glColor3f(1.0F, 1.0F, 1.0F);
  glVertex3f(0.0F, 1.0F, 0.0F);  // a
  glColor3f(1.0F, 0.0F, 0.0F);
  glVertex3f(-1.0F, 0.0F, 1.0F);  // b
  glEnd();

  glBegin(GL_LINES);
  glColor3f(0.0F, 0.0F, 0.0F);

  // a b
  glVertex3f(0.0F, 1.0F, 0.0F);
  glVertex3f(-1.0F, 0.0F, 1.0F);
  // a c
  glVertex3f(0.0F, 1.0F, 0.0F);
  glVertex3f(1.0F, 0.0F, 1.0F);

  // b c
  glVertex3f(-1.0F, 0.0F, 1.0F);
  glVertex3f(1.0F, 0.0F, 1.0F);

  // a d
  glVertex3f(0.0F, 1.0F, 0.0F);
  glVertex3f(0.0F, 0.0F, -1.0F);
  // b d
  glVertex3f(-1.0F, 0.0F, 1.0F);
  glVertex3f(0.0F, 0.0F, -1.0F);
  // c d
  glVertex3f(1.0F, 0.0F, 1.0F);
  glVertex3f(0.0F, 0.0F, -1.0F);

  glEnd();
}

int main(int argc, char const *argv[]) {
  GLFWwindow* window{nullptr};
  glfwSetErrorCallback(ErrorCallback);
  if (glfwInit() == 0) {
    return 1;
  }

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  // If you want an OpenGL 3.0+ context
  // see https://www.glfw.org/faq.html#41---how-do-i-create-an-opengl-30-context
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  window = glfwCreateWindow(500, 500, "Hello OpenGL and GLFW", 0, nullptr);
  if (window == nullptr) {
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = true;
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "GLEW: init failed: %s\n", glewGetErrorString(err));
    return 1;
  }

  glfwSetKeyCallback(window, KeyCallback);
  glfwSetMouseButtonCallback(window, MouseButtonCallback);
  glfwSetCursorPosCallback(window, MouseCursorPositionCallback);
  glfwSetCursorEnterCallback(window, CursorEnteredCallback);
  
  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  MSGLVersion();

  while (glfwWindowShouldClose(window) == 0) {
    float ratio{NAN};
    int width{0};
    int height{0};
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / static_cast<float>(height);
    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ratio, ratio, -ratio, ratio, 1.0F, 40.0F);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0F, 0.0F, -2.5F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F);
    glRotatef(static_cast<float>(glfwGetTime()) * 50.0F, 0.0F, 0.0F, 1.0F);
    glRotatef(static_cast<float>(glfwGetTime()) * 30.0F, 0.0F, 1.0F, 0.0F);
    glRotatef(static_cast<float>(glfwGetTime()) * 15.0F, 1.0F, 0.0F, 0.0F);

    DrawTetrahedron();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
