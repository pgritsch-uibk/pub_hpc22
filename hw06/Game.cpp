#include "Game.hpp"
#include <iostream>

Game::Game(int width, int height, const std::string& title, int capFPS)
    : window(width, height, title), nbody(500) {
	camera = raylib::Camera3D(position,            // position
	                          target,              // target
	                          up,                  // up
	                          45.0f,               // fovy
	                          CAMERA_PERSPECTIVE); // type

	camera.SetMode(CAMERA_CUSTOM); // Set a first person camera mode

	window.SetTargetFPS(capFPS);
}

Game::~Game() {}

// include poll event
void Game::handleInput() {
	static float speed = 1.0f;

	if(IsKeyPressed('P')) {
		debug = !debug;
	}

	if(IsKeyPressed('F')) {
		speed += 0.5f;
	}
	if(IsKeyPressed('R')) {
		speed -= 0.5f;
	}

	if(IsKeyPressed('Q')) {
		paused = !paused;
	}

	if(IsKeyDown('W')) {
		position.y += speed;
	}
	if(IsKeyDown('A')) {
		position.x -= speed;
	}
	if(IsKeyDown('S')) {
		position.y -= speed;
	}
	if(IsKeyDown('D')) {
		position.x += speed;
	}
}

void Game::update() {
	camera.SetPosition(position);
	camera.Update();

	static int x = 0;

	if(x++ % 2 == 0 && !paused) {
		nbody.update();
	}
}

void Game::render() {
	BeginDrawing();
	{
		window.ClearBackground(RAYWHITE);

		if(debug) {
			DrawFPS(10, 10);
		}

		camera.BeginMode();
		{
			DrawGrid(10, 5.0f);
			std::for_each(
			    nbody.particles.begin(), nbody.particles.end(),
			    [](const Particle& particle) { DrawSphere(particle.position, 0.5, RED); });
		}
		camera.EndMode();
	}
	EndDrawing();
}

bool Game::isRunnign() {
	return !window.ShouldClose() && running;
}

void Game::init() {}