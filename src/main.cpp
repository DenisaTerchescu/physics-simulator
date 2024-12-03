#include "raylib.h"

#include <iostream>

#include "rlgl.h"

#include "imgui.h"
#include "rlImGui.h"
#include "imguiThemes.h"

#include <physics.h>

PhysicsSimulator engine;

int currentShaderReadsBuffer = 0;
unsigned int buffers[2];
bool renderOnCPU = false;

void createGPUBuffers() {
	buffers[0] = rlLoadShaderBuffer(engine.objects.size() * sizeof(engine.objects[0]), engine.objects.data(), RL_DYNAMIC_COPY);
	buffers[1] = rlLoadShaderBuffer(engine.objects.size() * sizeof(engine.objects[0]), engine.objects.data(), RL_DYNAMIC_COPY);
}

void updateBuffers() {

	rlUnloadShaderBuffer(buffers[0]);
	rlUnloadShaderBuffer(buffers[1]);

	buffers[0] = rlLoadShaderBuffer(engine.objects.size() * sizeof(engine.objects[0]), engine.objects.data(), RL_DYNAMIC_COPY);
	buffers[1] = rlLoadShaderBuffer(engine.objects.size() * sizeof(engine.objects[0]), engine.objects.data(), RL_DYNAMIC_COPY);
}

void readBuffers() {

	rlReadShaderBuffer(buffers[!currentShaderReadsBuffer], engine.objects.data(),
		engine.objects.size() * sizeof(engine.objects[0]), 0);

	currentShaderReadsBuffer = !currentShaderReadsBuffer;
}

int main(void)
{


	for (int i = 0; i < 0; i++)
	{

	   engine.addSphere({ rand() % ((int)engine.glassContainer.x - 5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5,
				rand() % ((int)engine.glassContainer.z - 5) + 5 }, (rand() % 3 + 1) / 3.f);



		engine.addCube({ rand() % ((int)engine.glassContainer.x) - engine.glassContainer.x / 2, rand() % ((int)engine.glassContainer.y) - engine.glassContainer.y / 2, rand() % ((int)engine.glassContainer.z) - engine.glassContainer.z / 2
				},
				{ (rand() % 3 + 1) ,(rand() % 3 + 1) ,(rand() % 3 + 1) });


		engine.addCylindre({ rand() % ((int)engine.glassContainer.x) - engine.glassContainer.x / 2, rand() % ((int)engine.glassContainer.y) - engine.glassContainer.y / 2, rand() % ((int)engine.glassContainer.z) - engine.glassContainer.z / 2 },
			(rand() % 2 + 1) / 3.f, (rand() % 3 + 2) / 3.f);

	}

		//engine.addCube({ 2, 2, 1 }, { 2,2,2 });
		//engine.addCylindre({ 3, 10, 2 }, 2, 3);
		//engine.addCube({ 4, 16, 3 }, { 2,2,2 });
	


	engine.addSphere({ 1, 3, 0 }, 1);
	engine.addSphere({ 0, 6, 0 }, 1);
	engine.addSphere({ 3, 25, 4 }, 1);
	engine.addSphere({ 5, 22, 6 }, 1);
	engine.addSphere({ 29, 45, 8 }, 1);
	engine.addSphere({ 9, 20, 10 }, 1);

	engine.addCube({ 1, 6, 0 }, {2,2,2});
	engine.addCube({ 0, 4, 0 }, { 2,2,2 });
	engine.addCube({ 3, 25, 4 }, { 2,2,2 });
	engine.addCube({ 5, 22, 6 }, { 2,2,2 });
	engine.addCube({ 29, 42, 8 }, { 2,2,2 });
	engine.addCube({ 9, 20, 10 }, { 2,2,2 });

	//engine.addCylindre({ 1, 8, 0 }, 1, 1);
	//engine.addCylindre({ 1, 3, 3 }, 1, 1);
	//engine.addCylindre({ 5, 3, 3 }, 1, 1);
	//engine.addCylindre({ 7, 3, 3 }, 1, 1);
	//engine.addCylindre({ 29, 45, 8 }, 1, 1);
	//engine.addCylindre({ 7, 6, 0 }, 1, 1);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 600, "The Sims | Physics simulator");


	
	unsigned int computeShader = 0;
	{
		char* code = LoadFileText(RESOURCES_PATH "compute.glsl");
		unsigned int shader = rlCompileShader(code, RL_COMPUTE_SHADER);
		computeShader = rlLoadComputeShaderProgram(shader);
		UnloadFileText(code);
	}

	if (computeShader) {
		std::cout << "Compute shader loaded successfully!!\n";
	}
	else {
		std::cout << "Compute shader error!! \n";
	}

	int objectsCountUniform = rlGetLocationUniform(computeShader, "objectsCount");
	int deltaTimeUniform = rlGetLocationUniform(computeShader, "deltaTime");
	int glassContainerUniform = rlGetLocationUniform(computeShader, "glassContainer");

	if (!renderOnCPU)
	{
		createGPUBuffers();
		updateBuffers();
	}

#pragma region imgui
	rlImGuiSetup(true);

	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.FontGlobalScale = 2;

	ImGuiStyle &style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.Colors[ImGuiCol_WindowBg].w = 0.5f;
	}

#pragma endregion

	Camera3D camera = { 0 };
	camera.position = Vector3{ 10.0f, 10.0f, 30.0f }; 
	camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      
	camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          
	camera.fovy = 45.0f;                                
	camera.projection = CAMERA_PERSPECTIVE; 
	int option = 0;

	Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

	DisableCursor();                

	SetTargetFPS(60);

	while (!WindowShouldClose())
	{

#pragma region compute
		if (!renderOnCPU) {
			{
				rlEnableShader(computeShader);
				rlBindShaderBuffer(buffers[currentShaderReadsBuffer], 0);
				rlBindShaderBuffer(buffers[!currentShaderReadsBuffer], 1);

				int count = engine.objects.size();
				float dt = GetFrameTime();
				rlSetUniform(objectsCountUniform, &count, SHADER_UNIFORM_INT, 1);
				rlSetUniform(deltaTimeUniform, &dt, SHADER_UNIFORM_FLOAT, 1);
				rlSetUniform(glassContainerUniform, &engine.glassContainer, SHADER_UNIFORM_VEC3, 1);

				rlComputeShaderDispatch(count, 1, 1);
				rlDisableShader();
			}
		}
#pragma endregion



		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawFPS(10, 10);

	#pragma region imgui
		rlImGuiBegin();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PopStyleColor(2);
	#pragma endregion

		if (renderOnCPU) {
			engine.update(GetFrameTime());
		}
		


		DrawRectangle(GetScreenWidth() - 370, 10, 500, 250, PINK);
		DrawText("PRESS 1 - 100 Spheres, 250 Cubes,", GetScreenWidth() - 370 + 10, 12, 20, WHITE);
		DrawText("500 Cylinders ", GetScreenWidth() - 370 + 10, 42, 20, WHITE);
		DrawText("PRESS 2 - 250 Spheres, 500 Cubes, ", GetScreenWidth() - 370 + 10, 92, 20, WHITE);
		DrawText("1000 Cylinders ", GetScreenWidth() - 370 + 10, 112, 20, WHITE);
		DrawText("PRESS 3 - 500 Spheres, 1000 Cubes, ", GetScreenWidth() - 370 + 10, 162, 20, WHITE);
		DrawText("2500 Cylinders ", GetScreenWidth() - 370 + 10, 192, 20, WHITE);
		DrawText("Left click - insert object ", GetScreenWidth() - 370 + 10, 230, 20, BLACK);


		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		{
			UpdateCamera(&camera, CAMERA_FREE);
			camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			int randOption = rand() % 3;
			if (randOption == 0) {
				engine.addCube({ rand() % 15 - 7, rand() % 10 + 5, rand() % 15 - 7 },
					{ (rand() % 3 + 1) ,(rand() % 3 + 1) ,(rand() % 3 + 1) });
			}
			if (randOption == 1) {
				engine.addSphere({ rand()%15-7, rand()%10 + 5, rand() % 15 - 7},
					(rand()%3 + 1)/3.f);
			}
			if (randOption == 2) {
				engine.addCylindre({ rand() % 15 - 7, rand() % 10 + 5, rand() % 15 - 7 },
					(rand() % 2 + 1) / 3.f, (rand() % 3 + 2) / 3.f);
			}
			updateBuffers();
		}

		if (IsKeyPressed('Z')) camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
		if (IsKeyPressed('1')) option = 1;
		if (IsKeyPressed('2')) option = 2;
		if (IsKeyPressed('3')) option = 3;

		if (IsKeyPressed(KEY_ENTER)) renderOnCPU = !renderOnCPU;

		if (IsKeyPressed('R')) {
			for (Object& o : engine.objects) {
				o.pos = { rand() % ((int)engine.glassContainer.x - 5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5, 
					 rand() % ((int)engine.glassContainer.z - 5) - 7 };
			}
			updateBuffers();
		}

		if (option == 1) {
			engine.glassContainer = { 70,70,70 };
			engine.objects.clear();

			for (int i = 0; i < 500; i++)
			{
				if (i < 100) {
				engine.addSphere({ rand()% ((int)engine.glassContainer.x) - engine.glassContainer.x / 2, rand()% ((int)engine.glassContainer.y) - engine.glassContainer.y / 2,
					rand() % ((int)engine.glassContainer.z) - engine.glassContainer.z / 2 }, (rand()%3 + 1)/3.f);
				}
				
				if (i < 250) {
					engine.addCube({rand() % ((int)engine.glassContainer.x) - engine.glassContainer.x / 2, rand() % ((int)engine.glassContainer.y) - engine.glassContainer.y / 2, rand() % ((int)engine.glassContainer.z) - engine.glassContainer.z / 2
				},
						{ (rand() % 3 + 1) ,(rand() % 3 + 1) ,(rand() % 3 + 1) });
				}

				engine.addCylindre({ rand() % ((int)engine.glassContainer.x) - engine.glassContainer.x/2, rand() % ((int)engine.glassContainer.y) - engine.glassContainer.y / 2, rand() % ((int)engine.glassContainer.z) - engine.glassContainer.z / 2 },
					(rand() % 2 + 1) / 3.f, (rand() % 3 + 2) / 3.f);

			}
			option = 0;
		}
		if (option == 2) {
			engine.glassContainer = { 200,200,200 };
			engine.objects.clear();

			for (int i = 0; i < 1000; i++)
			{
				if (i < 250) {
					engine.addSphere({ rand() % ((int)engine.glassContainer.x) - engine.glassContainer.x / 2, rand() % ((int)engine.glassContainer.y) - engine.glassContainer.y / 2,
						rand() % ((int)engine.glassContainer.z) - engine.glassContainer.z / 2 }, (rand() % 3 + 1) / 3.f);
				}

				if (i < 500) {
					engine.addCube({ rand() % ((int)engine.glassContainer.x) - engine.glassContainer.x / 2, rand() % ((int)engine.glassContainer.y) - engine.glassContainer.y / 2, rand() % ((int)engine.glassContainer.z) - engine.glassContainer.z / 2
						},
						{ (rand() % 3 + 1) ,(rand() % 3 + 1) ,(rand() % 3 + 1) });
				}

				engine.addCylindre({ rand() % ((int)engine.glassContainer.x) - engine.glassContainer.x / 2, rand() % ((int)engine.glassContainer.y) - engine.glassContainer.y / 2, rand() % ((int)engine.glassContainer.z) - engine.glassContainer.z / 2 },
					(rand() % 2 + 1) / 3.f, (rand() % 3 + 2) / 3.f);

			}
			option = 0;
		}
		if (option == 3) {
			engine.glassContainer = { 300,300,300 };
			engine.objects.clear();

			for (int i = 0; i < 2500; i++)
			{
				if (i < 500) {
					engine.addSphere({ rand() % ((int)engine.glassContainer.x) - engine.glassContainer.x / 2, rand() % ((int)engine.glassContainer.y) - engine.glassContainer.y / 2,
						rand() % ((int)engine.glassContainer.z) - engine.glassContainer.z / 2 }, (rand() % 3 + 1) / 3.f);
				}

				if (i < 1000) {
					engine.addCube({ rand() % ((int)engine.glassContainer.x) - engine.glassContainer.x / 2, rand() % ((int)engine.glassContainer.y) - engine.glassContainer.y / 2, rand() % ((int)engine.glassContainer.z) - engine.glassContainer.z / 2
						},
						{ (rand() % 3 + 1) ,(rand() % 3 + 1) ,(rand() % 3 + 1) });
				}

				engine.addCylindre({ rand() % ((int)engine.glassContainer.x) - engine.glassContainer.x / 2, rand() % ((int)engine.glassContainer.y) - engine.glassContainer.y / 2, rand() % ((int)engine.glassContainer.z) - engine.glassContainer.z / 2 },
					(rand() % 2 + 1) / 3.f, (rand() % 3 + 2) / 3.f);

			}
			option = 0;
		}
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------

		ClearBackground(RAYWHITE);

		BeginMode3D(camera);

		{

			for (auto& o : engine.objects) 
			{
				if (o.type == CUBE) 
				{

				DrawCubeWires(
						{ o.pos.x, o.pos.y, o.pos.z },
						o.size.x, o.size.y, o.size.z,
						{ 0,155,155,255 });

				DrawCube(  
					{o.pos.x, o.pos.y, o.pos.z},
					o.size.x,o.size.y,o.size.z ,
					{ 100,255,255,255 });
				}
				else if (o.type == SPHERE) 
				{

					DrawSphereWires({ o.pos.x, o.pos.y, o.pos.z }, o.size.x / 2.f, 10, 10,
						{ 255,0,0,255 });

					DrawSphere({ o.pos.x, o.pos.y, o.pos.z }, o.size.x / 2.f,
						{ 255,155,100,255 });

				}
				else if (o.type == CYLINDER)
				{
					
					DrawCylinderWires({ o.pos.x, o.pos.y - o.size.y/2.f, o.pos.z },
						o.size.x / 2.f, o.size.x / 2.f, o.size.y, 10,
						{ 20,10,20,255 });

					DrawCylinder({ o.pos.x, o.pos.y - o.size.y / 2.f, o.pos.z },
						o.size.x / 2.f, o.size.x / 2.f, o.size.y, 10,
						{ 250,100,250,255 });

				}

			}

		}

		DrawGrid(engine.glassContainer.x, 1.0f);

		DrawCube({0,engine.glassContainer.y/2.f,0}, engine.glassContainer.x,
			engine.glassContainer.y, engine.glassContainer.z, { 155,100,100,20 });
		DrawCubeWires({ 0,engine.glassContainer.y / 2.f,0 }, engine.glassContainer.x,
			engine.glassContainer.y, engine.glassContainer.z, PINK);


		EndMode3D();

		rlImGuiEnd();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		EndDrawing();

		if(!renderOnCPU)
		{
			readBuffers();
		}
	}

	rlImGuiShutdown();

	CloseWindow();

	return 0;
}