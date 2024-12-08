#version 430 core

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

struct Object
{
	vec3 pos;
	float padding;

	vec3 size;
	float padding2;

	vec3 velocity;
	float padding3;

	vec3 gravity;
	int type;
};

readonly layout(std430, binding = 0) buffer readBuffer
{
	Object readBodies[];
};

layout(std430, binding = 1) buffer writeBuffer
{
	Object writeBodies[];
};

uniform int objectsCount;
uniform float deltaTime;
uniform vec3 glassContainer;

vec3 calculateMinimumPoint(inout Object o)
{
	return o.pos - o.size / 2.f;
}

vec3 calculateMaximumPoint(inout Object o)
{
	return o.pos + o.size / 2.f;
}

void main()
{

	// numarul total de tread-uri la nivel de workgroup
	uint noThreads = gl_WorkGroupSize.x;

	// index unic global pentru fiecare thread in parte
	uint threadGlobalIndex = gl_WorkGroupID.x * noThreads + gl_LocalInvocationID.x;
	uint i = threadGlobalIndex;
	writeBodies[i] = readBodies[i];

		// forta de frecare cu aerul
		float dragCoefficient = 0.2f;

		{
			writeBodies[i].velocity += deltaTime * writeBodies[i].gravity;
			writeBodies[i].pos += deltaTime * writeBodies[i].velocity;
			writeBodies[i].velocity -= writeBodies[i].velocity * deltaTime * dragCoefficient;
		}

		for (int j = 0; j < objectsCount; j++)
		{
			if (i == j) { continue; }

			if (writeBodies[i].pos == writeBodies[j].pos)
			{
				continue;
			}
			

			// coliziunea sfera - sfera
			if (writeBodies[i].type == 0 && writeBodies[j].type == 0)
			{
				// razele celor 2 sfere
				float r1 = writeBodies[i].size.x / 2;
				float r2 = writeBodies[j].size.x / 2;

				// vectorul dat de centrele sferelor
				vec3 normal = writeBodies[j].pos - writeBodies[i].pos;

				// distanta dintre cele 2 centre
				float distance = length(normal);

				if (distance < abs(r1 + r2))
				{
					normal = normalize(normal);

					float interlappedSection = (r1 + r2) - distance;

					writeBodies[i].pos -= normal * interlappedSection / 2.f;
					//writeBodies[j].pos += normal * interlappedSection / 2.f;

					writeBodies[i].velocity = reflect(writeBodies[i].velocity, normal);
					//writeBodies[j].velocity = reflect(writeBodies[j].velocity, -normal);
				}

			}
			// coliziunea cub - cub
			else if (writeBodies[i].type == 1 && writeBodies[j].type == 1)
			{

				float halfWidth1 = writeBodies[i].size.x / 2;
				float halfHeight1 = writeBodies[i].size.y / 2;
				float halfDepth1 = writeBodies[i].size.z / 2;

				float halfWidth2 = writeBodies[j].size.x / 2;
				float halfHeight2 = writeBodies[j].size.y / 2;
				float halfDepth2 = writeBodies[j].size.z / 2;

				// vectorul dat de centrele celor 2 cuburi
				vec3 normal = writeBodies[j].pos - writeBodies[i].pos;

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
					//normal = normalize(vec3(normal.x, 0, 0));
					normal = vec3(normal.x > 0 ? 1 : -1, 0, 0);
				}
				else if (overlapY < overlapZ) {
					overlappedSection = overlapY;
					//normal = normalize(vec3(0, normal.y, 0));
					normal = vec3(0, normal.y > 0 ? 1 : -1, 0);
				}
				else {
					overlappedSection = overlapZ;
					//normal = normalize(vec3(0, 0, normal.z));
					normal = vec3(0, 0, normal.z > 0 ? 1 : -1);
				}

				writeBodies[i].pos -= normal * (overlappedSection / 2.0f);
				//writeBodies[j].pos += normal * (overlappedSection / 2.0f);
				writeBodies[i].velocity = reflect(writeBodies[i].velocity, normal);
				//writeBodies[j].velocity = reflect(writeBodies[j].velocity, -normal);

			}

			// coliziunea cub - sfera
			else if (writeBodies[i].type == 1 && writeBodies[j].type == 0)
			{
				float r = writeBodies[j].size.x / 2;

				vec3 closestCubeCorner = writeBodies[j].pos;

				// varfurile minime si maxime ale cubului
				vec3 boxMin = calculateMinimumPoint(writeBodies[i]);
				vec3 boxMax = calculateMaximumPoint(writeBodies[i]);

				// mutam centrul sferei catre cel mai apropiat varf al cubului
				closestCubeCorner = clamp(closestCubeCorner, boxMin, boxMax);

				// bugfix daca obiectele se suprapun total
				if (closestCubeCorner.x == writeBodies[j].pos.x &&
					closestCubeCorner.y == writeBodies[j].pos.y &&
					closestCubeCorner.z == writeBodies[j].pos.z)
				{
					writeBodies[i].pos -= 0.1;
					//writeBodies[j].pos += 0.1;

					writeBodies[i].velocity = reflect(writeBodies[i].velocity, vec3(0, 1, 0));
					//writeBodies[j].velocity = reflect(writeBodies[j].velocity, -vec3(0, 1, 0));

					continue;
				}

				// distanta dintre centrele celor 2 obiecte
				float distance = length(closestCubeCorner - writeBodies[j].pos);

				if (distance < r)
				{
					vec3 normal = writeBodies[j].pos - closestCubeCorner;
					normal = normalize(normal);

					float overlappedSection = r - distance;

					writeBodies[i].pos -= normal * overlappedSection / 2.f;
					//writeBodies[j].pos += normal * overlappedSection / 2.f;

					writeBodies[i].velocity = reflect(writeBodies[i].velocity, normal);
					//writeBodies[j].velocity = reflect(writeBodies[j].velocity, -normal);
				}

			}
			// coliziunea cilindru - cilindru
			else if (writeBodies[i].type == 2 && writeBodies[j].type == 2)
			{
				float r1 = writeBodies[i].size.x / 2;
				float r2 = writeBodies[j].size.x / 2;

				float halfHeight1 = writeBodies[i].size.y / 2;
				float halfHeight2 = writeBodies[j].size.y / 2;

				vec3 normal = writeBodies[j].pos - writeBodies[i].pos;

				// se verifica suprapunerea pe verticala
				float verticalOverlap = (halfHeight1 + halfHeight2) - abs(normal.y);
				if (verticalOverlap <= 0) continue;

				// se verifica suprapunerea pe orizontala
				vec2 normalXZ = vec2(normal.x, normal.z);
				float distXZ = length(normalXZ);
				float horizontalOverlap = r1 + r2 - distXZ;

				if (horizontalOverlap <= 0) continue;

				float overlappedSection;

				// se determina axa unde obiectele intra in coliziune cel mai putin
				if (verticalOverlap < horizontalOverlap) {
					overlappedSection = verticalOverlap;
					//normal = normalize(vec3(0.0f, normal.y, 0.0f));
					normal = normalize(vec3(0.0f, normal.y > 0 ? 1 : -1, 0.0f));
				}
				else {
					overlappedSection = horizontalOverlap;
					normal = normalize(vec3(normalXZ.x > 0 ? 1 : -1, 0.0f, normalXZ.y > 0 ? 1 : -1));
					//normal = normalize(vec3(1, 0.0f, 1));
				}

				writeBodies[i].pos -= normal * (overlappedSection / 2.0f);
				//writeBodies[j].pos += normal * (overlappedSection / 2.0f);

				writeBodies[i].velocity = reflect(writeBodies[i].velocity, normal);
				//writeBodies[j].velocity = reflect(writeBodies[j].velocity, -normal);
			}
			// coliziunea cilindru - sfera
			else if (writeBodies[i].type == 2 && writeBodies[j].type == 0)
			{
				float cylinderRadius = writeBodies[i].size.x / 2;
				float cylinderHalfHeight = writeBodies[i].size.y / 2;

				float sphereRadius = writeBodies[j].size.x / 2;

				vec3 normal = writeBodies[j].pos - writeBodies[i].pos;

				float verticalDistance = abs(normal.y);
				float verticalOverlap = (cylinderHalfHeight + sphereRadius) - verticalDistance;

				if (verticalOverlap < 0) { continue; }

				// se verifica suprapunerea pe orizontala
				vec2 normalXZ = vec2(normal.x, normal.z);
				float distXZ = length(normalXZ);
				float horizontalOverlap = cylinderRadius + sphereRadius - distXZ;

				if (horizontalOverlap < 0) { continue; }

				float overlappedSection;

				// se determina axa unde obiectele s-au intersectat cel mai putin
				if (verticalOverlap > horizontalOverlap && (normal.x != 0 || normal.z != 0)) {
					overlappedSection = horizontalOverlap;
					if (distXZ > 0) {
						normal = -normalize(vec3(normal.x, 0, normal.z));
						//normal = -normalize(vec3(1, 0.0f, 1));
					}
					else {
						normal = vec3(1.0f, 0.0f, 0.0f);
					}
				}
				else
				{
					overlappedSection = verticalOverlap;

					// se verifica daca sfera e deasupra cilindrului
					if (normal.y > 0) {
						normal = vec3(0.0f, -1.0f, 0.0f);
					}
					else {
						normal = vec3(0.0f, 1.0f, 0.0f);
					}
				}

				//writeBodies[j].pos -= normal * (overlappedSection / 2.0f);
				writeBodies[i].pos += normal * (overlappedSection / 2.0f);

				//writeBodies[j].velocity = reflect(writeBodies[j].velocity, -normal);
				writeBodies[i].velocity = reflect(writeBodies[i].velocity, normal);

			}
			// coliziunea cilindru - cub
			else if (writeBodies[i].type == 2 && writeBodies[j].type == 1)
			{
				float cylinderRadius = writeBodies[i].size.x / 2;
				float cylinderHalfHeight = writeBodies[i].size.y / 2;

				float cubeHalfWidth = writeBodies[j].size.x / 2;
				float cubeHalfHeight = writeBodies[j].size.y / 2;
				float cubeHalfDepth = writeBodies[j].size.z / 2;

				vec3 normal = writeBodies[j].pos - writeBodies[i].pos;

				float verticalOverlap = (cylinderHalfHeight + cubeHalfHeight) - abs(normal.y);

				if (verticalOverlap <= 0) { continue; }

				float distanceXZ = length(vec2(normal.x, normal.z));

				// estimare grosiera a distantei dintre centrele celor 2 obiecte
				float combinedRadius = cylinderRadius + min(cubeHalfWidth, cubeHalfDepth) * 1.41;
				float horizontalOverlap = combinedRadius - distanceXZ;

				if (horizontalOverlap <= 0) { continue; }

				// bugfix daca obiectele se suprapun total
				if (writeBodies[j].pos.x == writeBodies[i].pos.x &&
					writeBodies[j].pos.y == writeBodies[i].pos.y &&
					writeBodies[j].pos.z == writeBodies[i].pos.z)
				{
					writeBodies[i].pos -= 0.1;
					writeBodies[j].pos += 0.1;

					writeBodies[i].velocity = reflect(writeBodies[i].velocity, vec3(0, 1, 0));
					writeBodies[j].velocity = reflect(writeBodies[j].velocity, -vec3(0, 1, 0));

					continue;
				}

				if (length(normal) < 0.3f)
				{
					writeBodies[i].pos -= 0.1;
					//writeBodies[j].pos += 0.1;

					writeBodies[i].velocity = reflect(writeBodies[i].velocity, vec3(0, 1, 0));
					//writeBodies[j].velocity = reflect(writeBodies[j].velocity, -vec3(0, 1, 0));

					continue;
				}

				float overlappedSection;

				if (verticalOverlap < horizontalOverlap) {
					overlappedSection = verticalOverlap;
					normal = -normalize(vec3(0, normal.y > 0 ? 1 : -1, 0));
				}
				else {
					overlappedSection = horizontalOverlap;
					if (distanceXZ > 0) {
						normal = normalize(vec3(normal.x > 0 ? 1 : -1, 0.0f, normal.z > 0 ? 1 : -1));
					}
					else {
						normal = vec3(1.0f, 0.0f, 0.0f);
					}
				}

				writeBodies[j].pos -= normal * (overlappedSection / 2.0f);
				//writeBodies[i].pos += normal * (overlappedSection / 2.0f);

				writeBodies[i].velocity = reflect(writeBodies[i].velocity, normal);
				//writeBodies[j].velocity = reflect(writeBodies[j].velocity, -normal);


				}


		}

		vec3 minPos = calculateMinimumPoint(writeBodies[i]);
		vec3 maxPos = calculateMaximumPoint(writeBodies[i]);

		// coliziunea cu peretii containerului de sticla
		if (minPos.y < 0)
		{
			writeBodies[i].pos.y += -minPos.y;
			writeBodies[i].velocity.y *= -0.9f;
		}

		if (maxPos.y > glassContainer.y)
		{
			writeBodies[i].pos.y -= maxPos.y - glassContainer.y;
			writeBodies[i].velocity.y *= -1;
		}

		if (minPos.x < -glassContainer.x / 2)
		{
			writeBodies[i].pos.x += (-glassContainer.x / 2) - minPos.x;
			writeBodies[i].velocity.x *= -1;
		}

		if (minPos.z < -glassContainer.z / 2)
		{
			writeBodies[i].pos.z += (-glassContainer.z / 2) - minPos.z;
			writeBodies[i].velocity.z *= -1;
		}

		if (maxPos.x > glassContainer.x / 2)
		{
			writeBodies[i].pos.x -= maxPos.x - glassContainer.x / 2;
			writeBodies[i].velocity.x *= -1;
		}

		if (maxPos.z > glassContainer.z / 2)
		{
			writeBodies[i].pos.z -= maxPos.z - glassContainer.z / 2;
			writeBodies[i].velocity.z *= -1;
		}

	

}