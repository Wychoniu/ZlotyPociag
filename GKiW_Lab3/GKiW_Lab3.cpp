#include "stdafx.h"
#include "Game.h"
#include "MainMenu.h"
#include "Bullet.h"
#include "CannonUpgradeMenu.h"
#include "GKiW_Lab3.h"
#include <string>
#include <iostream>
#include <mmsystem.h>
#include "FunctionsPack.h"
#include <time.h>

int w = 1366;
int h = 768;

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	srand(time(NULL) ^ clock());

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(w, h);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("Zloty pociag");

	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnReshape);
	glutKeyboardFunc(OnKeyPress);
	glutKeyboardUpFunc(OnKeyUp);
	glutSpecialFunc(OnSpecialKeyPress); // klawisze specjalne (strzalki, F1-F12, PgUp/PgDn, Home, End, Delete, Insert)
	glutSpecialUpFunc(OnSpecialKeyUp);
	glutPassiveMotionFunc(OnMove);
	glutMouseFunc(onMouseButton);
	glutTimerFunc(17, OnTimer, 0);

	//glutFullScreen();
	m_menu = new MainMenu();
	//game = new Game();

	glutMainLoop();

	return 0;
}

#pragma region Obsluga wejscia

bool keystate[256];
bool keystate_special[256];

void OnKeyPress(unsigned char key, int x, int y) {
	if (!keystate[key]) {
		OnKeyDown(key, x, y);
	}
	keystate[key] = true;
}

void OnSpecialKeyPress(int key, int x, int y) {
	if (!keystate_special[key]) {
		OnSpecialKeyDown(key, x, y);
	}
	keystate_special[key] = true;
}

void OnKeyDown(unsigned char key, int x, int y) {
	if (key == 27) {
		glutLeaveMainLoop();
	}
}

void OnSpecialKeyDown(int key, int x, int y) {
}

void OnKeyUp(unsigned char key, int x, int y) {
	keystate[key] = false;
}

void OnSpecialKeyUp(int key, int x, int y) {
	keystate_special[key] = false;
}

void OnMove(int x, int y) {
	if (CurrentState == play) {
		mouseX = x;
		mouseY = y;
	}
	//std::cout << "X:" << x << " Y:" << y << std::endl;
}

void onMouseButton(int button, int state, int x, int y) {
	if (CurrentState == menu) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			switch(m_menu->checkItems(x, y)) {
			case 0:
				game = new Game();
				CurrentState = play;
				glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
				horizontalAngle = 160.0f;
				verticalAngle = 0;
				delete m_menu;
				break;
			case 1:
				break;
			case 2:
				glutLeaveMainLoop();
				break;
			}
		}
	}
	if (CurrentState == cannonUpgrade) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			int o = cannonMenu->checkItems(x, y);
			if (o >= 0) {
				game->cannnonUpgradeClicked(o);
				if (o == 4) {
					delete cannonMenu;
					glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
					horizontalAngle = 160.0f;
					verticalAngle = 0;
					CurrentState = play;
				}
			}
		}
	}
	else if (CurrentState == play) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			if (true) { //game->cannon->reloading == 0
				PlaySound("cannon.wav", NULL, SND_ASYNC | SND_FILENAME);
				game->cannon->reloading = (game->cannon->fireRate - (0.6*game->cannon->fireRateLevel)) * 20;
				game->bullets.push_back(new Bullet(game->player.pos.x + game->player.dir.x * 1.2f,
													game->player.pos.y + game->player.dir.y * 1.2f,
													game->player.pos.z + game->player.dir.z * 1.2f,
													game->player.dir.x, 
													game->player.dir.y, 
													game->player.dir.z, 
													game->cannon->ballSpeed, 
													verticalAngle, 
													game->level->wind));
			}
		}
	}
}

#pragma endregion

void OnTimer(int id) {

	glutTimerFunc(17, OnTimer, 0);

	Tm = glutGet(GLUT_ELAPSED_TIME); // Ile milisekund uplynelo od momentu uruchomienia programu?

	if (CurrentState == play) {
		//Ruch kamery
		horizontalAngle += 0.15f * float(glutGet(GLUT_WINDOW_WIDTH) / 2 - mouseX);
		verticalAngle += 0.15f  * float(glutGet(GLUT_WINDOW_HEIGHT) / 2 - mouseY);
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

		if (keystate['w']) {
			verticalAngle += 0.5;
		}
		if (keystate['s']) {
			verticalAngle -= 0.5;
		}
		if (keystate['a']) {
			horizontalAngle += 0.5;
		}
		if (keystate['d']) {
			horizontalAngle -= 0.5;
		}

		if (verticalAngle > 90) verticalAngle = 90;
		if (verticalAngle < 0) verticalAngle = 0;

		game->hAngle = horizontalAngle;
		game->vAngle = verticalAngle;

		game->player.dir.x = cos(verticalAngle*3.14 / 180) * sin(horizontalAngle*3.14 / 180);
		game->player.dir.y = sin(verticalAngle*3.14 / 180);
		game->player.dir.z = cos(verticalAngle*3.14 / 180) * cos(horizontalAngle*3.14 / 180);

		//std::cout << horizontalAngle << " " << verticalAngle << std::endl;

		if (game->cannon->reloading == 0 && keystate[' ']) {
			PlaySound("cannon.wav", NULL, SND_ASYNC | SND_FILENAME);
			game->cannon->reloading = (game->cannon->fireRate - (0.6*game->cannon->fireRateLevel)) * 20;
			game->bullets.push_back(new Bullet(game->player.pos.x + game->player.dir.x * 1.2,
				game->player.pos.y + game->player.dir.y * 1.2,
				game->player.pos.z + game->player.dir.z * 1.2,
				game->player.dir.x,
				game->player.dir.y,
				game->player.dir.z,
				game->cannon->ballSpeed,
				verticalAngle,
				game->level->wind));
		}

		// Znalezienie kierunku prostopadlego
		vec3 per;
		per.x = -game->player.dir.z;
		per.z = game->player.dir.x;

		if (game->cannon->reloading > 0)
			game->cannon->reloading--;
	}

	if (CurrentState == postLevel) {
		if (wait > 0) wait--;
	}

	if (CurrentState == menu) {
		m_menu->show();
	}
	else if (CurrentState == play) {
		if (game->checkTime()) {
			CurrentState = postLevel;
			glDisable(GL_TEXTURE_2D);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			game->cleanMem();
			game->cash += game->level->curentCash;
			game->cash += (int)(game->level->curentPoints * 0.6);
			if (game->level->curentPoints >= game->level->requiredPoints) {
				game->points += game->level->curentPoints;
				game->currentLevel++;
			}
			wait = 200;
		}
		else if (game->hp <= 0) {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			wait = 200;
			CurrentState = postLevel;
			game->cleanMem();
		}
		else
			game->showScene();
	}
	else if (CurrentState == postLevel) {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (game->currentLevel > 5)
			printText(20, 380, 10, "Wygrales! Twoje punkty: " + std::to_string(game->points), 1, 1, 1);
		else if (game->level->curentPoints >= game->level->requiredPoints)
			printText(20, 380, 10, "Przeszedles do poziomu " + std::to_string(game->currentLevel), 1, 1, 1);
		else if (game->hp <= 0)
			printText(20, 380, 10, "Przegrales :(. Zacznij gre od nowa!", 1, 1, 1);
		else
			printText(80, 380, 10, "Nie zdobyles wymaganej liczby punktow!", 1, 1, 1);

		if (wait == 0) {
			if (game->currentLevel > 5 || game->hp <= 0)
				glutLeaveMainLoop();
			else {
				cannonMenu = new CannonUpgradeMenu(game);
				CurrentState = cannonUpgrade;
			}
		}
	}
	else if (CurrentState == cannonUpgrade) {
		cannonMenu->show();
	}

	glutSwapBuffers();
	glFlush();
	glutPostRedisplay();
}

void OnRender() {
}

void OnReshape(int width, int height) {
	//glutReshapeWindow(w, h);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glViewport(0, 0, width, height);
	//gluPerspective(60.0f, (float)width / height, .01f, 100.0f);
	h = height;
	w = width;

	const float aspectRatio = ((float)width) / height;
	float xSpan = 1;
	float ySpan = 1;

	if (aspectRatio > 1) {
		xSpan *= aspectRatio;
	}
	else {
		ySpan = xSpan / aspectRatio;
	}

	gluPerspective(60, (double)width / (double)height, 0.01f, 100.0f);

	glViewport(0, 0, width, height);
}
