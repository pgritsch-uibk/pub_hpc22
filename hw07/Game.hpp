#pragma once

#include "Sphere.hpp"
#include "NBody.hpp"
#include <raylib-cpp.hpp>
#include <random>

//#include "rlights.hpp"
#include "Light.hpp"

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

	std::vector<Sphere> spheres;

	raylib::Vector4 ambientColor = {0.0f, 0.0f, 0.0f, 1.0f};

	NBody nBody;

	raylib::Material material;
	raylib::Shader shader;
	Light light;

//	std::default_random_engine rng{std::random_device{}()};
//	std::uniform_real_distribution<float> dist{0.0f, 360.0f};

  public:
	Game(int width, int height, const std::string& title, int capFPS);
	~Game();

	void init();
	void handleInput();
	void update();
	void render();
	bool isRunnign();
};
