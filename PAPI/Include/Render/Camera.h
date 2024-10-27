#pragma once

class Camera
{
public:
	Transform Transformation = Transform();
	float     FOV            = 90;
	float     NearPlane      = 0.1f;
	float     FarPlane       = 1000.0f;
};
