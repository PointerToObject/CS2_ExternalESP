#include "GameEngine.h"
#include "../window.hpp"
#include "Includes.h"
#include "Memory.h"
#include "Offsets.h"
#include "Vector.h"
#include <thread>
#include "w2s.h"




int main()
{
	// thread your cheat here 
	ImDrawList* draw;
	CDispatcher* mem = CDispatcher::Get();
	mem->Attach("cs2.exe");
	uintptr_t base = mem->GetModuleBase("client.dll");

	CBaseEntity centity;
	CBasePlayer cplayer;
	// hide console window since we're making our own window, or use WinMain() instead.
	//ShowWindow(GetConsoleWindow(), SW_HIDE);



	overlay.shouldRun = true;
	overlay.RenderMenu = false;

	overlay.CreateOverlay();
	overlay.CreateDevice();
	overlay.CreateImGui();

	printf("[>>] Hit insert to show the menu in this overlay!\n");

	overlay.SetForeground(GetConsoleWindow());

	while (overlay.shouldRun)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		overlay.StartRender();
		
		if (overlay.RenderMenu) {
			overlay.Render();
		}
		else
			ImGui::GetBackgroundDrawList()->AddText({ 0, 0 }, ImColor(0.0f, 1.0f, 0.0f), "-.-.- RC7 v0.1 -.-.-");

		if (esp) {
				for (int i = 0; i < 64; i++) {
					uintptr_t entitylist = centity.GetEntityList(base);
					view_matrix_t vm = mem->ReadMemory<view_matrix_t>(base + dwViewMatrix);
					uintptr_t playerpawn = cplayer.getPCSPlayerPawn(base, i);
					int health = cplayer.getHealth(playerpawn);
					int team = cplayer.GetTeam(playerpawn);
					int localTeam = cplayer.getLocalTeam(base);
					CVector vecOrig = cplayer.GetOrigin(playerpawn);
					CVector localOrig = cplayer.GetLocalOrigin(base);
					if (!health)
						continue;

					CVector screen;
					ImVec2 screensize = ImGui::GetIO().DisplaySize;
					float x = vecOrig.x - localOrig.x;
					float y = vecOrig.y - localOrig.y;
					float z = vecOrig.z - localOrig.z;
					float distance = sqrt(x * x + y * y + z * z);
					char textBuffer[64];
					sprintf_s(textBuffer, "PLAYER [%.1fm]", distance);
					//ImGui::Text("pos test : %0.3f %0.3f %0.3f ", vecOrig.x, vecOrig.y, vecOrig.z);
					if (!w2s(screensize, vecOrig, screen, vm))
						continue;
					if (localTeam != team) {
						ImGui::GetForegroundDrawList()->AddText(ImVec2(screen.x + 6.0, screen.y + 3.0), IM_COL32(255, 255, 255, 255), textBuffer, 0);
					}

				}
		}


		// if you want to render here, you could move the imgui includes to your .hpp file instead of the .cpp file!
		overlay.EndRender();
	}

	overlay.DestroyImGui();
	overlay.DestroyDevice();
	overlay.DestroyOverlay();
}

