#pragma once

#define VK_NO_PROTOTYPES 1  
#include <vulkan.h>
#include "dispatch.h"
#include "vertex.h"
#include <vector>
#include <fstream>

/// <summary>
/// 
/// Gradient triangle running on Vulkan API
///
/// </summary>

const VkFormat kBackBufferFormat = VK_FORMAT_R8G8B8A8_SRGB;
const int MAX_FRAMES_IN_FLIGHT = 2;

// triangle vertices
const std::vector<Vertex> t_vertices = {
	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

// cube vertices

class VulkanRender {

	friend class CEngine;

	VulkanRender();
	~VulkanRender();

	void Update();

	void init_Instance();
	void init_PhysicalDevice();
	void init_Device();
	void init_Surface();
	void init_Swapchain();
	void init_ImageViews();
	void init_RenderPass();
	void init_GraphicsPipeline();
	void init_Framebuffers();
	void init_CommandPool();
	void init_VertexBuffer();
	void init_CommandBuffers();
	void init_SyncObjects();

	static std::vector<char> readFile(const std::string& filename); 
	VkShaderModule init_ShaderModule(const std::vector<char>& code);
	VkSurfaceFormatKHR getSwapSurfaceFormat();
	uint32_t findMemoryType(uint32_t typeFilter);

	void cleanupSwapchain();
	void reinit_Swapchain();

	size_t currentFrame = 0;
	SDispatch m_Dispatch{};
	int m_Width;
	int m_Height;
	VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;

	VkInstance m_Instance; 
	VkPhysicalDevice m_PhysicalDevice; 
	int m_GraphicsQueueFamily = -1; 
	VkDevice m_Device;
	VkQueue m_Queue; 
	VkSurfaceKHR m_Surface; 
	VkSwapchainKHR m_Swapchain; 
	std::vector<VkImage> m_ScImages; 
	std::vector<VkImageView> m_ScImageViews;
	VkRenderPass m_RenderPass; 
	VkShaderModule m_VertShader; 
	VkShaderModule m_FragShader; 
	VkPipelineLayout m_PipelineLayout; 
	VkPipeline m_Pipeline; 
	std::vector<VkFramebuffer> m_Framebuffers;
	VkCommandPool m_CommandPool;
	VkBuffer m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMemory;
	std::vector<VkCommandBuffer> m_CommandBuffers;
	std::vector <VkSemaphore> m_PresentSemaphores; 
	std::vector<VkSemaphore> m_SubmitSemaphores; 
	std::vector<VkFence> m_Fences; 
	std::vector<VkFence> imagesInFlight;

public:
	void onWindowResize(int width, int height);
};