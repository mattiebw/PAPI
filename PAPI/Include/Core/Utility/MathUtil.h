#pragma once

class MathUtil
{
public:
	NODISCARD static glm::mat4 CreateTransformationMatrix(const glm::vec3 &translation, const glm::vec3 &rotation,
	                                                      const glm::vec3 &scale);

	NODISCARD FORCEINLINE static float LerpSmooth(float a, float b, float r, float delta)
	{
		return (a - b) * glm::pow(r, delta) + b;
	}
	
	NODISCARD FORCEINLINE static glm::vec2 LerpSmooth(glm::vec2 a, glm::vec2 b, float r, float delta)
	{
		return { LerpSmooth(a.x, b.x, r, delta), LerpSmooth(a.y, b.y, r, delta) };
	}
	
	NODISCARD FORCEINLINE static glm::vec3 LerpSmooth(const glm::vec3& a, const glm::vec3& b, float r, float delta)
	{
		return { LerpSmooth(a.x, b.x, r, delta), LerpSmooth(a.y, b.y, r, delta), LerpSmooth(a.z, b.z, r, delta) };
	}
};
