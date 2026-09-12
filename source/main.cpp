#include <iostream>
#include <minilog.h>

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

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		lvk::ICommandBuffer& cmd = ctx->acquireCommandBuffer();
		lvk::TextureHandle swapchainTex = ctx->getCurrentSwapchainTexture();

		cmd.cmdBeginRendering(
			lvk::RenderPass{
				.color = {{
						.loadOp = lvk::LoadOp_Clear,
						.storeOp = lvk::StoreOp_Store,
						.clearColor = {0.1f, 0.2, 0.3f, 1.0f}
				}}
			},
			lvk::Framebuffer{ .color = {{.texture = swapchainTex }} }
		);
		cmd.cmdEndRendering();

		ctx->submit(cmd, swapchainTex);
	}

	ctx.reset();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}