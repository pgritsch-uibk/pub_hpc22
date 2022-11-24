#include "Game.hpp"
#include <iostream>
#include "Utility.hpp"

Game::Game(int width, int height, const std::string& title, int capFPS)
    : window(width, height, title), nBody(5000) {
	camera = raylib::Camera3D(position,
	                          target,
	                          up,
	                          45.0f,
	                          CAMERA_PERSPECTIVE);

	std::for_each(nBody.particles.begin(), nBody.particles.end(), [&](Particle& particle) {
		raylib::Color color_new = raylib::Color::FromHSV(dist(rng), 1.0f, 1.0f);
		spheres.emplace_back(particle.radius, color_new);
	});

	camera.SetMode(CAMERA_CUSTOM); // Set a first person camera mode

	window.SetTargetFPS(capFPS);
}

Game::~Game() = default;

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
	if(IsKeyDown('E')) {
		position.z += speed;
	}
	if(IsKeyDown('C')) {
		position.z -= speed;
	}
}

void Game::update() {
	camera.SetPosition(position);
	camera.Update();

	static int x = 0;

	if(x++ % 2 == 0 && !paused) {
		nBody.update();
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

			for(std::size_t i = 0; i < nBody.particles.size(); ++i) {
				Particle& p = nBody.particles[i];
				spheres[i].draw(p.position);
			}
		}
		camera.EndMode();
	}
	EndDrawing();
}

bool Game::isRunnign() {
	return !window.ShouldClose() && running;
}

void Game::init() {}