#include "physics.h"



void PhysicsEngine::addCube(glm::vec3 pos, glm::vec3 size)
{
	Object o;

	o.pos = pos;
	o.size = size;

	objects.push_back(o);
}

void PhysicsEngine::addSphere(glm::vec3 pos, float r)
{
	Object o;

	o.type = 1;
	o.pos = pos;
	o.size = {r*2, r*2, r*2};

	objects.push_back(o);
}

void PhysicsEngine::update(float deltaTime)
{

	for (int i = 0; i < objects.size(); i++)
	{
		Object& a = objects[i];

		a.acceleration += glm::vec3(0, -9.81, 0);

		{
			a.velocity += deltaTime * a.acceleration;
			a.pos += deltaTime * a.velocity;

			a.acceleration = {};
		}

		for (int j = 0; j < objects.size(); j++)
		{
			if (i == j) { continue; }

			Object& b = objects[j];

			//sphere vs sphere
			if (a.type == 1 && b.type == 1)
			{

				float aR = a.size.x / 2;
				float bR = b.size.x / 2;

				glm::vec3 normal = b.pos - a.pos;

				float distantaSquared = glm::dot(normal, normal);

				if (distantaSquared < (aR + bR) * (aR + bR))
				{
					normal = glm::normalize(normal);

					float distanta = sqrt(distantaSquared);
					float penetration = (aR + bR) - distanta;

					a.pos -= normal * penetration / 2.f;
					b.pos += normal * penetration / 2.f;

					a.velocity = glm::reflect(a.velocity, normal);
					b.velocity = glm::reflect(b.velocity, -normal);
				}


			}

		}

		//hit walls
		{

			auto minPos = a.getMin();
			auto maxPos = a.getMax();

			if (minPos.y < 0) 
			{
				a.pos += -minPos.y;
				a.velocity.y *= -1;
			}

			if (maxPos.y > glassContainer.y)
			{
				a.pos -= maxPos.y - glassContainer.y;
				a.velocity.y *= -1;
			}

			if (minPos.x < -glassContainer.x / 2) 
			{
				a.pos.x += (-glassContainer.x / 2) - minPos.x;
				a.velocity.x *= -1;
			}

			if (minPos.z < -glassContainer.z / 2)
			{
				a.pos.z += (-glassContainer.z / 2) - minPos.z;
				a.velocity.z *= -1;
			}

			if (maxPos.x > glassContainer.x / 2)
			{
				a.pos.x -= maxPos.x - glassContainer.x / 2;
				a.velocity.x *= -1;
			}

			if (maxPos.z > glassContainer.z / 2)
			{
				a.pos.z -= maxPos.z - glassContainer.z / 2;
				a.velocity.z *= -1;
			}
		}

	}



}
