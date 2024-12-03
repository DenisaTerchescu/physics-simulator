#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


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

readonly restrict layout(std430, binding = 0) buffer readBuffer
{
	Object readBodies[];
};

restrict layout(std430, binding = 1) buffer writeBuffer
{
	Object writeBodies[];
};

uniform int objectsCount;
uniform float deltaTime;
uniform vec3 glassContainer;

vec3 calculateMinimumPoint(inout Object b)
{
	return b.pos - b.size / 2.f;
}

vec3 calculateMaximumPoint(inout Object b)
{
	return b.pos + b.size / 2.f;
}

void main()
{

	uint i = gl_WorkGroupID.x;
	writeBodies[i] = readBodies[i];

	//writeBodies[i].pos -= vec3(0, 1 * deltaTime, 0);


	for (int i = 0; i < objectsCount; i++)
	{

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
		}

		vec3 minPos = calculateMinimumPoint(writeBodies[i]);
		vec3 maxPos = calculateMaximumPoint(writeBodies[i]);

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

}