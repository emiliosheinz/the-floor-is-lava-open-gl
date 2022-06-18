

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <string.h>

using namespace std;

void walk(string direction)
{
  if (direction == "up")
  {
    cout << "Walking up" << endl;
  }
  else if (direction == "down")
  {
    cout << "Walking down" << endl;
  }
  else if (direction == "left")
  {
    cout << "Walking left" << endl;
  }
  else if (direction == "right")
  {
    cout << "Walking right" << endl;
  }
}

void onKeyPress(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
  {
    walk("right");
  }
  else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
  {
    walk("left");
  }
  else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
  {
    walk("up");
  }
  else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
  {
    walk("down");
  }
}

void mapWalingEvents()
{
  glfwSetKeyCallback(g_window, onKeyPress);
}