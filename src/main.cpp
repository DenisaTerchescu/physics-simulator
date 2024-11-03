#include "raylib.h"
#include <iostream>

#pragma region imgui
#include "imgui.h"
#include "rlImGui.h"
#include "imguiThemes.h"
#pragma endregion

#include <physics.h>

int main(void)
{

	PhysicsEngine engine;

	for (int i = 0; i < 200; i++)
	{

		engine.addSphere({ rand()%15-7, rand()%10 + 5, rand() % 15 - 7},
			(rand()%3 + 1)/3.f);

	}

	//engine.addCube({ 0, 10, 0 }, { 2,2,2 });
	//engine.addSphere({ 3, 20, 2 }, 1);
	//engine.addSphere({ 3, 30, 4 }, 1);
	//engine.addSphere({ 5, 20, 6 }, 1);
	//engine.addSphere({ 7, 20, 8 }, 1);
	//engine.addSphere({ 9, 20, 10 }, 1);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 600, "Physics simulator");

#pragma region imgui
	rlImGuiSetup(true);

	//you can use whatever imgui theme you like!
	//ImGui::StyleColorsDark();
	//imguiThemes::yellow();
	//imguiThemes::gray();
	imguiThemes::green();
	//imguiThemes::red();
	//imguiThemes::embraceTheDarkness();


	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.FontGlobalScale = 2;

	ImGuiStyle &style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		//style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.5f;
		//style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
	}

#pragma endregion

	Camera3D camera = { 0 };
	camera.position = Vector3{ 10.0f, 10.0f, 30.0f }; // Camera position
	camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
	camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	camera.fovy = 45.0f;                                // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

	Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

	DisableCursor();                    // Limit cursor to relative movement inside the window

	SetTargetFPS(60);

	while (!WindowShouldClose())
	{
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

		engine.update(GetFrameTime());

		/*
		ImGui::Begin("Test");

		ImGui::Text("Hello");
		ImGui::Button("Button");
		ImGui::Button("Button2");

		ImGui::End();
		*/

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		{
			UpdateCamera(&camera, CAMERA_FREE);
			camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
		}

		if (IsKeyPressed('Z')) camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------

		ClearBackground(RAYWHITE);

		BeginMode3D(camera);

		{

			for (auto& o : engine.objects) 
			{
				if (o.type == 0) 
				{
				DrawCube(  
					{o.pos.x, o.pos.y, o.pos.z},
					o.size.x,o.size.y,o.size.z ,
					{ 100,255,255,255 });
				}
				else if (o.type == 1) 
				{

					DrawSphereWires({ o.pos.x, o.pos.y, o.pos.z }, o.size.x / 2.f, 10, 10,
						{ 255,0,0,255 });

					DrawSphere({ o.pos.x, o.pos.y, o.pos.z }, o.size.x / 2.f,
						{ 255,155,100,255 });

				}

			}

		}

		DrawGrid(20, 1.0f);

		DrawCube({0,engine.glassContainer.y/2.f,0}, engine.glassContainer.x,
			engine.glassContainer.y, engine.glassContainer.z, { 155,100,100,20 });
		DrawCubeWires({ 0,engine.glassContainer.y / 2.f,0 }, engine.glassContainer.x,
			engine.glassContainer.y, engine.glassContainer.z, MAROON);


		EndMode3D();

	

	#pragma region imgui
		rlImGuiEnd();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	#pragma endregion

		EndDrawing();
	}


#pragma region imgui
	rlImGuiShutdown();
#pragma endregion



	CloseWindow();

	return 0;
}