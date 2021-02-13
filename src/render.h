#pragma once

#define VK_NO_PROTOTYPES 1
#include <vulkan.h>
#include "dispatch.h"
#include <vector>
#include <fstream> 

const VkFormat kBackBufferFormat = VK_FORMAT_R8G8B8A8_SRGB;
const int MAX_FRAMES_IN_FLIGHT = 2;

class CRender {

	friend class CEngine;

	CRender();
	~CRender();

	void Update();
	void RecordClear(); // may not need in the future

	void init_Instance();
	void init_PhysicalDevice();
	void init_Device();
	void init_Surface();
	void init_SwapChain();
	void init_ImageViews();
	void init_RenderPass();
	void init_GraphicsPipeline();
	void init_Framebuffers();
	void init_CommandPool();
	void init_CommandBuffers();
	void init_SyncObjects();

	static std::vector<char> readFile(const std::string& filename); 
	VkShaderModule init_ShaderModule(const std::vector<char>& code);
	VkSurfaceFormatKHR getSwapSurfaceFormat();

	size_t currentFrame = 0;
	SDispatch m_Dispatch{};

	VkInstance m_Instance; 
	VkPhysicalDevice m_PhysicalDevice; 
	int m_GraphicsQueueFamily = -1; 
	VkDevice m_Device; 
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
	VkQueue m_Queue; 
	std::vector<VkSemaphore> m_SubmitSemaphores; 
	std::vector <VkSemaphore> m_PresentSemaphores; 
	std::vector<VkFence> m_Fences; 
	std::vector<VkFence> imagesInFlight; 
	VkCommandPool m_CommandPool;
	std::vector<VkCommandBuffer> m_CommandBuffers; 
};