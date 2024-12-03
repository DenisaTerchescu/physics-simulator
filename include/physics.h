#pragma once

#include <vector>
#include <glm/glm.hpp>

enum particle : int{
	CUBE, SPHERE, CYLINDER
};

struct Object
{
	glm::vec3 pos = {};
	float padding;
	glm::vec3 size = {};
	float padding2;

	glm::vec3 velocity = {};
	float padding3;

	glm::vec3 gravity = { 0,-9.81,0 };
	particle type = CUBE;
	
	glm::vec3 calculateMinimumPoint() { return pos - size / 2.f; }

	glm::vec3 calculateMaximumPoint() { return pos + size / 2.f; }

};

struct PhysicsSimulator
{
	std::vector<Object> objects;
	glm::vec3 glassContainer = { 20, 20, 20 };

	void addCube(glm::vec3 pos, glm::vec3 size);
	void addSphere(glm::vec3 pos, float r);

	void addCylindre(glm::vec3 pos, float r, float h);

	void update(float deltaTime);
};
