#pragma once

class Shader;

class ShaderLibrary
{
public:
	NODISCARD static Ref<Shader> CreateShader(std::string_view name);
	NODISCARD static Ref<Shader> GetShader(std::string_view name);
	static void                  AddShader(const Ref<Shader> &shader);
	static void                  AddShader(const Ref<Shader> &shader, const std::string &name);
	static bool                  RemoveShader(std::string_view name);

	NODISCARD FORCEINLINE static bool ShaderExists(std::string_view name)
	{
		return s_Shaders.find(name.data()) != s_Shaders.end();
	}

private:
	static std::unordered_map<std::string, Ref<Shader>> s_Shaders;
};
