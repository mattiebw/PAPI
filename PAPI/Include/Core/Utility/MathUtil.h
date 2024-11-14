#pragma once

struct Rect
{
	glm::vec2 Position;
	glm::vec2 Size;

	Rect(glm::vec2 position, glm::vec2 size)
		: Position(position), Size(size)
	{}

	Rect(float x, float y, float width, float height)
		: Position(glm::vec2(x, y)), Size(glm::vec2(width, height))
	{}
	
	NODISCARD FORCEINLINE bool OverlapsWith(const Rect& other)
	{
		return Position.x < other.Position.x + other.Size.x &&  // Not to the right of the other rectangle
			   Position.x + Size.x > other.Position.x &&      // Not to the left of the other rectangle
			   Position.y < other.Position.y + other.Size.y &&  // Not below the other rectangle
			   Position.y + Size.y > other.Position.y;         // Not above the other rectangle
	}

	NODISCARD FORCEINLINE bool Contains(const glm::vec2& point)
	{
		return Position.x <= point.x && Position.x + Size.x >= point.x
			&& Position.y <= point.y && Position.y + Size.y >= point.y;
	}
};

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
