
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

bool rotate_on;
GLuint vertex_array_object_id;
GLuint shader_program_id;

const char* vertex_shader_src =
    "#version 410\n"
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

const char* fragment_shader_src =
    "#version 410\n"
    // Use the same name as what was used in the vertex shader
    "in vec4 v_out_color;"
    "out vec4 frag_color;"
    "void main() {"
    // Hard coding the color
    // "  frag_color = vec4(0.5, 0.0, 0.5, 1.0);"
    "  frag_color = v_out_color;"
    "}";

#define msglError() _msglError(stderr, __FILE__, __LINE__)

bool _msglError(FILE* out, const char* filename, int line) {
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

void msglVersion() {
  fprintf(stderr, "Vendor: %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "Renderer: %s\n", glGetString(GL_RENDERER));
  fprintf(stderr, "OpenGL Version: %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "GLSL Version: %s\n",
          glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void Init() { rotate_on = false; }

static void msglfwErrorCallback(int error, const char* description) {
  fprintf(stderr, "GLFW Error: %s\n", description);
}

static void msglfwKeyCallback(GLFWwindow* window, int key, int scancode,
                              int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
      case GLFW_KEY_R:
        rotate_on = !rotate_on;
        break;
      default:
        fprintf(stderr, "Key pressed: %c:%d\n", key, key);
        break;
    }
  }
}

static void msglfwMouseButtonCallback(GLFWwindow* window, int button,
                                      int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    fprintf(stderr, "Right mouse button pressed.\n");
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    fprintf(stderr, "Left mouse button pressed.\n");
  }
}

void InitShader() {
  // The source code of the shader programs are defined as
  // const char* variables at the top of the file.

  GLint compiled_ok;

  // Create a vertex shader
  GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
  msglError();
  glShaderSource(vertex_shader_id, 1, &vertex_shader_src, NULL);
  glCompileShader(vertex_shader_id);
  glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &compiled_ok);
  msglError();
  if (!compiled_ok) {
    fprintf(stderr, "Compilation failed for shader \"%s\"\n",
            "built-in vertex shader");
    GLint info_log_length;
    glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    char* info_log = (char*)malloc(info_log_length);
    glGetShaderInfoLog(vertex_shader_id, info_log_length, NULL, info_log);
    fprintf(stderr, "Info Log:\n%s\n", info_log);
    free(info_log);
    assert(compiled_ok);
  }

  // Create a fragment shader
  GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
  msglError();
  glShaderSource(fragment_shader_id, 1, &fragment_shader_src, NULL);
  glCompileShader(fragment_shader_id);
  glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &compiled_ok);
  msglError();
  if (!compiled_ok) {
    fprintf(stderr, "Compilation failed for shader \"%s\"\n",
            "built-in vertex shader");
    GLint info_log_length;
    glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    char* info_log = (char*)malloc(info_log_length);
    glGetShaderInfoLog(fragment_shader_id, info_log_length, NULL, info_log);
    fprintf(stderr, "Info Log:\n%s\n", info_log);
    free(info_log);
    assert(compiled_ok);
  }

  GLint linked_ok;

  // Link it all together; shader_program_id is global
  shader_program_id = glCreateProgram();
  msglError();
  glAttachShader(shader_program_id, fragment_shader_id);
  glAttachShader(shader_program_id, vertex_shader_id);
  glLinkProgram(shader_program_id);
  glGetProgramiv(shader_program_id, GL_LINK_STATUS, &linked_ok);
  if (!linked_ok) {
    fprintf(stderr, "Linking failed.\n");
    GLint info_log_length;
    glGetProgramiv(shader_program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    char* info_log = (char*)malloc(info_log_length);
    glGetProgramInfoLog(shader_program_id, info_log_length, NULL, info_log);
    fprintf(stderr, "%s\n", info_log);
    free(info_log);
    assert(linked_ok);
  }

  msglError();
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

  msglError();

  GLuint vertex_buffer_object_id = 0;
  glGenBuffers(1, &vertex_buffer_object_id);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_id);
  // If using just floats
  // glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(vertex_t), points, GL_STATIC_DRAW);

  msglError();

  // vertex_array_object_id is global
  glGenVertexArrays(1, &vertex_array_object_id);
  assert(vertex_array_object_id >= 0);
  glBindVertexArray(vertex_array_object_id);
  // Vanilla
  // glEnableVertexAttribArray(0);
  // Fancy
  GLint location = glGetAttribLocation(shader_program_id, "in_vertex");
  msglError();
  glEnableVertexAttribArray(location);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_id);
  // If using just floats
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  // assert(shader_program_id >= 0);
  // printf("%u\n", location);
  // Vanilla
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
  //                       (GLvoid*)offsetof(vertex_t, coord));
  // Fancy
  glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
                        (GLvoid*)offsetof(vertex_t, coord));
  msglError();

  location = glGetAttribLocation(shader_program_id, "in_color");
  msglError();
  glEnableVertexAttribArray(location);
  msglError();
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_id);
  msglError();
  glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
                        (GLvoid*)offsetof(vertex_t, color));
  msglError();
  // Example of what happens when you ask for junk.
  // loc = glGetAttribLocation(shader_program_id, "junk");
  // printf("junk %d\n", location);
  // You could assert(location >= 0);
  // msglError();
}

void DrawTriangle() {
  msglError();
  glBindVertexArray(vertex_array_object_id);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  msglError();
}

int main(int argc, char const* argv[]) {
  GLFWwindow* window{nullptr};
  glfwSetErrorCallback(msglfwErrorCallback);
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

  glfwSetKeyCallback(window, msglfwKeyCallback);
  glfwSetMouseButtonCallback(window, msglfwMouseButtonCallback);

  Init();

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  InitShader();

  InitTriangle();

  GLuint u_model_view_matrix =
      glGetUniformLocation(shader_program_id, "model_view_matrix");
  assert(u_model_view_matrix >= 0);

  GLuint u_projection_matrix =
      glGetUniformLocation(shader_program_id, "projection_matrix");
  assert(u_projection_matrix >= 0);

  msglError();

  double previous_time{0.0};
  double z_rotate_angle{0.0};

  while (glfwWindowShouldClose(window) == 0) {
    float ratio{NAN};
    int width{0};
    int height{0};
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / static_cast<float>(height);

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection_matrix =
        glm::perspective<float>(90.0, ratio, 0.5, 5.0);

    glm::mat4 model_view_matrix;

    // glm::lookAt(eye_position, center, up)
    glm::mat4 look_at_matrix =
        glm::lookAt(glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 0.0, 0.0),
                    glm::vec3(0.0, 1.0, 0.0));

    double current_time = glfwGetTime();
    assert(current_time > 0.0);
    if (rotate_on) {
      // glfwGetTime() returns time in seconds
      double elapsed_time = current_time - previous_time;
      // rotate slowly around the z-axis once every 10 seconds
      // glm uses radians
      double previous_angle_degrees = z_rotate_angle * (180.0 / M_PI);
      double z_rotate_angle_degrees =
          previous_angle_degrees + (elapsed_time / 10.0) * 360.0;
      if (z_rotate_angle_degrees > 360.0) {
        z_rotate_angle_degrees = z_rotate_angle_degrees - 360.0;
      }
      z_rotate_angle = z_rotate_angle_degrees * (M_PI / 180.0);
    }
    // Rotated around glm::vec3(0.0f, 0.0f, -1.0f) by z_rotate_angle so
    // the triangle rotates counter clockwise from the camera's perspective.
    model_view_matrix = glm::rotate<float>(look_at_matrix, z_rotate_angle,
                                           glm::vec3(0.0f, 0.0f, -1.0f));

    previous_time = current_time;

    // Activate our shading program
    glUseProgram(shader_program_id);

    // Update any uniform variables
    glUniformMatrix4fv(u_model_view_matrix, 1, GL_FALSE,
                       glm::value_ptr(model_view_matrix));

    glUniformMatrix4fv(u_projection_matrix, 1, GL_FALSE,
                       glm::value_ptr(projection_matrix));

    DrawTriangle();
    msglError();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
