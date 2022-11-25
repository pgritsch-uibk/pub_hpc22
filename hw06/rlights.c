#include "rlights.hpp"

#include <raylib.h>
#include <printf.h>

static int lightsCount = 0;

void UpdateLightValues(Shader shader, Light light) {
	SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
	SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);

	float position[3] = { light.position.x, light.position.y, light.position.z };
	SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);

	float target[3] = { light.target.x, light.target.y, light.target.z };
	SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);

	float color[4] = { (float)light.color.r / (float)255, (float)light.color.g / (float)255,
		               (float)light.color.b / (float)255, (float)light.color.a / (float)255 };
	SetShaderValue(shader, light.colorLoc, color, SHADER_UNIFORM_VEC4);
}

Light CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader) {
	Light light = { 0 };

	if(lightsCount < MAX_LIGHTS) {
		printf("Light created successfully");
		light.enabled = true;
		light.type = type;
		light.position = position;
		light.target = target;
		light.color = color;

		light.enabledLoc = GetShaderLocation(shader, "lights.enabled");
		light.typeLoc = GetShaderLocation(shader, "lights.type");
		light.positionLoc = GetShaderLocation(shader, "lights.position");
		light.targetLoc = GetShaderLocation(shader, "lights.target");
		light.colorLoc = GetShaderLocation(shader, "lights.color");

		UpdateLightValues(shader, light);

		lightsCount++;
	}

	return light;
}
