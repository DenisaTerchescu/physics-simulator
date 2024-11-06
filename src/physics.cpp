#include "physics.h"
#include <iostream>

void PhysicsSimulator::addCube(glm::vec3 pos, glm::vec3 size)
{
	Object o;

	o.pos = pos;
	o.size = size;

	objects.push_back(o);
}

void PhysicsSimulator::addSphere(glm::vec3 pos, float r)
{
	Object o;

	o.type = SPHERE;
	o.pos = pos;
	o.size = {r*2, r*2, r*2};

	objects.push_back(o);
}

void PhysicsSimulator::addCylindre(glm::vec3 pos, float r, float h)
{
	Object o;

	o.type = CYLINDER;
	o.pos = pos;
	o.size = { r * 2, h, r * 2 };

	objects.push_back(o);
}

void PhysicsSimulator::update(float deltaTime)
{

	for (int i = 0; i < objects.size(); i++)
	{
		Object& a = objects[i];

		// forta de frecare cu aerul
		float dragCoefficient = 0.2f;

		{
			a.velocity += deltaTime * a.gravity;
			a.pos += deltaTime * a.velocity;
			a.velocity -= a.velocity * deltaTime * dragCoefficient;

		}

		for (int j = 0; j < objects.size(); j++)
		{
			if (i == j) { continue; }

			Object& b = objects[j];

			if (a.pos == b.pos)
			{
				continue;
			}

			if (a.type == SPHERE && b.type == SPHERE)
			{
				// razele celor 2 sfere
				float r1 = a.size.x / 2;
				float r2 = b.size.x / 2;

				// vectorul dat de centrele sferelor
				glm::vec3 normal = b.pos - a.pos;

				// distanta dintre cele 2 centre
				float distance = glm::length(normal);

				if (distance < abs(r1 + r2))
				{
					normal = glm::normalize(normal);

					float interlappedSection = (r1 + r2) - distance;

					a.pos -= normal * interlappedSection / 2.f;
					b.pos += normal * interlappedSection / 2.f;

					a.velocity = glm::reflect(a.velocity, normal);
					b.velocity = glm::reflect(b.velocity, -normal);
				}

			} 
			else if (a.type == CUBE && b.type == CUBE)
			{

				float halfWidth1 = a.size.x / 2;
				float halfHeight1 = a.size.y / 2;
				float halfDepth1 = a.size.z / 2;

				float halfWidth2 = b.size.x / 2;
				float halfHeight2 = b.size.y / 2;
				float halfDepth2 = b.size.z / 2;

				// vectorul dat de centrele celor 2 cuburi
				glm::vec3 normal = b.pos - a.pos;

				// cat se intersecteaza pe axa X
				float overlapX = abs(halfWidth1) + abs(halfWidth2) - abs(normal.x);

				if (overlapX <= 0) continue; 

				if (normal.x == 0 && normal.y == 0 && normal.z == 0) continue; 

				// cat se intersecteaza pe axa Y
				float overlapY = abs(halfHeight1) + abs(halfHeight2) - abs(normal.y);

				if (overlapY <= 0) continue; 

				// cat se intersecteaza pe axa Z
				float overlapZ = abs(halfDepth1) + abs(halfDepth2) - abs(normal.z);

				if (overlapZ <= 0) continue; 

				float overlappedSection;

				// determinam axa unde cuburile s-au intersectat cel mai putin
				if (overlapX < overlapY && overlapX < overlapZ) {
					overlappedSection = overlapX;
					//normal = glm::normalize(glm::vec3(normal.x, 0, 0));
					normal = glm::vec3(normal.x > 0 ? 1 : -1, 0, 0);
				}
				else if (overlapY < overlapZ) {
					overlappedSection = overlapY;
					//normal = glm::normalize(glm::vec3(0, normal.y, 0));
					normal = glm::vec3(0, normal.y > 0 ? 1 : -1, 0);
				}
				else {
					overlappedSection = overlapZ;
					//normal = glm::normalize(glm::vec3(0, 0, normal.z));
					normal = glm::vec3(0, 0, normal.z > 0 ? 1 : -1);
				}

				a.pos -= normal * (overlappedSection / 2.0f);
				b.pos += normal * (overlappedSection / 2.0f);
				a.velocity = glm::reflect(a.velocity, normal);
				b.velocity = glm::reflect(b.velocity, -normal);

			} 
			else if (a.type == CUBE && b.type == SPHERE)
			{
				float r = b.size.x / 2;

				glm::vec3 closestCubeCorner = b.pos;

				// varfurile minime si maxime ale cubului
				auto boxMin = a.calculateMinimumPoint();
				auto boxMax = a.calculateMaximumPoint();

				// mutam centrul sferei catre cel mai apropiat varf al cubului
				closestCubeCorner = glm::clamp(closestCubeCorner, boxMin, boxMax);

				// bugfix daca obiectele se suprapun total
				if (closestCubeCorner.x == b.pos.x && 
					closestCubeCorner.y == b.pos.y &&
					closestCubeCorner.z == b.pos.z) 
				{
					a.pos -= 0.1;
					b.pos += 0.1;

					a.velocity = glm::reflect(a.velocity, glm::vec3(0,1,0));
					b.velocity = glm::reflect(b.velocity, -glm::vec3(0, 1, 0));

					continue;
				}
				
				// distanta dintre centrele celor 2 obiecte
				float distance = glm::length(closestCubeCorner - b.pos);

				if (distance < r)
				{
					glm::vec3 normal = b.pos - closestCubeCorner;
					normal = glm::normalize(normal);

					float overlappedSection = r - distance;

					a.pos -= normal * overlappedSection / 2.f;
					b.pos += normal * overlappedSection / 2.f;

					a.velocity = glm::reflect(a.velocity, normal);
					b.velocity = glm::reflect(b.velocity, -normal);
				}

			}
			
			else if (a.type == CYLINDER && b.type == CYLINDER)
			{
				float r1 = a.size.x / 2;  
				float r2 = b.size.x / 2;

				float halfHeight1 = a.size.y / 2;
				float halfHeight2 = b.size.y / 2;

				glm::vec3 normal = b.pos - a.pos;

				// se verifica suprapunerea pe verticala
				float verticalOverlap = (halfHeight1 + halfHeight2) - abs(normal.y);
				if (verticalOverlap <= 0) continue; 

				// se verifica suprapunerea pe orizontala
				glm::vec2 normalXZ = glm::vec2(normal.x, normal.z);
				float distXZ = glm::length(normalXZ);
				float horizontalOverlap = r1 + r2 - distXZ;

				if (horizontalOverlap <= 0) continue;

				float overlappedSection;

				// se determina axa unde obiectele intra in coliziune cel mai putin
				if (verticalOverlap < horizontalOverlap) {
					overlappedSection = verticalOverlap;
					//normal = glm::normalize(glm::vec3(0.0f, normal.y, 0.0f));
					normal = glm::normalize(glm::vec3(0.0f, normal.y > 0 ? 1 : -1, 0.0f));
				}
				else {
					overlappedSection = horizontalOverlap;
					normal = glm::normalize(glm::vec3(normalXZ.x > 0 ? 1 : -1, 0.0f, normalXZ.y > 0 ? 1 : -1));
					//normal = glm::normalize(glm::vec3(1, 0.0f, 1));
				}

				a.pos -= normal * (overlappedSection / 2.0f);
				b.pos += normal * (overlappedSection / 2.0f);

				a.velocity = glm::reflect(a.velocity, normal);
				b.velocity = glm::reflect(b.velocity, -normal);
			}
			
			else if (a.type == CYLINDER && b.type == SPHERE)
			{
				float cylinderRadius = a.size.x / 2; 
				float cylinderHalfHeight = a.size.y / 2; 

				float sphereRadius = b.size.x / 2; 

				glm::vec3 normal = b.pos - a.pos;

				float verticalDistance = std::abs(normal.y);
				float verticalOverlap = (cylinderHalfHeight + sphereRadius) - verticalDistance;

				if (verticalOverlap < 0) { continue;}

				// se verifica suprapunerea pe orizontala
				glm::vec2 normalXZ = glm::vec2(normal.x, normal.z);
				float distXZ = glm::length(normalXZ);
				float horizontalOverlap = cylinderRadius + sphereRadius - distXZ;

				if (horizontalOverlap < 0) { continue; }

				float overlappedSection;

				// se determina axa unde obiectele s-au intersectat cel mai putin
				if (verticalOverlap > horizontalOverlap && (normal.x != 0 || normal.z != 0)) {
					overlappedSection = horizontalOverlap;
					if (distXZ > 0) {
						normal = -glm::normalize(glm::vec3(normal.x, 0, normal.z));
						//normal = -glm::normalize(glm::vec3(1, 0.0f, 1));
					}
					else {
						normal = glm::vec3(1.0f, 0.0f, 0.0f); 
					}
				}
				else 
				{
					overlappedSection = verticalOverlap;

					// se verifica daca sfera e deasupra cilindrului
					if (normal.y > 0) {
						normal = glm::vec3(0.0f, -1.0f, 0.0f); 
					}
					else {
						normal = glm::vec3(0.0f, 1.0f, 0.0f);
					}
				}

				b.pos -= normal * (overlappedSection / 2.0f);
				a.pos += normal * (overlappedSection / 2.0f); 

				b.velocity = glm::reflect(b.velocity, -normal);
				a.velocity = glm::reflect(a.velocity, normal);

			}
			
			else if (a.type == CYLINDER && b.type == CUBE) 
			{
				float cylinderRadius = a.size.x / 2;         
				float cylinderHalfHeight = a.size.y / 2;     

				float cubeHalfWidth = b.size.x / 2;           
				float cubeHalfHeight = b.size.y / 2;         
				float cubeHalfDepth = b.size.z / 2;           

				glm::vec3 normal = b.pos - a.pos;

				float verticalOverlap = (cylinderHalfHeight + cubeHalfHeight) - std::abs(normal.y);
				
				if (verticalOverlap <= 0) { continue; }

				float distanceXZ = glm::length(glm::vec2(normal.x, normal.z)); 

				// estimare grosiera a distantei dintre centrele celor 2 obiecte
				float combinedRadius = cylinderRadius + std::min(cubeHalfWidth, cubeHalfDepth)*1.41;
				float horizontalOverlap = combinedRadius - distanceXZ;

				if (horizontalOverlap <= 0) { continue; }

				// bugfix daca obiectele se suprapun total
				if (b.pos.x == a.pos.x &&
					b.pos.y == a.pos.y &&
					b.pos.z == a.pos.z)
				{
					a.pos -= 0.1;
					b.pos += 0.1;

					a.velocity = glm::reflect(a.velocity, glm::vec3(0, 1, 0));
					b.velocity = glm::reflect(b.velocity, -glm::vec3(0, 1, 0));

					continue;
				}

				if (glm::length(normal) < 0.3f)
				{
					a.pos -= 0.1;
					b.pos += 0.1;

					a.velocity = glm::reflect(a.velocity, glm::vec3(0, 1, 0));
					b.velocity = glm::reflect(b.velocity, -glm::vec3(0, 1, 0));

					continue;
				}

				float overlappedSection;

				if (verticalOverlap < horizontalOverlap) {
					overlappedSection = verticalOverlap;
					normal = -glm::normalize(glm::vec3(0, normal.y > 0 ? 1 : -1, 0)); 
				}
				else {
					overlappedSection = horizontalOverlap;
					if (distanceXZ > 0) {
						normal = glm::normalize(glm::vec3(normal.x > 0 ? 1 : -1, 0.0f, normal.z > 0 ? 1 : -1));
					}
					else {
						normal = glm::vec3(1.0f, 0.0f, 0.0f); 
					}
				}
				
				b.pos -= normal * (overlappedSection / 2.0f); 
				a.pos += normal * (overlappedSection / 2.0f);

				a.velocity = glm::reflect(a.velocity, normal);
				b.velocity = glm::reflect(b.velocity, -normal);


			} 
		}

		// coliziunile cu cubul de sticla
		{

			auto minPos = a.calculateMinimumPoint();
			auto maxPos = a.calculateMaximumPoint();

			if (minPos.y < 0) 
			{
				a.pos.y += -minPos.y;
				a.velocity.y *= -0.9f;
			}

			if (maxPos.y > glassContainer.y)
			{
				a.pos.y -= maxPos.y - glassContainer.y;
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
