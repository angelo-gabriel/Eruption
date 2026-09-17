#include <iostream>
#include <minilog.h>
#include "Utils.h"

#include <GLFW/glfw3.h>

#include <lvk/LVK.h>

int main()
{
	minilog::initialize(nullptr, { .threadNames = false });

	int width = 1280;
	int height = 720;

	GLFWwindow* window = lvk::initWindow("Vulkano App", width, height);
	if (!window)
	{
		std::cerr << "Falha ao criar janela!\n";
		return -1;
	}

	lvk::ContextConfig cfg = {};

	std::unique_ptr<lvk::IContext> ctx = lvk::createVulkanContextWithSwapchain(
		window,
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height),
		cfg
	);

	if (!ctx)
	{
		std::cerr << "Falha ao inicializar LightweightVK!\n";
		return -1;
	}
	std::cout << "LightweightVK inicializado com sucesso!\n";

	lvk::Holder<lvk::ShaderModuleHandle> vert = loadShaderModule(
		ctx, SHADER_DIR "/main.vert");
	lvk::Holder<lvk::ShaderModuleHandle> frag = loadShaderModule(
		ctx, SHADER_DIR "/main.frag");
	lvk::Holder<lvk::RenderPipelineHandle> rpTriangle =
		ctx->createRenderPipeline({
			.smVert = vert,
			.smFrag = frag,
			.color = { {.format = ctx->getSwapchainFormat() } } });

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		lvk::ICommandBuffer& buf = ctx->acquireCommandBuffer();
		lvk::TextureHandle swapchainTex = ctx->getCurrentSwapchainTexture();

		buf.cmdBeginRendering(
			{.color = {{ .loadOp = lvk::LoadOp_Clear, .clearColor = {1, 1, 1, 1} }}},
			{.color = {{ .texture = ctx->getCurrentSwapchainTexture() }}});
		buf.cmdBindRenderPipeline(rpTriangle);
		buf.cmdPushDebugGroupLabel("Render Triangle", 0xff0000ff);
		buf.cmdDraw(3);
		buf.cmdPopDebugGroupLabel();
		buf.cmdEndRendering();

		ctx->submit(buf, swapchainTex);
	}

	ctx.reset();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}