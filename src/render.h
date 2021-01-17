#pragma once

#define VK_NO_PROTOTYPES 1
#include <vulkan.h>
#include "dispatch.h"
#include <vector>

const VkFormat kBackBufferFormat = VK_FORMAT_R8G8B8A8_SRGB;

class CRender {

	friend class CEngine;

	CRender();
	~CRender();

	void Update();
	void RecordClear();

	bool m_bInitialFrame = true;
	SDispatch m_Dispatch{};

	VkInstance m_Instance;
	VkPhysicalDevice m_PhysicalDevice;
	int m_GraphicsQueueFamily = -1;
	VkDevice m_Device;
	VkSurfaceKHR m_Surface;
	VkSwapchainKHR m_Swapchain;
	std::vector<VkImage> m_ScImages;
	VkQueue m_Queue;
	VkSemaphore m_SubmitSemaphore;
	VkSemaphore m_PresentSemaphore;
	VkCommandPool m_CommandPool;
	VkCommandBuffer m_ClearCmd;
};