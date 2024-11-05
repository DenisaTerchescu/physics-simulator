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

		a.acceleration += glm::vec3(1, -9.81, 1);

		{
			a.velocity += deltaTime * a.acceleration;
			a.pos += deltaTime * a.velocity;

			a.acceleration = {};
		}

		for (int j = 0; j < objects.size(); j++)
		{
			if (i == j) { continue; }

			Object& b = objects[j];

			if (a.pos == b.pos)
			{
				continue;
			}

			//sphere vs sphere
			if (a.type == SPHERE && b.type == SPHERE)
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
			else if (a.type == CUBE && b.type == CUBE)
			{
				float aHalfWidth = a.size.x / 2;
				float aHalfHeight = a.size.y / 2;
				float aHalfDepth = a.size.z / 2;

				float bHalfWidth = b.size.x / 2;
				float bHalfHeight = b.size.y / 2;
				float bHalfDepth = b.size.z / 2;

				// Vector from box A center to box B center
				glm::vec3 normal = b.pos - a.pos;

				// Calculate overlap on the x-axis
				float overlapX = aHalfWidth + bHalfWidth - abs(normal.x);
				if (overlapX <= 0) continue; // No collision on the x-axis

				// Calculate overlap on the y-axis
				float overlapY = aHalfHeight + bHalfHeight - abs(normal.y);
				if (overlapY <= 0) continue; // No collision on the y-axis

				// Calculate overlap on the z-axis
				float overlapZ = aHalfDepth + bHalfDepth - abs(normal.z);
				if (overlapZ <= 0) continue; // No collision on the z-axis

				// Determine the axis of least penetration
				float penetration;
				glm::vec3 collisionNormal;

				if (overlapX < overlapY && overlapX < overlapZ) {
					penetration = overlapX;
					collisionNormal = glm::vec3(normal.x < 0 ? -1.0f : 1.0f, 0.0f, 0.0f);
				}
				else if (overlapY < overlapZ) {
					penetration = overlapY;
					collisionNormal = glm::vec3(0.0f, normal.y < 0 ? -1.0f : 1.0f, 0.0f);
				}
				else {
					penetration = overlapZ;
					collisionNormal = glm::vec3(0.0f, 0.0f, normal.z < 0 ? -1.0f : 1.0f);
				}

				// Separate the boxes along the collision normal by half the penetration depth
				a.pos -= collisionNormal * (penetration / 2.0f);
				b.pos += collisionNormal * (penetration / 2.0f);

				// Reflect the velocities along the collision normal
				a.velocity = glm::reflect(a.velocity, collisionNormal);
				b.velocity = glm::reflect(b.velocity, -collisionNormal);

			}
			else if (a.type == CUBE && b.type == SPHERE)
			{
				//b is sphere
				float bR = b.size.x / 2;

				glm::vec3 corner = b.pos;

				auto boxMin = a.getMin();
				auto boxMax = a.getMax();

				corner = glm::clamp(corner, boxMin, boxMax);

				if(corner.x == b.pos.x && 
					corner.y == b.pos.y &&
					corner.z == b.pos.z) 
				{
					continue;
				}
				
				float distanceSquared = glm::dot(corner - b.pos, corner - b.pos);

				if (distanceSquared < bR * bR)
				{
					glm::vec3 normal = b.pos - corner;
					normal = glm::normalize(normal);

					float distanta = sqrt(distanceSquared);
					float penetration = (bR) - distanta;

					a.pos -= normal * penetration / 2.f;
					b.pos += normal * penetration / 2.f;

					a.velocity = glm::reflect(a.velocity, normal);
					b.velocity = glm::reflect(b.velocity, -normal);
				}

			}

			//cylinder
			else if (a.type == CYLINDER && b.type == CYLINDER)
			{
				float aRadius = a.size.x / 2;  // Assuming size.x is the diameter of the cylinder
				float bRadius = b.size.x / 2;

				float aHalfHeight = a.size.y / 2;
				float bHalfHeight = b.size.y / 2;

				// Vector from center of cylinder A to center of cylinder B
				glm::vec3 normal = b.pos - a.pos;

				// Check y-axis overlap
				float verticalOverlap = (aHalfHeight + bHalfHeight) - abs(normal.y);
				if (verticalOverlap <= 0) continue; // No collision in the y-axis

				// Check horizontal (x-z plane) overlap as if they're circles
				glm::vec2 normalXZ = glm::vec2(normal.x, normal.z);
				float distXZSquared = glm::dot(normalXZ, normalXZ);
				float combinedRadius = aRadius + bRadius;

				if (distXZSquared >= combinedRadius * combinedRadius) continue; // No collision in the x-z plane

				// Collision detected, determine the axis of least penetration
				glm::vec3 collisionNormal;
				float penetration;

				if (verticalOverlap < combinedRadius - sqrt(distXZSquared)) {
					// y-axis is the axis of least penetration
					penetration = verticalOverlap;
					collisionNormal = glm::vec3(0.0f, (normal.y < 0 ? -1.0f : 1.0f), 0.0f);
				}
				else {
					// x-z plane (horizontal) is the axis of least penetration
					float distXZ = sqrt(distXZSquared);
					penetration = combinedRadius - distXZ;
					collisionNormal = glm::normalize(glm::vec3(normalXZ.x, 0.0f, normalXZ.y)); // 3D vector in x-z plane
				}

				// Separate the cylinders along the collision normal by half the penetration depth
				a.pos -= collisionNormal * (penetration / 2.0f);
				b.pos += collisionNormal * (penetration / 2.0f);

				// Reflect the velocities along the collision normal
				a.velocity = glm::reflect(a.velocity, collisionNormal);
				b.velocity = glm::reflect(b.velocity, -collisionNormal);
			}

			//cylinder sphere
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

		// the collisions with the glass container
		{

			auto minPos = a.getMin();
			auto maxPos = a.getMax();

			if (minPos.y < 0) 
			{
				a.pos.y += -minPos.y;
				a.velocity.y *= -1;
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
