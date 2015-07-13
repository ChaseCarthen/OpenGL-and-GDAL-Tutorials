
#include <GLUT/glut.h>

GLint faces[6][4] = {
    {0, 1, 3, 2},
    {4, 5, 7, 6},  // Back face and front
    {0, 4, 6, 2},
    {1, 3, 7, 5},  // Left face and right face
    {0, 1, 5, 4},
    {2, 3, 7, 6},  // Bottom face and top face
};
GLfloat n[6][3] = {  // Normals
    {0, 0, -1},
    {0, 0, 1},  // Back and front
    {1, 0, 0},
    {-1, 0, 0},  // Left and right
    {0, 1, 0},
    {0, -1, 0}};  // Bottom and top
GLfloat v[8][3];

void drawBox(void) {
  int i;
  for (i = 0; i < 6; i++) {
    glBegin(GL_QUADS);
    glNormal3fv(n[i]);
    glVertex3fv(v[faces[i][0]]);
    glVertex3fv(v[faces[i][1]]);
    glVertex3fv(v[faces[i][2]]);
    glVertex3fv(v[faces[i][3]]);
    glEnd();
  }
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawBox();
  glutSwapBuffers();
}

void init(void) {
  for (int i = 0; i < 8; i++) {
    // Set the x, y and z coordinates
    v[i][0] = (i & 1) * 2 - 1;
    v[i][1] = (i >> 1 & 1) * 2 - 1;
    v[i][2] = (i >> 2 & 1) * 2 - 1;
  }

  GLfloat light_diffuse[] = {1.0, 1.0, 1.0};
  GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  gluPerspective(40.0, 1.0, 1.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 5.0,  // eye is at (0,0,5)
            0.0, 0.0, 0.0,  // center is at (0,0,0)
            0.0, 1.0, 0.);  // up is in positive Y direction

  glTranslatef(0.0, 0.0, -1.0);
  glRotatef(-20, 1.0, 0.0, .0);
  glRotatef(20, 0.0, 1.0, .0);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Cube");
  glutReshapeWindow(200, 200);
  glutDisplayFunc(display);
  init();
  glCreateProgram();
  glutMainLoop();
  return 0;
}
