#include "menu.h"

extern int gWidth, gHeight, points;
extern vector<int> puntuaciones;
extern bool playMusicGameOver;
extern int winningPoints;

CMenu::CMenu(){
	currentMenu = PRINCIPAL;
	currentOption = PLAY;
	currentSong = ANTI;
	currentCar = CARLOS;
	currentLevel = EASY;
	sound = "Music/Good Feeling - Flo Rida.ogg";
	font = GLUT_BITMAP_9_BY_15;
	color[0] = 0.f; color[1] = 1.f; color[2] = 1.f;
	colorSelect[0] = 1.f; colorSelect[1] = 1.f; colorSelect[2] = 1.f;
	game = createIrrKlangDevice();
}

CMenu::~CMenu(){
	//
}

void CMenu::displayMenu(){
	void *font = GLUT_BITMAP_9_BY_15;

	setOrthographicProjection();

	glPushMatrix();
	glLoadIdentity();
	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentOption == PLAY)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 150) / 2, (gHeight - 20) / 2, 5, font, "JUGAR");

	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentOption == MUSIC)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 260) / 2, (gHeight + 40) / 2, 5, font, "CAMBIAR MUSICA");

	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentOption == SCORES)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 320) / 2, (gHeight + 100) / 2, 5, font, "PUNTUACIONES ALTAS");

	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentOption == EXIT)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 140) / 2, (gHeight + 160) / 2, 5, font, "SALIR");
	glPopMatrix();

	restorePerspectiveProjection();
}

void CMenu::displayMusicMenu(){
	setOrthographicProjection();

	glPushMatrix();
	glLoadIdentity();
	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentSong == ANTI)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 310) / 2, (gHeight - 150) / 2, 5, font, "ANTIKYTHERA MECHANISM");
	
	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentSong == GOODFEELING)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 320) / 2, (gHeight - 100) / 2, 5, font, "GOOD FEELING - FLO-RIDA");

	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentSong == REVOLVERGUETTA)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 500) / 2, (gHeight - 50) / 2, 5, font, "REVOLVER vs MADONNA - DAVID GUETTA");

	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentSong == DANZ)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 310) / 2, (gHeight) / 2, 5, font, "DANZ! - LA VIDA BOHEME");

	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentSong == ANIMALS)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 280) / 2, (gHeight + 50) / 2, 5, font, "ANIMALS - NICKELBACK");

	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentSong == EXITM)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 70) / 2, (gHeight + 150) / 2, 5, font, "ATRAS");
	glPopMatrix();

	restorePerspectiveProjection();
}

void CMenu::displayCarsMenu(){

	setOrthographicProjection();

	glPushMatrix();
	glLoadIdentity();
	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentCar == CARLOS)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 150) / 2, (gHeight - 150), 5, font, "LONDON CAB");

	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentCar == KARINA)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 100) / 2, (gHeight - 120), 5, font, "TRACTOR");

	glColor3f(0.0f, 1.0f, 1.0f);
	if (currentCar == FRANK)
		glColor3f(1.0f, 1.0f, 1.0f);
	renderSpacedBitmapString((gWidth - 240) / 2, (gHeight - 90), 5, font, "MITSUBISHI ECLIPSE");
	glPopMatrix();

	restorePerspectiveProjection();
}

void CMenu::displayLevelsMenu(){

	setOrthographicProjection();

	glPushMatrix();
	glLoadIdentity();
		glColor3f(0.0f, 1.0f, 1.0f);
		if (currentLevel == EASY)
			glColor3f(1.0f, 1.0f, 1.0f);
		renderSpacedBitmapString((gWidth - 100) / 2, (gHeight - 30) / 2, 5, font, "FACIL");
		
		glColor3f(0.0f, 1.0f, 1.0f);
		if (currentLevel == MEDIUM)
			glColor3f(1.0f, 1.0f, 1.0f);
		renderSpacedBitmapString((gWidth - 100) / 2, (gHeight + 40) / 2, 5, font, "MEDIO");
		
		glColor3f(0.0f, 1.0f, 1.0f);
		if (currentLevel == HARD)
			glColor3f(1.0f, 1.0f, 1.0f);
		renderSpacedBitmapString((gWidth - 120) / 2, (gHeight + 110) / 2, 5, font, "DIFICIL");
		glPopMatrix();

	restorePerspectiveProjection();
}

void CMenu::displayGameOver(){
	void *font = GLUT_BITMAP_9_BY_15;

	setOrthographicProjection();

	glPushMatrix();
	glLoadIdentity();
	glColor3f(0.0f, 1.0f, 1.0f);

	if (points >= winningPoints){
		renderSpacedBitmapString((gWidth - 250) / 2, (gHeight - 30) / 2, 5, font, "OBJETIVO ALCANZADO");
		if (playMusicGameOver)
			game->play2D("Music/Win.mp3", false);
	}
	else{
		renderSpacedBitmapString((gWidth - 380) / 2, (gHeight - 30) / 2, 5, font, "NO HAS ALCANZADO EL OBJETIVO");
		if (playMusicGameOver)
			game->play2D("Music/Lose.mp3", false);
	}

	if (points>puntuaciones[0])
		renderSpacedBitmapString((gWidth - 400) / 2, (gHeight + 40) / 2, 5, font, "HAS HECHO UNA PUNTUACION ALTA");

	renderSpacedBitmapString((gWidth - 370) / 2, (gHeight + 80) / 2, 5, font, "PRESIONE ESC PARA CONTINUAR");

	restorePerspectiveProjection();
}

void CMenu::setCurrentOption(options opt){
	currentOption = opt;
}

options CMenu::getCurrentOption(){
	return currentOption;
}

void CMenu::setCurrentMenu(menus opt){
	currentMenu = opt;
}

menus CMenu::getCurrentMenu(){
	return currentMenu;
}

void CMenu::setSound(string snd){
	sound = snd;
}

string CMenu::getSound(){
	return sound;
}

void CMenu::setCurrentSong(music opt){
	currentSong = opt;
}

music CMenu::getCurrentSong(){
	return currentSong;
}

void CMenu::setCurrentCar(cars opt){
	currentCar=opt;
}

cars CMenu::getCurrentCar(){
	return currentCar;
}

void CMenu::setCurrentLevel(levels opt){
	currentLevel=opt;
}

levels CMenu::getCurrentLevel(){
	return currentLevel;
}