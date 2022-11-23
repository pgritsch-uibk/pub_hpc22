#pragma once

#include "NBody.hpp"
#include <raylib-cpp.hpp>

class Game {
  private:
	raylib::Window window;
	raylib::Camera3D camera;
	bool running = true;
	bool debug = true;
	bool paused = true;

	raylib::Vector3 position = { 20.0f, 20.0f, 20.0f };
	raylib::Vector3 target = { 0.0f, 0.0f, 0.0f };
	raylib::Vector3 up = { 0.0f, 1.0f, 0.0f };

	NBody nbody;

  public:
	Game(int width, int height, const std::string& title, int capFPS);
	~Game();

	void init();
	void handleInput();
	void update();
	void render();
	bool isRunnign();
};
