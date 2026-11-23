#include <iostream>
#include <minilog.h>
#include "Utils.h"

#include <GLFW/glfw3.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <lvk/LVK.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/version.h>

using glm::vec3;
using glm::mat4;

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

	// assimp
	const aiScene* scene = aiImportFile(MODEL_DIR "/rubber_duck/scene.gltf", aiProcess_Triangulate);
	if (!scene || !scene->HasMeshes())
	{
		printf("Unable to load data/rubber_duck/scene.gltf\n");
		exit(255);
	}

	const aiMesh* mesh = scene->mMeshes[0];
	std::vector<vec3> positions;
	std::vector<uint32_t> indices;
	for (unsigned int i = 0; i != mesh->mNumVertices; i++)
	{
		const aiVector3D v = mesh->mVertices[i];
		positions.push_back(vec3(v.x, v.y, v.z));
	}

	for (unsigned int i = 0; i != mesh->mNumFaces; i++)
	{
		for (int j = 0; j != 3; j++)
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}
	aiReleaseImport(scene);

	lvk::Holder<lvk::BufferHandle> vertexBuffer = ctx->createBuffer({
		.usage = lvk::BufferUsageBits_Vertex,
		.storage = lvk::StorageType_Device,
		.size = sizeof(vec3) * positions.size(),
		.data = positions.data(),
		.debugName = "Buffer: vertex" });
	lvk::Holder<lvk::BufferHandle> indexBuffer = ctx->createBuffer({
		.usage = lvk::BufferUsageBits_Index,
		.storage = lvk::StorageType_Device,
		.size = sizeof(uint32_t) * indices.size(),
		.data = indices.data(),
		.debugName = "Buffer: index" });
	lvk::Holder<lvk::TextureHandle> depthTexture = ctx->createTexture({
		.type = lvk::TextureType_2D,
		.format = lvk::Format_Z_F32,
		.dimensions = {(uint32_t)width, (uint32_t)height},
		.usage = lvk::TextureUsageBits_Attachment,
		.debugName = "Depth buffer" });

	const lvk::VertexInput vdesc = {
		.attributes = { {.location = 0, .format = lvk::VertexFormat_Float3} },
		.inputBindings = { {.stride = sizeof(vec3)} }
	};

	lvk::Holder<lvk::ShaderModuleHandle> vert =
		loadShaderModule(ctx, SHADER_DIR "/main.vert");
	lvk::Holder<lvk::ShaderModuleHandle> frag =
		loadShaderModule(ctx, SHADER_DIR "/main.frag");
	lvk::Holder<lvk::RenderPipelineHandle> pipelineSolid =
		ctx->createRenderPipeline({
			.vertexInput = vdesc,
			.smVert = vert,
			.smFrag = frag,
			.color = { {.format = ctx->getSwapchainFormat()} },
			.depthFormat = ctx->getFormat(depthTexture),
			.cullMode = lvk::CullMode_Back
			});

	const uint32_t isWireframe = 1;
	lvk::Holder<lvk::RenderPipelineHandle> pipelineWireframe =
		ctx->createRenderPipeline({
			.vertexInput = vdesc,
			.smVert = vert,
			.smFrag = frag,
			.specInfo = {
				.entries = { {.constantId = 0, .size = sizeof(uint32_t)} },
				.data = &isWireframe,
				.dataSize = sizeof(isWireframe) },
			.color = { {.format = ctx->getSwapchainFormat()} },
			.depthFormat = ctx->getFormat(depthTexture),
			.cullMode = lvk::CullMode_Back,
			.polygonMode = lvk::PolygonMode_Line
			});

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		const float ratio = width / height;
		const mat4 p = glm::perspective(45.0f, ratio, 0.1f, 1000.0f);
		
		const mat4 m = glm::rotate(
			mat4(1.0f), glm::radians(-90.0f), vec3(1, 0, 0));
		const mat4 v = glm::rotate(glm::translate(mat4(1.0f),
			vec3(0.0f, -0.5f, -1.5f)), (float)glfwGetTime(),
			vec3(0.0f, 1.0f, 0.0f));
		
		const lvk::RenderPass renderPass = {
			.color = {
				{.loadOp = lvk::LoadOp_Clear, .clearColor = { 1, 1, 1, 1 }}},
			.depth = {.loadOp = lvk::LoadOp_Clear, .clearDepth = 1. }
		};
		const lvk::Framebuffer framebuffer = {
			.color = {{.texture = ctx->getCurrentSwapchainTexture() }},
			.depthStencil = {.texture = depthTexture }
		};

		lvk::ICommandBuffer& buf = ctx->acquireCommandBuffer();
		buf.cmdBeginRendering(renderPass, framebuffer);
		buf.cmdBindVertexBuffer(0, vertexBuffer);
		buf.cmdBindIndexBuffer(indexBuffer, lvk::IndexFormat_UI32);
		
		buf.cmdBindRenderPipeline(pipelineSolid);
		buf.cmdBindDepthState({ .compareOp = lvk::CompareOp_Less,
			.isDepthWriteEnabled = true });
		buf.cmdPushConstants(p * v * m);
		buf.cmdDrawIndexed(indices.size());

		buf.cmdBindRenderPipeline(pipelineWireframe);
		buf.cmdSetDepthBiasEnable(true);
		buf.cmdSetDepthBias(0.0f, -1.0f, 0.0f);
		buf.cmdDrawIndexed(indices.size());

		buf.cmdEndRendering();
		ctx->submit(buf, ctx->getCurrentSwapchainTexture());
	}

	vert.reset();
	frag.reset();
	depthTexture.reset();
	pipelineSolid.reset();
	pipelineWireframe.reset();
	vertexBuffer.reset();
	indexBuffer.reset();
	ctx.reset();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}