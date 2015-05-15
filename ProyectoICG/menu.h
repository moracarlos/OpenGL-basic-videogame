#pragma once

#include "main.h"

enum options{
	NOTHING,
	PLAY,
	MUSIC,
	SCORES,
	EXIT,
	LEVELS,
	START,
	OVER
};

enum music{
	ANTI,
	GOODFEELING,
	REVOLVERGUETTA,
	DANZ,
	ANIMALS,
	EXITM
};

enum menus{
	PRINCIPAL,
	SOUNDTRACK,
	CARS,
	LEVEL,
	HIGHSCORES,
	NOMENU
};

enum levels{
	EASY,
	MEDIUM,
	HARD
};

enum cars{
	CARLOS,
	KARINA,
	FRANK
};

class CMenu{
	private:
		options currentOption;
		menus currentMenu;
		music currentSong;
		cars currentCar;
		levels currentLevel;
		string sound;
		void *font;
		GLfloat color[3];
		GLfloat colorSelect[3];
		ISoundEngine* game;

	public:
		CMenu();
		~CMenu();
		void displayMenu();
		void displayMusicMenu();
		void displayCarsMenu();
		void displayLevelsMenu();
		void displayGameOver();
		void setCurrentOption(options opt);
		options getCurrentOption();
		void setCurrentMenu(menus opt);
		menus getCurrentMenu();
		void setSound(string snd);
		string getSound();
		void setCurrentSong(music opt);
		music getCurrentSong();
		void setCurrentCar(cars opt);
		cars getCurrentCar();
		void setCurrentLevel(levels opt);
		levels getCurrentLevel();
};