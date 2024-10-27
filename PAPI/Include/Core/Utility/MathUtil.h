#pragma once

class MathUtil
{
public:
	static glm::mat4 CreateTransformationMatrix(const glm::vec3 &translation, const glm::vec3 &rotation,
	                                            const glm::vec3 &scale);
};
