#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Object
{
	glm::vec3 pos = {};
	glm::vec3 size = {};
	glm::vec3 velocity = {};
	glm::vec3 acceleration = {};
	int type = 0; //0 for cube, 1 for sphere
	
	glm::vec3 getMin() 
	{
		if (type == 0 || type == 1)
		{
			return pos - size / 2.f;
		}
	}

	glm::vec3 getMax()
	{
		if (type == 0 || type == 1)
		{
			return pos + size / 2.f;
		}
	}
};

struct PhysicsEngine
{

	std::vector<Object> objects;

	void addCube(glm::vec3 pos, glm::vec3 size);
	void addSphere(glm::vec3 pos, float r);

	void update(float deltaTime);

	glm::vec3 glassContainer = { 20, 20, 20 };
};