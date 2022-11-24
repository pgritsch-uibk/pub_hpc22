#include "Game.hpp"
#include <iostream>
#include "Utility.hpp"

Game::Game(int width, int height, const std::string& title, int capFPS)
    : window(width, height, title), nBody(5000), meshes() {
	camera = raylib::Camera3D(position,            // position
	                          target,              // target
	                          up,                  // up
	                          45.0f,               // fovy
	                          CAMERA_PERSPECTIVE); // type

	std::for_each(nBody.particles.begin(), nBody.particles.end(), [&](Particle& particle) {
		raylib::Color color_new = raylib::Color::FromHSV(dist(rng), 1.0f, 1.0f);
		meshes.emplace_back(raylib::Mesh::Sphere(Utility::map(particle.radius, 0.0001f, 0.001f, 0.1f, 0.5f), 16, 16));
		materials.emplace_back();
		materials.back().maps[MATERIAL_MAP_DIFFUSE].color = color_new;
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
				DrawMesh(meshes[i], materials[i], MatrixMultiply(MatrixIdentity(), MatrixTranslate(p.position.x, p.position.y, p.position.z)));
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