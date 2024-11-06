#include "physics.h"

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
		float dragCoefficient = 0.9f;

		{
			a.velocity += deltaTime * a.gravity * dragCoefficient;
			a.pos += deltaTime * a.velocity;

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
				float overlapX = halfWidth1 + halfWidth2 - abs(normal.x);

				if (overlapX <= 0) continue; 

				// cat se intersecteaza pe axa Y
				float overlapY = halfHeight1 + halfHeight2 - abs(normal.y);
				if (overlapY <= 0) continue; 

				// cat se intersecteaza pe axa Z
				float overlapZ = halfDepth1 + halfDepth2 - abs(normal.z);

				if (overlapZ <= 0) continue; 

				float overlappedSection;

				// determinam axa unde cuburile s-au intersectat cel mai putin
				if (overlapX < overlapY && overlapX < overlapZ) {
					overlappedSection = overlapX;
					normal = glm::normalize(glm::vec3(normal.x, 0, 0));
				}
				else if (overlapY < overlapZ) {
					overlappedSection = overlapY;
					normal = glm::normalize(glm::vec3(0, normal.y, 0));
				}
				else {
					overlappedSection = overlapZ;
					normal = glm::normalize(glm::vec3(0, 0, normal.z));
				}

				a.pos -= normal * (overlappedSection / 2.0f);
				b.pos += normal * (overlappedSection / 2.0f);

				a.velocity = glm::reflect(a.velocity, normal);
				b.velocity = glm::reflect(b.velocity, -normal);

			}
			else if (a.type == CUBE && b.type == SPHERE)
			{
				float r = b.size.x / 2;

				glm::vec3 centerSphere = b.pos;

				// varfurile minime si maxime ale cubului
				auto boxMin = a.calculateMinimumPoint();
				auto boxMax = a.calculateMaximumPoint();

				// mutam centrul sferei catre cel mai apropiat punct al cubului
				centerSphere = glm::clamp(centerSphere, boxMin, boxMax);

				// bugfix daca obiectele se suprapun total
				if (centerSphere.x == b.pos.x && 
					centerSphere.y == b.pos.y &&
					centerSphere.z == b.pos.z) 
				{
					a.pos -= 10;
					b.pos += 10;

					a.velocity = glm::reflect(a.velocity, glm::vec3(0,1,0));
					b.velocity = glm::reflect(b.velocity, -glm::vec3(0, 1, 0));
				}
				
				// distanta dintre centrele celor 2 obiecte
				float distance = glm::length(centerSphere - b.pos);

				if (distance < r)
				{
					glm::vec3 normal = b.pos - centerSphere;
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
					normal = glm::normalize(glm::vec3(0.0f, normal.y, 0.0f));
				}
				else {
					overlappedSection = horizontalOverlap;
					normal = glm::normalize(glm::vec3(normalXZ.x, 0.0f, normalXZ.y));
				}

				a.pos -= normal * (overlappedSection / 2.0f);
				b.pos += normal * (overlappedSection / 2.0f);

				a.velocity = glm::reflect(a.velocity, normal);
				b.velocity = glm::reflect(b.velocity, -normal);
			}

/////////////////////////////////
			else if (a.type == CYLINDER && b.type == SPHERE)
			{
				float cylinderRadius = a.size.x / 2;  // Assuming size.x is the diameter of the cylinder
				float cylinderHalfHeight = a.size.y / 2; // Assuming size.y is the total height of the cylinder

				float sphereRadius = b.size.x / 2; // Assuming size.x is the diameter of the sphere

				// Vector from the center of the cylinder to the center of the sphere
				glm::vec3 delta = b.pos - a.pos;

				// Check if the sphere's y position is within the cylinder's height
				if (std::abs(delta.y) > cylinderHalfHeight + sphereRadius) {
					continue; // No collision if the sphere is above or below the cylinder
				}

				// Calculate the horizontal distance in the x-z plane
				float distXZSquared = delta.x * delta.x + delta.z * delta.z;
				float combinedRadius = cylinderRadius + sphereRadius;

				// Check for collision in the x-z plane
				if (distXZSquared >= combinedRadius * combinedRadius) {
					continue; // No collision in the x-z plane
				}

				float verticalDistance = std::abs(delta.y);
				float verticalOverlap = (cylinderHalfHeight + sphereRadius) - verticalDistance;
				float horizontalOverlap = combinedRadius - sqrt(distXZSquared);

				// Collision detected, determine the collision normal
				glm::vec3 collisionNormal;

				// Calculate penetration depth in the x-z plane
				float distXZ = sqrt(distXZSquared);
				float penetration = combinedRadius - distXZ;

				if (verticalOverlap > horizontalOverlap) {
					penetration = horizontalOverlap;
					if (distXZSquared > 0) {
						// Normal in the x-z plane
						collisionNormal = -glm::normalize(glm::vec3(delta.x, 0.0f, delta.z)); // Normal vector in x-z plane
					}
					else {
						// Arbitrary normal if sphere is exactly at the cylinder axis (center)
						collisionNormal = glm::vec3(1.0f, 0.0f, 0.0f); // Can be any direction
					}
				}
				else 
				{
					// If the sphere is exactly at the cylinder axis (center)
					if (delta.y > 0) {
						penetration = std::abs(delta.y) - sphereRadius - cylinderHalfHeight;
						collisionNormal = glm::vec3(0.0f, -1.0f, 0.0f); 
					}
					else {
						penetration = -(std::abs(delta.y) - sphereRadius - cylinderHalfHeight);
						collisionNormal = glm::vec3(0.0f, 1.0f, 0.0f);
					}
				}

				b.pos -= collisionNormal * (penetration / 2.0f); // Move the sphere out
				a.pos += collisionNormal * (penetration / 2.0f); // Optional: Adjust the cylinder slightly (can be omitted)

				b.velocity = glm::reflect(b.velocity, -collisionNormal);
				a.velocity = glm::reflect(a.velocity, collisionNormal);


			}

			//cylinder cube
			else if (a.type == CYLINDER && b.type == CUBE) 
			{
				float cylinderRadius = a.size.x / 2;          // Assuming size.x is the diameter of the cylinder
				float cylinderHalfHeight = a.size.y / 2;      // Assuming size.y is the total height of the cylinder

				float cubeHalfWidth = b.size.x / 2;           // Assuming size.x is the width of the cube
				float cubeHalfHeight = b.size.y / 2;          // Assuming size.y is the height of the cube
				float cubeHalfDepth = b.size.z / 2;           // Assuming size.z is the depth of the cube

				// Vector from the center of the cylinder to the center of the cube
				glm::vec3 delta = b.pos - a.pos;

				// Check vertical overlap
				float verticalDistance = std::abs(delta.y);
				float verticalOverlap = (cylinderHalfHeight + cubeHalfHeight) - verticalDistance;

				// Check horizontal (x-z plane) distance
				float closestX = glm::clamp(delta.x, -cubeHalfWidth, cubeHalfWidth);
				float closestZ = glm::clamp(delta.z, -cubeHalfDepth, cubeHalfDepth);
				float distanceXZSquared = (closestX * closestX) + (closestZ * closestZ);
				float combinedRadius = cylinderRadius;
				float horizontalOverlap = combinedRadius - sqrt(distanceXZSquared);

				if (distanceXZSquared >= combinedRadius * combinedRadius) {
					continue; // No collision in the x-z plane
				}

				// If there's no overlap in the vertical direction, there is no collision
				if (verticalOverlap <= 0) {
					continue; // No collision
				}

				// Collision detected, determine the axis of least penetration
				glm::vec3 collisionNormal;
				float penetration;

				// Determine the penetration depth
				if (verticalOverlap < horizontalOverlap) {
					// Vertical overlap is greater
					penetration = -verticalOverlap;
					collisionNormal = -glm::vec3(0.0f, (delta.y > 0 ? -1.0f : 1.0f), 0.0f); // Normal is up or down
				}
				else {
					// Horizontal overlap is greater
					penetration = combinedRadius - std::sqrt(distanceXZSquared);
					if (distanceXZSquared > 0) {
						// Normal in the x-z plane
						collisionNormal = glm::normalize(glm::vec3(delta.x, 0.0f, delta.z)); // Normal vector in x-z plane
					}
					else {
						// Arbitrary normal if the cylinder is exactly at the cube's axis
						collisionNormal = glm::vec3(1.0f, 0.0f, 0.0f); // Can be any direction
					}
				}

				// Separate the cylinder and cube along the collision normal by half the penetration depth
				b.pos -= collisionNormal * (penetration / 2.0f); // Move the cube out
				a.pos += collisionNormal * (penetration / 2.0f); // Optional adjustment for the cylinder

				// Reflect the velocities along the collision normal
				a.velocity = glm::reflect(a.velocity, collisionNormal);
				b.velocity = glm::reflect(b.velocity, -collisionNormal);


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
