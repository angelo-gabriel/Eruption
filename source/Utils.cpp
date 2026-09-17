#include <cstring>
#include <fstream>
#include <string>
#include <string_view>

#include "Utils.h"

bool endsWith(const char* s, const char* part)
{
	if (!s || !part)
		return false;

	const std::string_view str{ s };
	const std::string_view suffix{ part };

	return str.size() >= suffix.size() &&
		str.ends_with(suffix);
}

std::string readShaderFile(const char* fileName)
{
	std::ifstream file(fileName, std::ios::binary);

	if (!file)
	{
		LLOGW("I/O error. Cannot open shader file '%s'\n", fileName);
		return {};
	}

	std::string code{
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	};

	if (code.size() >= 3 &&
		static_cast<unsigned char>(code[0]) == 0xEF &&
		static_cast<unsigned char>(code[1]) == 0xBB &&
		static_cast<unsigned char>(code[2]) == 0xBF)
	{
		code.erase(0, 3);
	}

	constexpr std::string_view includeDirective = "#include ";

	while (true)
	{
		const auto pos = code.find(includeDirective);

		if (pos == std::string::npos)
			break;

		const auto p1 = code.find('<', pos);
		const auto p2 = code.find('>', p1);

		if (p1 == std::string::npos ||
			p2 == std::string::npos ||
			p2 <= p1)
		{
			LLOGW(
				"Error while loading shader program: %s\n",
				code.c_str()
			);
			return {};
		}

		const std::string name = code.substr(
			p1 + 1,
			p2 - p1 - 1
		);

		const std::string includedCode = readShaderFile(name.c_str());

		code.replace(
			pos,
			p2 - pos + 1,
			includedCode
		);
	}

	return code;
}

lvk::ShaderStage lvkShaderStageFromFileName(const char* fileName)
{
	if (endsWith(fileName, ".vert"))
		return lvk::Stage_Vert;

	if (endsWith(fileName, ".frag"))
		return lvk::Stage_Frag;

	if (endsWith(fileName, ".geom"))
		return lvk::Stage_Geom;

	if (endsWith(fileName, ".comp"))
		return lvk::Stage_Comp;

	if (endsWith(fileName, ".tesc"))
		return lvk::Stage_Tesc;

	if (endsWith(fileName, ".tese"))
		return lvk::Stage_Tese;

	return lvk::Stage_Vert;
}

lvk::Holder<lvk::ShaderModuleHandle> loadShaderModule(
	const std::unique_ptr<lvk::IContext>& ctx,
	const char* fileName)
{
	const std::string code = readShaderFile(fileName);

	if (code.empty())
		return {};

	const lvk::ShaderStage stage = lvkShaderStageFromFileName(fileName);
	lvk::Holder<lvk::ShaderModuleHandle> handle =
		ctx->createShaderModule({ code.c_str(), stage,
			(std::string("Shader Module: ") + fileName).c_str() });

	return handle;
}