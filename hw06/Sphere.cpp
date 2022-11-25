#include "Sphere.hpp"

#include "Utility.hpp"

Sphere::Sphere(float radius, raylib::Color color) : mesh(raylib::Mesh::Sphere(Utility::map(radius, 0.0001f, 0.001f, 0.05f, 0.5f), 8, 16)), material() {
	material.maps[MATERIAL_MAP_DIFFUSE].color = color;
}

void Sphere::draw(const raylib::Vector3& position) {
	DrawMesh(mesh, material, MatrixMultiply(MatrixIdentity(), MatrixTranslate(position.x, position.y, position.z)));
}

Sphere::Sphere(Sphere&& sphere) : mesh(std::move(sphere.mesh)), material(std::move(sphere.material)) {}
//Sphere::Sphere(const Sphere& sphere) : mesh(sphere.mesh), material() {
//	material.maps[MATERIAL_MAP_DIFFUSE].color = sphere.material.maps[MATERIAL_MAP_DIFFUSE].color;
//}
