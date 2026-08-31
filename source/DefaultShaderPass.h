#pragma once

#include <ShaderPass.h>

class DefaultShaderPass : public ShaderPass
{
public:
	DefaultShaderPass(std::string vertFilePath, std::string fragFilePath);
};

