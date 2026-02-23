#pragma once

#include <Windows.h>
#include "../../Public/Snowdrop/SDK.h"

extern bool bCameraOverrideEnabled;
extern bool bFOVOverrideEnabled;
extern bool bFreecamEnabled;
extern float fCustomFOV;
extern float fFreecamSensitivity;
extern float fScrollFOVDelta;

inline void UpdateCameraOverrides()
{
	if (!bCameraOverrideEnabled)
		return;

	auto* client = Snowdrop::CamelClient::GetInstance();
	if (!client || !client->GetWorldModel() || !client->GetWorldModel()->pCameraManager)
		return;

	auto* cam = client->GetWorldModel()->pCameraManager->pCamera;
	if (!cam)
		return;

	// FOV override
	if (bFOVOverrideEnabled)
		cam->position.myY = fCustomFOV;

	if (bFreecamEnabled)
	{
		float speed = 0.5f * fFreecamSensitivity;

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			cam->position.myX -= speed;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			cam->position.myX += speed;
		if (GetAsyncKeyState(VK_UP) & 0x8000)
			cam->position.myZ += speed;
		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
			cam->position.myZ -= speed;

		if (fScrollFOVDelta != 0.0f)
		{
			cam->position.myY += fScrollFOVDelta * fFreecamSensitivity;
			fScrollFOVDelta = 0.0f;
		}
	}
}
