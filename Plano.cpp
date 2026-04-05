#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <array>
#include <vector>

// funciones

std::vector<std::array<float, 3>> puntos;

float zoom = -3.0f;

bool clickmause = false;

float angleX = 45.0f;
float angleY = 45.0f;
double lastX, lastY;

// voids

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void charCallback(GLFWwindow *window, unsigned int c) {
  ImGui_ImplGlfw_CharCallback(window, c);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  zoom += yoffset * 0.5f;
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
  if (ImGui::GetIO().WantCaptureMouse)
    return;
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      clickmause = true;
      glfwGetCursorPos(window, &lastX, &lastY);
    } else {
      clickmause = false;
    }
  }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
  if (ImGui::GetIO().WantCaptureMouse)
    return;
  if (!clickmause)
    return;

  float dx = xpos - lastX;
  float dy = ypos - lastY;
  lastX = xpos;
  lastY = ypos;

  angleX += dx * 0.3f;
  angleY += dy * 0.3f;
}

void framebufferCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

// MAIN

int main() {
  glfwInit();

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "Plano cartesiano", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glewInit();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, false);
  ImGui_ImplOpenGL2_Init();

  // registros de voids
  glfwSetKeyCallback(window, keyCallback);
  glfwSetCharCallback(window, charCallback);
  glfwSetFramebufferSizeCallback(window, framebufferCallback);
  glfwSetScrollCallback(window, scrollCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetCursorPosCallback(window, mouseCallback);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // camara

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, zoom);
    glRotatef(angleY, 1.0f, 0.0f, 0.0f);
    glRotatef(angleX, 0.0f, 1.0f, 0.0f);
    // lines

    glBegin(GL_LINES);

    glColor3f(0.2f, 0.2f, 0.2f);

    for (int i = -100; i <= 100; i++) {
      glVertex3f(100.0f, 0.0f, (float)i);
      glVertex3f(-100.0f, 0.0f, (float)i);

      glVertex3f((float)i, 0.0f, 100.0f);
      glVertex3f((float)i, 0.0f, -100.0f);
    }

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);

    glEnd();

    glPointSize(8.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 0.0f); // amarillo
    for (auto &p : puntos) {
      glVertex3f(p[0], p[1], p[2]);
    }

    glEnd();

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_Once);
    ImGui::Begin("Plano Cartesiano");
    ImGui::Text("Agrega elementos:");
    static float px = 0.0f, py = 0.0f, pz = 0.0f;
    ImGui::InputFloat("X", &px);
    ImGui::InputFloat("Y", &py);
    ImGui::InputFloat("Z", &pz);
    if (ImGui::Button("Agregar Punto")) {
      puntos.push_back({px, py, pz});
    }
    if (ImGui::Button("Deshacer")) {
      if (!puntos.empty()) {
        puntos.pop_back();
      }
    }
    if (ImGui::Button("Borrar puntos")) {
      puntos.clear();
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  return 0;
}