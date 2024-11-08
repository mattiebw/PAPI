#pragma once

class Camera
{
public:
	Transform Transformation = Transform();
	float     FOV            = 90;
	float     NearPlane      = 0.1f;
	float     FarPlane       = 1000.0f;
	float     Aspect         = 0;

	NODISCARD FORCEINLINE glm::mat4 GetViewMatrix() const
	{
		return inverse(MathUtil::CreateTransformationMatrix(Transformation.Position, Transformation.Rotation, Transformation.Scale));
	}
	
	NODISCARD FORCEINLINE glm::mat4 GetViewProjectionMatrix() const
	{
		return glm::perspective(FOV, Aspect, NearPlane, FarPlane) * GetViewMatrix();
	}
};
