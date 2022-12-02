#include "Game.hpp"
#include "Utility.hpp"
#include <algorithm>
#include <iostream>

constexpr int GLSL_VERSION = 330;

Game::Game(int width, int height, const std::string& title, int capFPS)
    : window(width, height, title), nBody(100),
      shader("./shader/lighting.vs", "./shader/lighting.fs"),
      light(LightType::LIGHT_POINT, true, { 20, 20, 20 }, raylib::Vector3::Zero(), WHITE, 0.0f) {
	camera = raylib::Camera3D(position, target, up, 45.0f, CAMERA_PERSPECTIVE);

	shader.locs[SHADER_LOC_VECTOR_VIEW] = shader.GetLocation("viewPos");

	int ambientLoc = shader.GetLocation("ambient");
	shader.SetValue(ambientLoc, &ambientColor, SHADER_UNIFORM_VEC4);

	std::for_each(nBody.particles.begin(), nBody.particles.end(), [&](Particle& particle) {
		raylib::Color color_new = raylib::Color::FromHSV(0.0f, 1.0f, 1.0f);
		spheres.emplace_back(particle.radius, color_new);
	});

	std::for_each(spheres.begin(), spheres.end(),
	              [&](Sphere& sphere) { sphere.material.shader = shader; });

	light.defineShader(shader);

	camera.SetMode(CAMERA_CUSTOM);

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

	shader.SetValue(shader.locs[SHADER_LOC_VECTOR_VIEW], &position, SHADER_UNIFORM_VEC3);
}

void Game::render() {
	BeginDrawing();
	{
		window.ClearBackground(BLACK);

		if(debug) {
			DrawFPS(10, 10);
		}

		camera.BeginMode();
		{
			DrawGrid(10, 5.0f);

			if (debug) {
				light.draw();
			}

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