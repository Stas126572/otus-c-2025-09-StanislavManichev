#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <stdio.h>


int main(int argc, char* argv[]) {
  if (!glfwInit())
    return -1;


  GLFWwindow *window =
      glfwCreateWindow(640, 480, "OpenGL Rotate Cube", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);


  while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glRotatef(1.0f, 1.0f, 0.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUAD_STRIP);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.5f, -0.4f, 0.5f);
    glVertex3f(0.5f, 0.6f, 0.5f);
    glVertex3f(-0.5f, -0.4f, 0.5f);
    glVertex3f(-0.5f, 0.6f, 0.5f);

    glVertex3f(-0.5f, -0.4f, 0.5f);
    glVertex3f(-0.5f, 0.6f, 0.5f);
    glVertex3f(-0.5f, -0.4f, -0.5f);
    glVertex3f(-0.5f, 0.6f, -0.5f);

    glVertex3f(-0.5f, 0.6f, 0.5f);
    glVertex3f(-0.5f, 0.6f, -0.5f);
    glVertex3f(0.5f, 0.6f, 0.5f);
    glVertex3f(0.5f, 0.6f, -0.5f);

    glVertex3f(-0.5f, -0.4f, -0.5f);
    glVertex3f(-0.5f, 0.6f, -0.5f);
    glVertex3f(0.5f, -0.4f, -0.5f);
    glVertex3f(0.5f, 0.6f, -0.5f);

        glVertex3f(-0.5f, -0.4f, 0.5f);
    glVertex3f(-0.5f, -0.4f, -0.5f);
    glVertex3f(0.5f, -0.4f, 0.5f);
    glVertex3f(0.5f, -0.4f, -0.5f);

    glVertex3f(0.5f, -0.4f, -0.5f);
    glVertex3f(0.5f, 0.6f, -0.5f);
    glVertex3f(0.5f, -0.4f, 0.5f);
    glVertex3f(0.5f, 0.6f, 0.5f);

  

    glEnd();

    glBegin(GL_LINE_LOOP);
        glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f,  0.6f,  0.5f);
    glVertex3f(-0.5f,  0.6f, -0.5f);
    glVertex3f(-0.5f, -0.4f, -0.5f);
    glVertex3f(-0.5f, -0.4f,  0.5f); 
glEnd();

    glBegin(GL_LINE_LOOP);
        glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.5f,  0.6f,  0.5f); 
    glVertex3f(0.5f,  0.6f, -0.5f);
    glVertex3f(0.5f, -0.4f, -0.5f); 
    glVertex3f(0.5f, -0.4f,  0.5f); 

    glEnd();


    
    glBegin(GL_LINE_LOOP);

    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.4f, 0.5f);
    glVertex3f(0.5f, -0.4f, 0.5f);
    glVertex3f(0.5f, 0.6f, 0.5f); 
    glVertex3f(-0.5f, 0.6f, 0.5f);

    glEnd();

    glBegin(GL_LINE_LOOP);

    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.4f, -0.5f);
    glVertex3f(0.5f, -0.4f, -0.5f);
    glVertex3f(0.5f, 0.6f, -0.5f);
    glVertex3f(-0.5f, 0.6f, -0.5f);

    glEnd();
glPopMatrix();


    glEnd();


    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

