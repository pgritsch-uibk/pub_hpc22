#include "Game.hpp"
#include "Utility.hpp"
#include <iostream>

constexpr int GLSL_VERSION = 330;


Game::Game(int width, int height, const std::string& title, int capFPS)
    : window(width, height, title), nBody(5000) {
	camera = raylib::Camera3D(position, target, up, 45.0f, CAMERA_PERSPECTIVE);

	shader = LoadShader("./shader/lighting.vs", "./shader/lighting.fs");
	shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

	int ambientLoc = GetShaderLocation(shader, "ambient");
	SetShaderValue(shader, ambientLoc, (float[4]){ 0.0f, 0.0f, 0.0f, 1.0f }, SHADER_UNIFORM_VEC4);

	std::for_each(nBody.particles.begin(), nBody.particles.end(), [&](Particle& particle) {
		raylib::Color color_new = raylib::Color::FromHSV(0.0f, 1.0f, 1.0f);
		spheres.emplace_back(particle.radius, color_new);
	});

	std::for_each(spheres.begin(), spheres.end(), [&](Sphere& sphere) {
		sphere.material.shader = shader;
	});

	light = CreateLight(LIGHT_POINT, (Vector3){ 20, 20, 20 }, Vector3Zero(), WHITE, shader);

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

	SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], (float[3]){ position.x, position.y, position.z }, SHADER_UNIFORM_VEC3);
//	UpdateLightValues(shader, light);
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