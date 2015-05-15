#pragma comment (lib,"lib/freeglut.lib")
#pragma comment (lib,"lib/glew32.lib")
#pragma comment (lib,"lib/glew32s.lib")
#pragma comment(lib, "lib/irrKlang.lib")
#pragma comment(lib, "lib/DevIL.lib")
#pragma comment(lib, "lib/ILU.lib")
#pragma comment(lib, "lib/ILUT.lib")

#include "Header/GL/glew.h"
#include "GLSLProgram.h"
#include "object.h"
#include "menu.h"
#include "Camera.h"
#include "textureObject.h"
#include "Header/GL/freeglut.h"
#include "Header/glm/fwd.hpp"
#include "Header/glm/glm.hpp"
#include "Header/glm/mat4x4.hpp"
#include "Header/glm/matrix.hpp"
#include "Header/glm/gtc/type_ptr.hpp"
#include "Header/glm/gtc/quaternion.hpp"
#include "Header/glm/gtc/matrix_transform.hpp"
#include "Header/irrKlang/irrKlang.h"
#include "Header\IL\il.h"
#include <stdlib.h>
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <algorithm>

using namespace std;
using namespace irrklang;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void setOrthographicProjection();

void restorePerspectiveProjection();

void renderSpacedBitmapString(float x, float y, int spacing, void *font, char *string);

void Render_fps();

void display(void);

void init(void);

void setTime();

void gluperspective(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar);

void refresh();

void reshape(int iWidth, int iHeight);

void keyOperations(void);

void playMusic();

void keyPressed(unsigned char key, int x, int y);

void keyUp(unsigned char key, int x, int y);

void processNormalKeys(unsigned char key, int x, int y);

void switchOption();

void switchMusic();

void switchCar();

void switchLevel();

void processSpecialKeys(int key, int x, int y);

void mouseButton(int button, int state, int x, int y);

void cursorPos(int x, int y);

void passiveCursorPos(int x, int y);

void processMouseEntry(int state);

void renderBitmapString(float x, float y, float z, void *font, char *string);

void calculateMinMax(int i);

void loadFile(string route);

bool initialize();

void destroy();

void fullScreenMode();

void reinicializar();

void loadScores();

int main(int argc, char** argv);