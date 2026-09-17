#pragma once

#include <string.h>
#include <string>

#include <lvk/LVK.h>

bool endsWith(const char* s, const char* part);

std::string readShaderFile(const char* fileName);

lvk::ShaderStage lvkShaderStageFromFileName(const char* fileName);

lvk::Holder<lvk::ShaderModuleHandle> loadShaderModule(const std::unique_ptr<lvk::IContext>& ctx, const char* fileName);
