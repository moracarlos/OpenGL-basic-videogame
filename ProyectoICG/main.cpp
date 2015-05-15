#include "main.h"
#define PI 3.14159265359

int gWidth, gHeight;
const float NCP = 0.01f, FCP = 2000.0f, fAngle = 45.f; //FCP = 20.0f
CGLSLProgram m_program;
CGLSLProgram m_texture_program;
glm::mat4x4 mProjMatrix, mModelViewMatrix;
//Light variables
float lightPos[3];
float eyePos[3];
float ld[3];//{1,0.8,0};
float ls[3];//{1,0.8,0};
float ambient[3];//{1,0.2,1};

//Camera variables
bool* keyStates = new bool[256]; // Create an array of boolean values of length 256 (0-255)

CObject* object; //para cargar el carro
vector<CObject*> carros;
vector<CObject*> coins;
vector<CObject*> hourglass;
vector <CTexturedObject*> texturedObjects;

CCamera* myCamera;
double rotation = 0;

//Variables to compute frames per second
int frame;
long t, timebase;
char s[50], pts[50], timer[50];
int points = 0, minutes = 0, seg = 0;
time_t currentTime;
struct tm *localT = localtime(&currentTime);
int initSec = localT->tm_sec;

//Menu variables
CMenu* menu = new CMenu();
options option = NOTHING;
menus currentMenu = PRINCIPAL;
music song = ANTI;
int optionCount = 1, optionLimit = 4;
int musicCount = 1, musicLimit = 6;
int carCount = 1, carLimit = 3;
int levelCount = 1, levelLimit = 3;
CTexturedObject* menuTexture;
float rot = 0.017;

///Music variables
ISoundEngine* principal = createIrrKlangDevice();
ISoundEngine* click = createIrrKlangDevice();

bool collision = false;
vector <bool> picked;

// = {false, false, false, false, false};
vector <GLfloat> trasX, trasXClock;
vector <GLfloat> trasZ, trasZClock;
int clockPos = 0, clockTime = 50;
vector<int> tiempos;
bool displayClock = true;

//Variables para los scores
vector<int>puntuaciones;
char punt[7];

int winningPoints;

bool playMusicGameOver = true; //Plays music only one time in display function

void setOrthographicProjection() {
	// switch to projection mode
	glMatrixMode(GL_PROJECTION);

	// save previous matrix which contains the
	//settings for the perspective projection
	glPushMatrix();

	// reset matrix
	glLoadIdentity();

	// set a 2D orthographic projection
	gluOrtho2D(0, gWidth, gHeight, 0);

	// switch back to modelview mode
	glMatrixMode(GL_MODELVIEW);
}

void restorePerspectiveProjection() {
	glMatrixMode(GL_PROJECTION);
	// restore previous projection matrix
	glPopMatrix();

	// get back to modelview mode
	glMatrixMode(GL_MODELVIEW);
}

//Writes on Screen
void renderSpacedBitmapString(float x, float y, int spacing, void *font, char *string){
	char *c;
	int x1 = x;

	for (c = string; *c != '\0'; c++) {
		glRasterPos2f(x1, y);
		glutBitmapCharacter(font, *c);
		x1 = x1 + glutBitmapWidth(font, *c) + spacing;
	}
}

void Render_fps(){
	/*-----------------------------------------------FRAMES POR SEGUNDO ----------------------------------*/
	frame++;

	t = glutGet(GLUT_ELAPSED_TIME);
	if (t - timebase > 1000) {
		sprintf(s, "Proyecto - FPS:%4.2f", frame*1000.0 / (t - timebase));
		timebase = t;
		frame = 0;
	}
	/*----------------------------------------------------------------------------------------------------*/

	/*----------------------------------------TIME--------------------------------------------------------*/
		currentTime = time(0);
		localT = localtime(&currentTime);
		//initSec = localT->tm_sec;

		sprintf(timer, "Tiempo restante: %dmin %dseg", minutes, seg);
	/*----------------------------------------------------------------------------------------------------*/

	/*--------------------------------------------------POINTS--------------------------------------------*/
		sprintf(pts, "Puntaje: %dptos", points);
	/*----------------------------------------------------------------------------------------------------*/

	/*----------------------------------WRITE ON SCREEN---------------------------------------------------*/
	void *font = GLUT_BITMAP_9_BY_15;

	setOrthographicProjection();

	glPushMatrix();
	glLoadIdentity();
	glColor3f(0.0f, 1.0f, 1.0f);
	renderSpacedBitmapString(5, 30, 5, font, s);
	if(minutes==0 && seg<=30)	glColor3f(1.0f, 0.0f, 0.0f);
	renderSpacedBitmapString(5, 60, 5, font, timer);
	glColor3f(0.0f, 1.0f, 1.0f);
	renderSpacedBitmapString(5, 90, 5, font, pts);
	glPopMatrix();

	if (initSec != localT->tm_sec){
		initSec = localT->tm_sec;
		if (minutes == 0 && seg - 1 == 0){
			//
			if(points>puntuaciones[0]){
				puntuaciones[0] = points;
	//			sort(puntuaciones.begin(),puntuaciones.end());
				cout << "Estoy imprimiendo en el archivo" << endl;
				std::fstream file("./scores.txt", std::fstream::out | std::ofstream::trunc);

				for(int i=0;i<puntuaciones.size();++i){
					if (i != puntuaciones.size() - 1){
						file << puntuaciones[i] << endl;
					}
					else{
						file << puntuaciones[i];
					}
				}

				for (int i = 0; i < puntuaciones.size();i++)
					cout << puntuaciones[i] << " puntuaciones" << endl;

				file.close();
				loadScores();
			}
			option=OVER;
			//
		}
		else{
			seg--;
			for(int i=0;i<coins.size();++i)
				tiempos[i]++;
			
			clockTime--;
			if (clockTime <= 0){
				clockTime = 100;
				clockPos++;
				displayClock = true;
				if (clockPos >= 15){
					clockPos = 0;
				}
			}

			if (seg <= 0){
				minutes--; seg = 60;
				if (minutes <= 0) minutes = 0;
			}
		}
	}

	restorePerspectiveProjection();
	/*---------------------------------------------------------------------------------------------------------*/
}

void display(void){	
	keyOperations();

	glFrontFace(GL_CCW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //GL_LINE GL_FILL GL_POINT

	mModelViewMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, -0.f)); //Antes tenia -10
	double angle = carros[carCount-1]->getAlpha();
		
	if (option == NOTHING){
		playMusicGameOver = true;
		float ratio = gWidth / float(gHeight);
		if (gHeight == 0) gHeight = 1;
		glViewport(0, 0, gWidth, gHeight);
		gluperspective(fAngle, ratio, NCP, FCP);
		glm::vec3 center(menuTexture->getXMid(), menuTexture->getYMid(), menuTexture->getZMid());
		glm::vec3 eye(menuTexture->getXMid(), menuTexture->getYMid(), menuTexture->getZMid() - 10);
		glm::mat4 LA = glm::lookAt(eye, center, glm::vec3(0, 1, 0));
		mProjMatrix = mProjMatrix * LA;
		menuTexture->display();
		menu->setCurrentMenu(PRINCIPAL);
		menu->displayMenu();

	}else if (option == PLAY){
		//menu->game->stopAllSounds();
		//playMusic();
		menuTexture->display();
		menu->setCurrentMenu(CARS);
		menu->displayCarsMenu();		
		carros[carCount - 1]->setScaleX(0.5);
		carros[carCount - 1]->setScaleY(0.5);
		carros[carCount - 1]->setScaleZ(0.5);

		carros[carCount - 1]->setTraslateY(0.1);
		carros[carCount - 1]->setTraslateZ(-1.5);

		glm::quat q = glm::quat(0.1, glm::vec3(0,1,0));
		carros[carCount - 1]->setQuat(q.x, q.y, q.z, q.w);

		//rot = rot + 0.0001;

		carros[carCount - 1]->display();

	}else if (option == MUSIC){
		menuTexture->display();
		menu->setCurrentMenu(SOUNDTRACK);
		menu->displayMusicMenu();

	}else if (option == LEVELS){
		menuTexture->display();
		menu->setCurrentMenu(LEVEL); //Init car
		menu->displayLevelsMenu();

	}else if (option == START){
		glPushMatrix();

		/*-----------------------------------------------------------DISPLAY CAR---------------------------------------*/
			carros[carCount-1]->calculatePhysics();
			refresh();
			carros[carCount - 1]->display();
			for (int i = 0; i < texturedObjects.size(); i++)
				texturedObjects[i]->display();
			
		/*-----------------------------------------------------------DISPLAY COINS--------------------------------------*/
			for (int i = 0; i < coins.size(); i++){
				if (/*!picked[i] &&*/ tiempos[i]>10){
					coins[i]->setTraslateX(trasX[i]);
					coins[i]->setTraslateZ(trasZ[i]);
					coins[i]->setTraslateY(2);
					coins[i]->setScaleX(0.5);
					coins[i]->setScaleY(0.5);
					coins[i]->setScaleZ(0.5);
					coins[i]->setQuat(1.f, 0, 0, 1);
					coins[i]->display();
					if (carros[carCount - 1]->getZMin() + carros[carCount - 1]->getTraslateZ() <= coins[i]->getZMin() + coins[i]->getTraslateZ() &&
						carros[carCount - 1]->getZMax() + carros[carCount - 1]->getTraslateZ() >= coins[i]->getZMax() + coins[i]->getTraslateZ() &&
						carros[carCount - 1]->getXMin() + carros[carCount - 1]->getTraslateX() <= coins[i]->getXMin() + coins[i]->getTraslateX() &&
						carros[carCount - 1]->getXMax() + carros[carCount - 1]->getTraslateX() >= coins[i]->getXMax() + coins[i]->getTraslateX()){
						picked[i] = true;
						click->play2D("Music/Mario Coin.WAV",false);
						points += 10;
						tiempos[i]=0;
					}
				}
			}
			collision = false;

		/*-----------------------------------------------------------DISPLAY CLOCK --------------------------------------*/
			if (displayClock){
				hourglass[0]->setScaleX(0.05);
				hourglass[0]->setScaleY(0.05);
				hourglass[0]->setScaleZ(0.05);
				hourglass[0]->setTraslateX(trasXClock[clockPos]);
				hourglass[0]->setTraslateY(5);
				hourglass[0]->setTraslateZ(trasZClock[clockPos]);
				hourglass[0]->display();
				if (carros[carCount - 1]->getZMin() + carros[carCount - 1]->getTraslateZ() >= hourglass[0]->getZMin() + hourglass[0]->getTraslateZ() &&
					carros[carCount - 1]->getZMax() + carros[carCount - 1]->getTraslateZ() <= hourglass[0]->getZMax() + hourglass[0]->getTraslateZ() &&
					carros[carCount - 1]->getXMin() + carros[carCount - 1]->getTraslateX() >= hourglass[0]->getXMin() + hourglass[0]->getTraslateX() &&
					carros[carCount - 1]->getXMax() + carros[carCount - 1]->getTraslateX() <= hourglass[0]->getXMax() + hourglass[0]->getTraslateX()){
					displayClock = false;
					click->play2D("Music/clock.wav", false); //CAMBIAR ESTO POR SONIDO DE TOMAR RELOJ!!!!!!
					minutes = minutes + 1;
				}
			}
		glPopMatrix();
		Render_fps();

	}else if(option == OVER){
		//principal->stopAllSounds();
		float ratio = gWidth / float(gHeight);
		if (gHeight == 0) gHeight = 1;
		glViewport(0, 0, gWidth, gHeight);
		gluperspective(fAngle, ratio, NCP, FCP);
		glm::vec3 center(menuTexture->getXMid(), menuTexture->getYMid(), menuTexture->getZMid());
		glm::vec3 eye(menuTexture->getXMid(), menuTexture->getYMid(), menuTexture->getZMid() - 10);
		glm::mat4 LA = glm::lookAt(eye, center, glm::vec3(0, 1, 0));
		mProjMatrix = mProjMatrix * LA;
		menuTexture->display();
		menu->displayGameOver();
		playMusicGameOver = false;

	}else if(option == SCORES){
		menu->setCurrentMenu(HIGHSCORES);
		menuTexture->display();	
		setOrthographicProjection();
		glPushMatrix();
		glLoadIdentity();
		glColor3f(0.0f, 1.0f, 1.0f);

		int sizePts = puntuaciones.size() - 1, posY = 150;

		for (int i = 0; i < 5; i++){
			if (sizePts - i >= 0){
				sprintf(punt, "%d", puntuaciones[sizePts - i]);
				renderSpacedBitmapString((gWidth - 50) / 2, (gHeight - posY) / 2, 5, GLUT_BITMAP_9_BY_15, punt);
				posY = posY - 50;
			}
			else{
				break;
			}
		}
		renderSpacedBitmapString((gWidth - 300) / 2, (gHeight + 150) / 2, 5, GLUT_BITMAP_9_BY_15, "PRESIONE ESC PARA IR ATRAS");
		glPopMatrix();
		restorePerspectiveProjection();
	}

	glutSwapBuffers();
}

//init Shaders
bool initialize(){
	//Glew INIT
	//glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "- glew Init failed :(" << std::endl;
		return false;
	}

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	//Load the shaders
	m_program.loadShader("shaders/basic.vert", CGLSLProgram::VERTEX);
	m_program.loadShader("shaders/basic.frag", CGLSLProgram::FRAGMENT);
	//Link the shaders in a program
	m_program.create_link();
	//Enable the program
	m_program.enable();
	//Link the attributes and the uniforms
	m_program.addAttribute("vVertex");
	m_program.addAttribute("vColor");
	m_program.addAttribute("vNormal");
	m_program.addUniform("mProjection");
	m_program.addUniform("mModelView");

	m_program.addUniform("centerPosition");
	m_program.addUniform("translateFactor");
	m_program.addUniform("scaleFactor");
	m_program.addUniform("quat");

	m_program.addUniform("lightPos");
	m_program.addUniform("eyePos");
	m_program.addUniform("ld");
	m_program.addUniform("ls");
	m_program.addUniform("ambient");

	m_program.addUniform("diffuseObject");
	m_program.addUniform("specularObject");
	m_program.addUniform("ambientObject");

	//Disable the program
	m_program.disable();

	//Function to initiate a triangle
	//	initiateTriangle();
	
	
	//Load texture shader
	m_texture_program.loadShader("shaders/texture.vert", CGLSLProgram::VERTEX);
	m_texture_program.loadShader("shaders/texture.frag", CGLSLProgram::FRAGMENT);
	//Link the shaders in a program
	m_texture_program.create_link();
	//Enable the program
	m_texture_program.enable();
	//Link the attributes and the uniforms
	m_texture_program.addAttribute("vVertex");
	m_texture_program.addAttribute("vColor");
	m_texture_program.addAttribute("vNormal");
	m_texture_program.addAttribute("vTexture");
	m_texture_program.addUniform("mProjection");
	m_texture_program.addUniform("mModelView");
	//Disable the program
	m_texture_program.disable();

	return true;
}

//initGLUT and Camera
void init(void){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);
//	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(mouseButton);
	glutMotionFunc(cursorPos);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyUp);

	//Init Camera
	glm::vec3 eye(carros[carCount-1]->getXMid(), carros[carCount-1]->getYMid(), carros[carCount-1]->getZMid());
	glm::vec3 center(carros[carCount-1]->getXMid(), carros[carCount-1]->getYMid(), carros[carCount-1]->getZMid());
	//cout << "Eye: " << eye.x << " " << eye.y << " " << eye.z << endl;
	//cout << "Center: " << center.x << " " << center.y << " " << center.z << endl;
	myCamera = new CCamera(eye, center);
}

void gluperspective(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar){
	const GLdouble pi = 3.1415926535897932384626433832795;
	GLdouble fW, fH;
	fH = tan(fovY / 360 * pi) * zNear;	//fH = tan( (fovY / 2) / 180 * pi ) * zNear;
	fW = fH * aspect;
	glFrustum(-fW, fW, -fH, fH, zNear, zFar);
	mProjMatrix = glm::frustum(-fW, fW, -fH, fH, zNear, zFar);
	//mProjMatrix = glm::perspective(fovY, aspect, zNear, zFar); //Aqui va con glm::frustum
}

void refresh(){
	eyePos[0] = myCamera->getEye()[0];
	eyePos[1] = myCamera->getEye()[1];
	eyePos[2] = myCamera->getEye()[2];
	glm::vec3 up(0, 1, 0);
	float ratio = gWidth / float(gHeight);
	if (gHeight == 0) gHeight = 1;
	glViewport(0, 0, gWidth, gHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluperspective(fAngle, ratio, NCP, FCP);
	//Para seguir al objeto
	glm::vec3 cameraPosition = glm::vec3(myCamera->getCenter().x, myCamera->getCenter().y+2, myCamera->getCenter().z) + glm::vec3(-20 * cos((PI / 180) * (90 + carros[carCount-1]->getAlpha())), 0, -20 * sin((PI / 180) * (90 + carros[carCount-1]->getAlpha())));
	glm::mat4 LA = glm::lookAt(cameraPosition, myCamera->getCenter(), up);
	glMultMatrixf(glm::value_ptr(LA));
	mProjMatrix = mProjMatrix * LA;
	glMatrixMode(GL_MODELVIEW);
}

void reshape(int width, int height){
	gWidth = width;
	gHeight = height;
	if (gHeight == 0)
		gHeight = 1;

	float ratio = 1.0* gWidth / gHeight;

	glViewport(0, 0, gWidth, gHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluperspective(fAngle, ratio, NCP, FCP);
	glMatrixMode(GL_MODELVIEW);
	refresh();
}

//returns true if object is outside of skybox
bool collide(){ //Rotar el objeto
	double ang = carros[carCount - 1]->getAlpha();
	if (carros[carCount - 1]->getZMin() + carros[carCount - 1]->getTraslateZ() > texturedObjects[1]->getZMin() &&
		carros[carCount - 1]->getZMax() + carros[carCount - 1]->getTraslateZ() < texturedObjects[1]->getZMax() &&
		carros[carCount - 1]->getXMin() + carros[carCount - 1]->getTraslateX() > texturedObjects[1]->getXMin() &&
		carros[carCount - 1]->getXMax() + carros[carCount - 1]->getTraslateX() < texturedObjects[1]->getXMax() ){

		return false;

	}
	return true;
}

void reinicializar(){

	points=0;
	
	for(int i=0;i<picked.size();++i)
		picked[i]=false;

	for(int i=0;i<tiempos.size();++i)
		tiempos[i]=11;

	for(int i=0;i<trasX.size();++i){

		if(i%2==0)
			trasX[i]=(rand()%50)*(i+1);
		else
			trasX[i]=(-(rand()%50)*(i+1));
	}


	for(int i=0;i<trasZ.size();++i){

		if(i%2==1)
			trasZ[i]=(rand()%50)*i;
		else
			trasZ[i]=(-(rand()%50)*i);
	}

	refresh();
	
	carros.clear();
	loadFile("./Objects/London_Cab.obj");
	carros.push_back(object);
	loadFile("./Objects/tractor.obj");
	carros.push_back(object);
	loadFile("./Objects/eclipse.obj");
	carros.push_back(object);

	glm::vec3 eye(carros[carCount - 1]->getXMid(), carros[carCount - 1]->getYMid(), carros[carCount - 1]->getZMid());
	glm::vec3 center(carros[carCount - 1]->getXMid(), carros[carCount - 1]->getYMid(), carros[carCount - 1]->getZMid());
	//cout << "Eye: " << eye.x << " " << eye.y << " " << eye.z << endl;
	//cout << "Center: " << center.x << " " << center.y << " " << center.z << endl;
	myCamera = new CCamera(eye, center);
//		glm::vec3 cameraPosition = glm::vec3(myCamera->getCenter().x, myCamera->getCenter().y+2, myCamera->getCenter().z) + glm::vec3(-10 * cos((PI / 180) * (90 + carros[carCount-1]->getAlpha())), 0, -10 * sin((PI / 180) * (90 + carros[carCount-1]->getAlpha())));
}

void keyOperations(void){
	if (keyStates['w']){
		carros[carCount - 1]->speedUp(0.0025);
	}
	if (keyStates['s']){
		carros[carCount-1]->speedDown(0.00125);
	}
	if (keyStates['d']){
		carros[carCount-1]->turnRight(0.5);
	}
	if (keyStates['a']){
		carros[carCount-1]->turnLeft(0.5);
	}
}

void playMusic(){
	song = menu->getCurrentSong();
	switch (song){
		case GOODFEELING:
			principal->stopAllSounds();
			principal->play2D("Music/Good Feeling - Flo Rida.ogg", true);
			break;

		case ANTI:
			principal->stopAllSounds();
			principal->play2D("Music/Antikythera.mp3", true);
			break;

		case REVOLVERGUETTA:
			principal->stopAllSounds();
			principal->play2D("Music/Revolver.mp3", true);
			break;

		case DANZ:
			principal->stopAllSounds();
			principal->play2D("Music/danz.mp3", true);
			break;

		case ANIMALS:
			principal->stopAllSounds();
			principal->play2D("Music/Animals.ogg", true);
			break;
	}
}

void keyPressed(unsigned char key, int x, int y){
	keyStates[key] = true; // Set the state of the current key to pressed  
	switch (key) {
		case 27: //SCAPE
			if (menu->getCurrentMenu() == PRINCIPAL){
				exit(0);
			}
			else{
				click->play2D("Music/Rafaga.ogg", false);
				Sleep(500);//Parche del delay
				menu->setCurrentMenu(PRINCIPAL);
				menu->setCurrentOption(PLAY);
				option = NOTHING;
				optionCount = 1;

				carros.clear();
				loadFile("./Objects/London_Cab.obj");
				carros.push_back(object);
				loadFile("./Objects/tractor.obj");
				carros.push_back(object);
				loadFile("./Objects/eclipse.obj");
				carros.push_back(object);
			}
			break;
		case 13: //ENTER
			if (menu->getCurrentMenu() == PRINCIPAL){
				click->play2D("Music/Rafaga.ogg", false);
				Sleep(500);//Parche del delay
				option = menu->getCurrentOption();
				if (option == EXIT)
					exit(0);
			}
			else if (menu->getCurrentMenu() == SOUNDTRACK){
				click->play2D("Music/Rafaga.ogg", false);
				Sleep(500);//Parche del delay
				menu->setCurrentMenu(PRINCIPAL);
				menu->setCurrentOption(PLAY);
				option = NOTHING;
				optionCount = 1;
				playMusic();
			}
			else if(menu->getCurrentMenu() == CARS){
				click->play2D("Music/Rafaga.ogg", false);
				Sleep(500);//Parche del delay
				menu->setCurrentMenu(LEVEL);
				levelCount = 1;
				option = LEVELS;
				optionCount = 1;			
			}		
			else if(menu->getCurrentMenu() == LEVEL){ //OJO ARREGLAR
					click->play2D("Music/Rafaga.ogg", false);
					Sleep(500);//Parche del delay
					//
					setTime();
					//
					menu->setCurrentOption(PLAY);
					levelCount = 1;
					option = START;
					menu->setCurrentLevel(EASY);
					optionCount = 1;
					menu->setCurrentMenu(NOMENU);
					reinicializar();
			}
			break;
	}
}

void keyUp(unsigned char key, int x, int y){
	keyStates[key] = false; // Set the state of the current key to not pressed  
}

void setTime(){
	if (menu->getCurrentLevel() == EASY){
		minutes = 4; seg = 0;
	}
	else if (menu->getCurrentLevel() == MEDIUM){
		minutes = 2; seg = 30;
	}
	else if (menu->getCurrentLevel() == HARD){
		minutes = 1; seg = 30;
	}
}

void switchOption(){
	switch (optionCount){
		case 1:
			menu->setCurrentOption(PLAY);
			break;

		case 2:
			menu->setCurrentOption(MUSIC);
			break;

		case 4:
			menu->setCurrentOption(EXIT);
			break;

		case 3:
			menu->setCurrentOption(SCORES);
			break;
		case 5:
			menu->setCurrentOption(LEVELS);
			break;
		case 6:
			menu->setCurrentOption(START);
			break;
	}
}

void switchMusic(){
	switch (musicCount){
		case 1:
			menu->setCurrentSong(ANTI);
			break;

		case 2:
			menu->setCurrentSong(GOODFEELING);
			break;

		case 3:
			menu->setCurrentSong(REVOLVERGUETTA);
			break;

		case 4:
			menu->setCurrentSong(DANZ);
			break;

		case 5:
			menu->setCurrentSong(ANIMALS);
			break;

		case 6:
			menu->setCurrentSong(EXITM);
			break;
	}
}

void switchCar(){

	switch (carCount){
		case 1:
			menu->setCurrentCar(CARLOS);
			break;

		case 2:
			menu->setCurrentCar(KARINA);
			break;

		case 3:
			menu->setCurrentCar(FRANK);
			break;
	}


}

void switchLevel(){
	switch (levelCount){

		case 1:
			menu->setCurrentLevel(EASY);
			minutes = 1; seg = 0;
			winningPoints = 100;
			break;

		case 2:
			menu->setCurrentLevel(MEDIUM);	
			minutes = 2; seg = 0;
			winningPoints = 150;
			break;

		case 3:
			menu->setCurrentLevel(HARD);
			minutes = 3; seg = 0;
			winningPoints = 200;
			break;
	}
}

void processSpecialKeys(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_RIGHT:
			//
			break;

		case GLUT_KEY_LEFT:
			//
			break;

		case GLUT_KEY_DOWN: 
			if (menu->getCurrentMenu() == PRINCIPAL){
				click->play2D("Music/Hitmarker.ogg", false);
				optionCount++;
				if (optionCount > optionLimit){
					optionCount = optionLimit;
				}
				switchOption();
			}
			else if (menu->getCurrentMenu() == SOUNDTRACK){
				click->play2D("Music/Hitmarker.ogg", false);
				musicCount++;
				if (musicCount > musicLimit){
					musicCount = musicLimit;
				}
				switchMusic();
			}else if(menu->getCurrentMenu() == CARS){
				click->play2D("Music/Hitmarker.ogg", false);
				carCount++;
				if (carCount > carLimit){
					carCount = carLimit;
				}
				switchCar();	
			}else if(menu->getCurrentMenu() == LEVEL){
				click->play2D("Music/Hitmarker.ogg", false);
				levelCount++;
				if (levelCount > levelLimit){
					levelCount = levelLimit;
				}
				switchLevel();	
			}
			
			break;
	
		case GLUT_KEY_UP:
			if (menu->getCurrentMenu() == PRINCIPAL){
				click->play2D("Music/Hitmarker.ogg", false);
				optionCount--;
				if (optionCount < 1){
					optionCount = 1;
				}
				switchOption();
			}
			else if (menu->getCurrentMenu() == SOUNDTRACK){
				click->play2D("Music/Hitmarker.ogg", false);
				musicCount--;
				if (musicCount < 1){
					musicCount = 1;
				}
				switchMusic();
			}else if(menu->getCurrentMenu() == CARS){
				click->play2D("Music/Hitmarker.ogg", false);
				carCount--;
				if (carCount < 1){
					carCount = 1;
				}
				switchCar();	
			}else if(menu->getCurrentMenu() == LEVEL){
				click->play2D("Music/Hitmarker.ogg", false);
				levelCount--;
				if (levelCount < 1){
					levelCount = 1;
				}
				switchLevel();	
			}

			break;
	}
}

void mouseButton(int button, int state, int x, int y){

}

void cursorPos(int x, int y){

}

void calculateMinMax(int i){
	if (i == 0){
		object->setXMin(object->getVertex(0));
		object->setYMin(object->getVertex(1));
		object->setZMin(object->getVertex(2));
		//
		object->setXMax(object->getVertex(0));
		object->setYMax(object->getVertex(1));
		object->setZMax(object->getVertex(2));
	}
	else{
		if (object->getVertex(i * 3) < object->getXMin()){
			object->setXMin(object->getVertex(i * 3));
		}
		if (object->getVertex(i * 3 + 1) < object->getYMin()){
			object->setYMin(object->getVertex(i * 3 + 1));
		}
		if (object->getVertex(i * 3 + 2) < object->getZMin()){
			object->setZMin(object->getVertex(i * 3 + 2));
		}
		//
		if (object->getVertex(i * 3) > object->getXMax()){
			object->setXMax(object->getVertex(i * 3));
		}
		if (object->getVertex(i * 3 + 1) > object->getYMax()){
			object->setYMax(object->getVertex(i * 3 + 1));
		}
		if (object->getVertex(i * 3 + 2) > object->getZMax()){
			object->setZMax(object->getVertex(i * 3 + 2));
		}
	}
}

void destroy(){
	int win = glutGetWindow();
	glutDestroyWindow(win);
	principal->drop();
	click->drop();
	m_program.~CGLSLProgram();
	m_texture_program.~CGLSLProgram();
}

void loadFile(string route){
	object = new CObject(); //para cargar el carro

	//open file
	std::fstream file(route, std::fstream::in | std::fstream::out);
	
	//
	int i, j = 0, indexMinMax = 0;
	GLfloat point; //read vertex
	vector <GLuint> fc; //store faces
	GLuint pt; //read faces
	string type, subtype, line; 
	//OBJ
		fc.clear();
		while (!file.eof()){
			subtype = type.substr(0, 1);
			if (subtype == "#"){ //Comments
				std::getline(file, line);
			}
			if (type == "vt"){ //Texture Vertex
				std::getline(file, line);
			}
			if (type == "vn"){ //Normal Vertexx
				std::getline(file, line);
			} 
			if (type == "v"){ //Vertex
				for (i = 0; i < 3; i++){
					file >> point;
					object->setVertex(point);
				}
				object->setColor(0, 1, 0);
				
				object->setNumVertex(object->getNumVertex() + 1);
				//
				calculateMinMax(indexMinMax);
				indexMinMax++;
				//
			}
			if (type == "f"){ //faces v/vt/vn
				std::getline(file, line);
				char * tok = strtok(&line[0u], " ");
				while (tok != NULL){
					string num = tok;
					size_t pos = num.find("/");
					num = num.substr(0, pos);
					pt = stoi(num);
					fc.push_back(pt);
					tok = strtok(NULL, " ");
				}
				object->setNumFaces(object->getNumFaces() + 1);
				for (j = 1; j < fc.size() - 1; j++){
					object->setIndex(fc[0] - 1);
					object->setIndex(fc[j] - 1);
					object->setIndex(fc[j + 1] - 1);
				}
				fc.clear();
			}
			file >> type;
		}

	object->setXMid((object->getXMin() + object->getXMax()) / 2);
	object->setYMid((object->getYMin() + object->getYMax()) / 2);
	object->setZMid((object->getZMin() + object->getZMax()) / 2);


	object->createBB();
	object->FaceNormals();
	object->VertexNormals();

	if (!object->initialize())
		exit(EXIT_FAILURE);

	file.close();
}

//Enter full screen
void fullScreenMode(){
	glutGameModeString("800x600:32");
	if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
		glutEnterGameMode();
	else {
		printf("The select mode is not available\n");
		exit(1);
	}
	
}

void loadScores(){
	puntuaciones.clear();
	int puntuacion;
	std::fstream file("./scores.txt", std::fstream::in);
	while(!file.eof()){
		file>>puntuacion;
		puntuaciones.push_back(puntuacion);	
	}
	sort(puntuaciones.begin(),puntuaciones.end());

	file.close();
}

int main(int argc, char** argv){
	gWidth = 1024;
	gHeight = 600;

	for (int i = 0; i < 256; i++) keyStates[i] = false;

	glutInit(&argc, argv);	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(gWidth, gHeight);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - gWidth) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - gHeight) / 2);
	glutCreateWindow("Proyecto");

	fullScreenMode();

	if (!initialize())
		exit(EXIT_FAILURE);

	if (!principal || !click)
      return 0;

	loadFile("./Objects/London_Cab.obj");
	carros.push_back(object);
	loadFile("./Objects/tractor.obj");
	carros.push_back(object);
	loadFile("./Objects/eclipse.obj");
	carros.push_back(object);

	loadFile("./Objects/coin.obj");
	for (int i = 0; i < 5; i++){
		coins.push_back(object);
		coins.back()->setAmbientObject(0.82f, 0.68f, 0.27f);
	}
	
	loadFile("./Objects/hourglass.obj");
	hourglass.push_back(object);
	hourglass.back()->setAmbientObject(1.f, 0.f, 0.f);

	CTexturedObject* sky = new CTexturedObject("Texture/sky.obj", "Texture/sky.jpg", "Texture/sky.txt");
	texturedObjects.push_back(sky);
	CTexturedObject* floor = new CTexturedObject("Texture/floor.obj", "Texture/floor.png", "Texture/floor.txt");
	texturedObjects.push_back(floor);

	menuTexture = new CTexturedObject("Texture/menu.obj", "Texture/menu.jpg", "Texture/menu.txt");

	for(int i=0;i<15;++i)
		picked.push_back(false);

	for(int i=0;i<15;++i){
		if(i%2==0)
			trasX.push_back((rand()%50)*(i+1));
		else
			trasX.push_back(-(rand()%50)*(i+1));
	}
	
	for(int i=0;i<15;++i){
		if(i%2==1)
			trasZ.push_back((rand()%50)*i);
		else
			trasZ.push_back(-(rand()%50)*i);
	}

	for(int i=0;i<15;++i)
		tiempos.push_back(11);

	for (int i = 0; i<15; ++i){
		if (i % 2 == 0)
			trasXClock.push_back((rand() % 50)*(i + 1));
		else
			trasXClock.push_back(-(rand() % 50)*(i + 1));
	}

	for (int i = 0; i<15; ++i){
		if (i % 2 == 1)
			trasZClock.push_back((rand() % 50)*i);
		else
			trasZClock.push_back(-(rand() % 50)*i);
	}

	loadScores();

	ld[0]=1;
	ld[1]=0.8;
	ld[2]=0;
	ls[0]=0.0;
	ls[1]=0.0;
	ls[2]=1.0;
	ambient[0]=0;
	ambient[1]=0.5;
	ambient[2]=0.7;

	init();
	principal->play2D("Music/Antikythera.mp3", true); //reproduce la musica
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glutMainLoop();

	destroy();
	return 0;
}