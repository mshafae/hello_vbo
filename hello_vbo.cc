
#include <GL/glew.h>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

typedef struct {
  glm::vec3 coord;
  glm::vec3 color;
} vertex_t;

const char* vertex_shader =
    "#version 400\n"
    "uniform mat4 model_view_matrix;"
    "uniform mat4 projection_matrix;"
    "in vec3 in_vertex;"
    "in vec3 in_color;"
    "out vec4 v_out_color;"
    "void main() {"
    "  gl_Position = projection_matrix * model_view_matrix * vec4(in_vertex, "
    "1.0);"
    // Hard coding the color, and passing it.
    // "  v_out_color = vec4(0.5, 0.0, 0.5, 1.0);"
    "  v_out_color = vec4(in_color, 1.0);"
    "}";

const char* fragment_shader =
    "#version 400\n"
    // Use the same name as what was used in the vertex shader
    "in vec4 v_out_color;"
    "out vec4 frag_color;"
    "void main() {"
    // Hard coding the color
    // "  frag_color = vec4(0.5, 0.0, 0.5, 1.0);"
    "  frag_color = v_out_color;"
    "}";

#define MSGLError() _MSGLError(stderr, __FILE__, __LINE__)

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

static void MouseButtonCallback(GLFWwindow* window, int button, int action,
                                int mods) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    fprintf(stderr, "Right mouse button pressed.\n");
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    fprintf(stderr, "Left mouse button pressed.\n");
  }
}

// static void MouseCursorPositionCallback(GLFWwindow* window, double xpos,
// double ypos) {
//   fprintf(stderr, "Mouse at (%f, %f)\n", xpos, ypos);
// }
//
// static void CursorEnteredCallback(GLFWwindow* window, int entered){
//   if (entered == GLFW_TRUE) {
//     fprintf(stderr, "Cursor entered the window.\n");
//   } else {
//     fprintf(stderr, "Cursor exited the window.\n");
//   }
// }

// void DrawTriangle() {
GLuint vao;
GLuint shader_program;

void InitShader() {
  // The source code of the shader programs are defined as
  // const char* variables at the top of the file.

  // Create a vertex shader
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  MSGLError();
  glShaderSource(vs, 1, &vertex_shader, NULL);
  glCompileShader(vs);
  MSGLError();

  // Create a fragment shader
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  MSGLError();
  glShaderSource(fs, 1, &fragment_shader, NULL);
  glCompileShader(fs);
  MSGLError();

  // Link it all together; shader_program is global
  shader_program = glCreateProgram();
  MSGLError();
  glAttachShader(shader_program, fs);
  glAttachShader(shader_program, vs);
  glLinkProgram(shader_program);
  MSGLError();
}

void InitTriangle() {
  // Just floats
  // float points[] = {
  //    0.0f,  0.5f,  0.0f,
  //    0.5f, -0.5f,  0.0f,
  //   -0.5f, -0.5f,  0.0f
  // };

  // Using a struct to interleave the data
  // First the vector then the color
  vertex_t points[] = {
      {glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
      {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
      {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
  };

  MSGLError();

  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // If using just floats
  // glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(vertex_t), points, GL_STATIC_DRAW);

  MSGLError();

  // vao is global
  glGenVertexArrays(1, &vao);
  assert(vao >= 0);
  glBindVertexArray(vao);
  // Vanilla
  // glEnableVertexAttribArray(0);
  // Fancy
  GLint loc = glGetAttribLocation(shader_program, "in_vertex");
  MSGLError();
  printf("in_vertex %d\n", loc);
  glEnableVertexAttribArray(loc);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // If using just floats
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  // assert(shader_program >= 0);
  // printf("%u\n", loc);
  // Vanilla
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
  //                       (GLvoid*)offsetof(vertex_t, coord));
  // Fancy
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
                        (GLvoid*)offsetof(vertex_t, coord));
  MSGLError();

  loc = glGetAttribLocation(shader_program, "in_color");
  MSGLError();
  printf("in_color %d\n", loc);
  glEnableVertexAttribArray(loc);
  MSGLError();
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  MSGLError();
  glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
                        (GLvoid*)offsetof(vertex_t, color));
  MSGLError();
  // loc = glGetAttribLocation(shader_program, "junk");
  // printf("junk %d\n", loc);
  // You could assert(loc >= 0);
  // MSGLError();
}

void DrawTriangle() {
  MSGLError();
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  MSGLError();
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

int main(int argc, char const* argv[]) {
  GLFWwindow* window{nullptr};
  glfwSetErrorCallback(ErrorCallback);
  if (glfwInit() == 0) {
    return 1;
  }

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  // If you want an OpenGL 3.0+ context
  // see https://www.glfw.org/faq.html#41---how-do-i-create-an-opengl-30-context
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
  // glfwSetCursorPosCallback(window, MouseCursorPositionCallback);
  // glfwSetCursorEnterCallback(window, CursorEnteredCallback);

  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  MSGLError();

  InitShader();

  MSGLVersion();

  InitTriangle();

  MSGLError();

  GLuint u_model_view_matrix =
      glGetUniformLocation(shader_program, "model_view_matrix");
  assert(u_model_view_matrix >= 0);

  MSGLError();

  GLuint u_projection_matrix =
      glGetUniformLocation(shader_program, "projection_matrix");
  assert(u_projection_matrix >= 0);

  MSGLError();

  while (glfwWindowShouldClose(window) == 0) {
    float ratio{NAN};
    int width{0};
    int height{0};
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / static_cast<float>(height);

    MSGLError();

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 projection_matrix =
        glm::perspective<float>(90.0, ratio, 0.5, 5.0);
    // lookAt(eye_position, center, up)
    // glm::mat4 look_at_matrix =
    //     glm::lookAt(glm::vec3(0.0, 0.0, -2.5), glm::vec3(0.0, 0.0, 0.0),
    //                 glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 look_at_matrix =
        glm::lookAt(glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, 0.0),
                    glm::vec3(0.0, 1.0, 0.0));
    // glm::mat4 z_rotate = glm::rotate<float>(look_at_matrix, (glfwGetTime() * 50.0F), glm::vec3(0.0f, 0.0f, 1.0f));
    // glm::mat4 y_rotate = glm::rotate<float>(z_rotate, (glfwGetTime() * 30.0F), glm::vec3(0.0f, 1.0f, 0.0f));
    // glm::mat4 x_rotate = glm::rotate<float>(y_rotate, (glfwGetTime() * 15.0F), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 model_view_matrix = look_at_matrix;
    // glm::mat4 model_view_matrix = x_rotate;

    MSGLError();

    // Activate our shading program
    glUseProgram(shader_program);

    MSGLError();

    // Update any uniform variables
    glUniformMatrix4fv(u_model_view_matrix, 1, GL_FALSE,
                       glm::value_ptr(model_view_matrix));

    MSGLError();

    glUniformMatrix4fv(u_projection_matrix, 1, GL_FALSE,
                       glm::value_ptr(projection_matrix));

    MSGLError();

    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    // glFrustum(-ratio, ratio, -ratio, ratio, 1.0F, 40.0F);
    // MSGLError();
    //
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
    // MSGLError();
    // gluLookAt(0.0F, 0.0F, -2.5F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F);
    // glRotatef(static_cast<float>(glfwGetTime()) * 50.0F, 0.0F, 0.0F, 1.0F);
    // glRotatef(static_cast<float>(glfwGetTime()) * 30.0F, 0.0F, 1.0F, 0.0F);
    // glRotatef(static_cast<float>(glfwGetTime()) * 15.0F, 1.0F, 0.0F, 0.0F);
    // MSGLError();

    // Draw!
    // DrawTetrahedron();
    DrawTriangle();
    MSGLError();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
