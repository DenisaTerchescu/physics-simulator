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

//bodies is basically write bodies
restrict layout(std430, binding = 1) buffer writeBuffer
{
	Object writeBodies[];
};

uniform int objectsCount;
uniform float deltaTime;

void main()
{

	uint i = gl_WorkGroupID.x;
	writeBodies[i] = readBodies[i];

	writeBodies[i].pos -= vec3(0, 1 * deltaTime, 0);

}