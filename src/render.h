#pragma once

#define VK_NO_PROTOTYPES 1  
#include <vulkan.h>
#include "dispatch.h"
#include "vertex.h"
#include <vector>
#include <fstream>

const VkFormat kBackBufferFormat = VK_FORMAT_R8G8B8A8_SRGB;
const int MAX_FRAMES_IN_FLIGHT = 2;

// triangle vertices
const std::vector<Vertex> t_vertices = {
	{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};

//triangle indices
const std::vector<uint16_t> t_indices = { 0, 1, 2, };

// rectangle vertices
const std::vector<Vertex> r_vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

// rectangle indices
const std::vector<uint16_t> r_indices = { 0, 1, 2, 2, 3, 0 };

// cube vertices
const std::vector<Vertex> c_vertices;

const std::vector<Vertex> m_Vertices = r_vertices;
const std::vector<uint16_t> m_Indices = r_indices;

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
	void init_IndexBuffer();
	void init_CommandBuffers();
	void init_SyncObjects();

	static std::vector<char> read_shader_file(const std::string& filename); 
	VkShaderModule init_ShaderModule(const std::vector<char>& code);
	VkSurfaceFormatKHR get_SwapSurface_Format();
	uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);
	void init_Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copy_Buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void cleanup_Swapchain();
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
	VkBuffer m_IndexBuffer;
	VkDeviceMemory m_IndexBufferMemory;
	std::vector<VkCommandBuffer> m_CommandBuffers;
	std::vector <VkSemaphore> m_PresentSemaphores; 
	std::vector<VkSemaphore> m_SubmitSemaphores; 
	std::vector<VkFence> m_Fences; 
	std::vector<VkFence> imagesInFlight;

public:
	void onWindowResize(int width, int height);
};