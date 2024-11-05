#include "raylib.h"
#include <iostream>

#include "imgui.h"
#include "rlImGui.h"
#include "imguiThemes.h"

#include <physics.h>


int main(void)
{

	PhysicsSimulator engine;

	for (int i = 0; i < 0; i++)
	{

		//engine.addSphere({ rand()%15-7, rand()%10 + 5, rand() % 15 - 7},
		//	(rand()%3 + 1)/3.f);
		//
		engine.addCube({ rand() % ((int)engine.glassContainer.x -5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5, rand() % ((int)engine.glassContainer.z - 5) - 7},
			{ (rand() % 3 + 1) ,(rand() % 3 + 1) ,(rand() % 3 + 1)});

		engine.addCylindre({ rand() % ((int)engine.glassContainer.x - 5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5, rand() % ((int)engine.glassContainer.z - 5) - 7 },
			(rand() % 2 + 1) / 3.f, (rand() % 3 + 2) / 3.f);

	}

		//engine.addCube({ 2, 2, 1 }, { 2,2,2 });
		//engine.addCylindre({ 3, 10, 2 }, 2, 3);
		//engine.addCube({ 4, 16, 3 }, { 2,2,2 });
	


	engine.addSphere({ 1, 2, 0 }, 1);
	engine.addSphere({ 0, 4, 0 }, 1);
	engine.addSphere({ 3, 25, 4 }, 1);
	engine.addSphere({ 5, 22, 6 }, 1);
	engine.addSphere({ 29, 45, 8 }, 1);
	engine.addSphere({ 9, 20, 10 }, 1);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 600, "The Sims | Physics simulator");

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
	camera.position = Vector3{ 10.0f, 10.0f, 30.0f }; 
	camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      
	camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          
	camera.fovy = 45.0f;                                
	camera.projection = CAMERA_PERSPECTIVE; 
	int option = 0;

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

		}

		if (IsKeyPressed('Z')) camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
		if (IsKeyPressed('1')) option = 1;
		if (IsKeyPressed('2')) option = 2;
		if (IsKeyPressed('3')) option = 3;
		if (IsKeyPressed('R')) {
			for (Object& o : engine.objects) {
				o.pos = { rand() % ((int)engine.glassContainer.x - 5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5, 
					 rand() % ((int)engine.glassContainer.z - 5) - 7 };
			}
		}

		std::cout << engine.objects.size() << '\n';

		
		if (option == 1) {
			engine.glassContainer = { 50,50,50 };
			engine.objects.clear();

			for (int i = 0; i < 500; i++)
			{
				if (i < 100) {
				engine.addSphere({ rand()% ((int)engine.glassContainer.x - 5) -7, rand()% ((int)engine.glassContainer.y - 10) + 5, 
					rand() % ((int)engine.glassContainer.z - 5) + 5}, (rand()%3 + 1)/3.f);
				}
				if (i < 250) {
					engine.addCube({ rand() % ((int)engine.glassContainer.x - 5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5, rand() % ((int)engine.glassContainer.z - 5) - 7 },
						{ (rand() % 3 + 1) ,(rand() % 3 + 1) ,(rand() % 3 + 1) });
				}

				engine.addCylindre({ rand() % ((int)engine.glassContainer.x - 5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5, rand() % ((int)engine.glassContainer.z - 5) - 7 },
					(rand() % 2 + 1) / 3.f, (rand() % 3 + 2) / 3.f);

			}
			option = 0;
		}
		if (option == 2) {
			engine.glassContainer = { 100,100,100 };
			engine.objects.clear();

			for (int i = 0; i < 1000; i++)
			{
				if (i < 250) {
					engine.addSphere({ rand() % ((int)engine.glassContainer.x - 5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5,
						rand() % ((int)engine.glassContainer.z - 5) + 5 }, (rand() % 3 + 1) / 3.f);
				}
				if (i < 500) {
					engine.addCube({ rand() % ((int)engine.glassContainer.x - 5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5, rand() % ((int)engine.glassContainer.z - 5) - 7 },
						{ (rand() % 3 + 1) ,(rand() % 3 + 1) ,(rand() % 3 + 1) });
				}

				engine.addCylindre({ rand() % ((int)engine.glassContainer.x - 5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5, rand() % ((int)engine.glassContainer.z - 5) - 7 },
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
					engine.addSphere({ rand() % ((int)engine.glassContainer.x - 5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5,
						rand() % ((int)engine.glassContainer.z - 5) + 5 }, (rand() % 3 + 1) / 3.f);
				}
				if (i < 1000) {
					engine.addCube({ rand() % ((int)engine.glassContainer.x - 5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5, rand() % ((int)engine.glassContainer.z - 5) - 7 },
						{ (rand() % 3 + 1) ,(rand() % 3 + 1) ,(rand() % 3 + 1) });
				}

				engine.addCylindre({ rand() % ((int)engine.glassContainer.x - 5) - 7, rand() % ((int)engine.glassContainer.y - 10) + 5, rand() % ((int)engine.glassContainer.z - 5) - 7 },
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