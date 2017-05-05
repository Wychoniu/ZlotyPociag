#include "Level.h"
#include "Cannon.h"
#include "Bullet.h"
#include "Particle.h"
#include <vector>
#include "FunctionsPack.h"

class Game
{
	struct vec3 {
		float x, y, z;
	};

	struct SCameraState {
		vec3 pos;
		vec3 dir;
		float speed;
		float velM, velS;
		float velRY, velRX;
	};

public:
	Game();
	~Game();
	void showScene();
	void cannnonUpgradeClicked(int);
	bool checkTime();
	void loadLevel(int);
	void cleanMem();

	SCameraState player;
	vec3 LightPos;
	bool lightEnabled = true;
	Level *level;
	Cannon *cannon;
	std::string message = "";
	int currentLevel = 1;
	int points;
	int cash;

	std::vector < Bullet* > bullets;
	std::vector < Particle* > particles;

private:
	int timer;
};
