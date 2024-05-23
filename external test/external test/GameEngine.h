#pragma once
#include "Includes.h"
#include "Offsets.h"
#include "Memory.h"
#include "Vector.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"



class CBaseEntity
{

	CDispatcher* mem = CDispatcher::Get();
public:
	uintptr_t GetPlayerController(uintptr_t base)
	{
		return mem->ReadMemory<uintptr_t>(base + dwLocalPlayerController);
	}

	uintptr_t GetPlayerPawn(uintptr_t base)
	{
		auto localPlayerController = GetPlayerController(base);
		if (!localPlayerController) return 0;

		return mem->ReadMemory<uintptr_t>(localPlayerController + m_hPlayerPawn);
	}

	uintptr_t GetEntityList(uintptr_t base)
	{
		return mem->ReadMemory<uintptr_t>(base + dwEntityList);
	}

	int GetLocalTeam(uintptr_t base)
	{
		auto localPlayer = GetPlayerController(base);
		if (!localPlayer) return 0;

		return mem->ReadMemory<int>(localPlayer + m_iTeamNum);
	}

	uintptr_t GetLocalListEntry2(uintptr_t base)
	{
		auto localPlayerPawn = GetPlayerPawn(base);
		if (!localPlayerPawn) return 0;

		auto entityList = GetEntityList(base);
		if (!entityList) return 0;

		return mem->ReadMemory<uintptr_t>(entityList + 0x8 * ((localPlayerPawn & 0x7FFF) >> 9) + 16);
	}

	float getSensitivity(uintptr_t base)
	{
		uintptr_t sense = mem->ReadMemory<uintptr_t>(base + dwSensitivity);

		return mem->ReadMemory<float>(sense + dwSensitivity_sensitivity);
	}

	uintptr_t GetLocalCSPlayerPawn(uintptr_t base)
	{
		auto localListEntry2 = GetLocalListEntry2(base);
		if (!localListEntry2) return 0;

		auto localPlayerPawn = GetPlayerPawn(base);
		if (!localPlayerPawn) return 0;

		return mem->ReadMemory<uintptr_t>(localListEntry2 + 120 * (localPlayerPawn & 0x1FF));
	}

	CVector GetLocalOrigin(uintptr_t base)
	{
		auto localCSPlayerPawn = GetLocalCSPlayerPawn(base);
		if (!localCSPlayerPawn) return CVector{};

		return mem->ReadMemory<CVector>(localCSPlayerPawn + m_vOldOrigin);
	}

	uintptr_t getGameSceneNode(uintptr_t entity)
	{
		return mem->ReadMemory<uintptr_t>(entity + m_pGameSceneNode);
	}

	CVector GetBone(uintptr_t gameSceneNode, int bone)
	{
		auto boneArray = mem->ReadMemory<uint64_t>(gameSceneNode + 0x160 + 0x80);
		if (!boneArray) return CVector{};

		return mem->ReadMemory<CVector>(boneArray + (bone * 32));
	}



	bool isLocalPlayer(uintptr_t entity)
	{
		uintptr_t base;
		auto localPlayerPawn = GetPlayerPawn(base);
		if (!localPlayerPawn) return false;

		auto playerPawn = mem->ReadMemory<uintptr_t>(entity + m_hPlayerPawn);
		if (!playerPawn) return false;

		return playerPawn == localPlayerPawn;
	}
};

class CBasePlayer : public CBaseEntity
{
	CDispatcher* mem = CDispatcher::Get();
public:
	uintptr_t GetEntity(uintptr_t entityList, int playerIndex)
	{
		auto listEntry = mem->ReadMemory<uintptr_t>(entityList + (8 * (playerIndex & 0x7FFF) >> 9) + 16);
		if (!listEntry) return 0;
		return mem->ReadMemory<uintptr_t>(listEntry + 120 * (playerIndex & 0x1FF));
	}

	int GetTeam(uintptr_t entity)
	{
		return mem->ReadMemory<int>(entity + m_iTeamNum);
	}

	uintptr_t GetPlayerPawn(uintptr_t entity_list, int playerIndex)
	{
		auto list_entry = mem->ReadMemory<uintptr_t>(entity_list + (8 * (playerIndex & 0x7FFF) >> 9) + 16);
		if (!list_entry) return 0;

		auto entity2 = mem->ReadMemory<uintptr_t>(list_entry + 120 * (playerIndex & 0x1FF));

		return mem->ReadMemory<uintptr_t>(entity2 + m_hPlayerPawn);
	}

	uintptr_t getPawn(uintptr_t entity)
	{
		return mem->ReadMemory<uintptr_t>(entity + m_hPlayerPawn);
	}

	uintptr_t getPCSPlayerPawn(uintptr_t base, int playerIndex) // Include base parameter here
	{
		auto entity_list = GetEntityList(base);
		if (!entity_list) return 0;

		auto entity = GetEntity(entity_list, playerIndex);

		auto pawn = getPawn(entity);
		if (!pawn) return 0;

		auto list_entry2 = mem->ReadMemory<uintptr_t>(entity_list + 0x8 * ((pawn & 0x7FFF) >> 9) + 16);
		if (!list_entry2) return 0;

		return mem->ReadMemory<uintptr_t>(list_entry2 + 120 * (pawn & 0x1FF));
	}

	uintptr_t GetGameSceneNode(uintptr_t entity)
	{
		return mem->ReadMemory<uintptr_t>(entity + m_pGameSceneNode);
	}

	CVector GetOrigin(uintptr_t entity)
	{
		return mem->ReadMemory<CVector>(entity + m_vOldOrigin);
	}



	int getLocalTeam(uintptr_t base)
	{
		auto localPlayer = GetPlayerController(base);
		if (!localPlayer) return 0;

		return mem->ReadMemory<int>(localPlayer + m_iTeamNum);
	}

	bool isSpotted(uintptr_t entity)
	{
		return mem->ReadMemory<bool>(entity + m_entitySpottedState + m_bSpotted);
	}

	bool isSpottedByMask(uintptr_t playerPawn, int localIndex)
	{
		uintptr_t state = mem->ReadMemory<uintptr_t>(playerPawn + m_entitySpottedState + m_bSpottedByMask);
		return state & (1 << localIndex - 1);
	}

	bool isLocalTeam(uintptr_t entity, uintptr_t localPlayer)
	{
		auto team = GetTeam(entity);
		auto localTeam = getLocalTeam(localPlayer);

		return team == localTeam;
	}

	bool isGrounded(uintptr_t entity)
	{
		return mem->ReadMemory<bool>(entity + 0x30);
	}

	int crossHairID(uintptr_t entity)
	{
		return mem->ReadMemory<int>(entity + 0x13B0);
	}

	bool isAlive(uintptr_t entity)
	{
		auto health = mem->ReadMemory<int>(entity + m_iHealth);
		return health > 0 && health <= 100;
	}

	int getHealth(uintptr_t entity)
	{
		return mem->ReadMemory<int>(entity + m_iHealth);
	}

	bool isPlayer(uintptr_t entity)
	{
		auto playerPawn = getPawn(entity);
		return playerPawn != 0;
	}

	bool isEnemy(uintptr_t entity, uintptr_t localPlayer)
	{
		return !isLocalTeam(entity, localPlayer);
	}
};









//bool w2s(view_matrix_t viewMatrix, CVector origin, CVector& screen)
//{
//	float screenW = (viewMatrix.matrix[3][0] * origin.x) + (viewMatrix.matrix[3][1] * origin.y) + (viewMatrix.matrix[3][2] * origin.z) + viewMatrix.matrix[3][3];
//
//	if (!(screenW > 0.001f))
//		return false;
//
//	float screenX = (viewMatrix.matrix[0][0] * origin.x) + (viewMatrix.matrix[0][1] * origin.y) + (viewMatrix.matrix[0][2] * origin.z) + viewMatrix.matrix[0][3];
//	float screenY = (viewMatrix.matrix[1][0] * origin.x) + (viewMatrix.matrix[1][1] * origin.y) + (viewMatrix.matrix[1][2] * origin.z) + viewMatrix.matrix[1][3];
//
//	float camX = 1920 / 2.f;
//	float camY = 1080 / 2.f;
//
//	float X = camX + (camX * screenX / screenW);
//	float Y = camY - (camY * screenY / screenW);
//
//	screen.x = (int)X;
//	screen.y = (int)Y;
//
//	return true;
//}