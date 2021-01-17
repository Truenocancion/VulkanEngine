#include "render.h"
#include "engine.h"
#include "viewport.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"

CRender::CRender() {
	SDL_Vulkan_LoadLibrary(nullptr);
	unsigned instanceExtensionCount;
	SDL_Vulkan_GetInstanceExtensions(gEngine->GetViewport()->m_Window, &instanceExtensionCount, nullptr);

	std::vector<const char*> instanceExtensions;
	instanceExtensions.resize(instanceExtensionCount);
	SDL_Vulkan_GetInstanceExtensions(gEngine->GetViewport()->m_Window, &instanceExtensionCount, instanceExtensions.data());
	
	m_Dispatch.GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)SDL_Vulkan_GetVkGetInstanceProcAddr();
	m_Dispatch.InitGlobal();

	// creating instance:
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "My Vulkan engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo instanceInfo{};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;
#ifdef _DEBUG 
	instanceInfo.enabledLayerCount = 1;
	const char* layers[] = {"VK_LAYER_KHRONOS_validation"};
	instanceInfo.ppEnabledLayerNames = layers;
#endif
	instanceInfo.enabledExtensionCount = instanceExtensions.size();
	instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();

	VkResult result = m_Dispatch.CreateInstance(&instanceInfo, nullptr, &m_Instance);
	if (result != VK_SUCCESS) abort(); 
	m_Dispatch.InitInstance(m_Instance);

	uint32_t physicalDeviceCount;
	result = m_Dispatch.vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);
	if (result != VK_SUCCESS) abort();
	if (physicalDeviceCount == 0) abort();

	std::vector<VkPhysicalDevice> physicalDevices;
	physicalDevices.resize(physicalDeviceCount);

	result = m_Dispatch.vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, physicalDevices.data());
	if (result != VK_SUCCESS) abort();

	m_PhysicalDevice = physicalDevices[0];

	VkPhysicalDeviceProperties properties;
	m_Dispatch.vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

	uint32_t queueFamilyCount;
	m_Dispatch.vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> familyProperties;
	familyProperties.resize(queueFamilyCount);

	m_Dispatch.vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, familyProperties.data());

	for (uint32_t i = 0; i < queueFamilyCount; i++) {
		VkQueueFamilyProperties& props = familyProperties[i];
		if ((props.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (props.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
			m_GraphicsQueueFamily = i;
			break;
		}
	}
	if (m_GraphicsQueueFamily == -1) abort();

	if (SDL_Vulkan_CreateSurface(gEngine->GetViewport()->m_Window, m_Instance, &m_Surface) != SDL_TRUE) {
		abort();
	}

	VkBool32 bSurfaceSupported;
	if (m_Dispatch.vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, m_GraphicsQueueFamily, m_Surface, &bSurfaceSupported) != VK_SUCCESS) {
		abort();
	}
	if (bSurfaceSupported != VK_TRUE) abort();

	float qPriorities[] = { 0.5 };
	VkDeviceQueueCreateInfo queueCreateInfo;
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext = nullptr;
	queueCreateInfo.flags = 0;
	queueCreateInfo.queueFamilyIndex = m_GraphicsQueueFamily;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = qPriorities;

	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	result = m_Dispatch.vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device);
	if (result != VK_SUCCESS) abort();

	m_Dispatch.InitDevice(m_Device);

	VkSwapchainCreateInfoKHR scCreateInfo{};
	scCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	scCreateInfo.surface = m_Surface;
	scCreateInfo.minImageCount = 2;
	scCreateInfo.imageFormat = kBackBufferFormat;
	scCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	scCreateInfo.imageExtent.width = kWindowWidth;
	scCreateInfo.imageExtent.height = kWindowHeight;
	scCreateInfo.imageArrayLayers = 1;
	scCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	scCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	scCreateInfo.queueFamilyIndexCount = 1;
	scCreateInfo.pQueueFamilyIndices = (uint32_t*)&m_GraphicsQueueFamily;
	scCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	scCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	scCreateInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	scCreateInfo.clipped = VK_TRUE;
	scCreateInfo.oldSwapchain = nullptr;

	result = m_Dispatch.vkCreateSwapchainKHR(m_Device, &scCreateInfo, nullptr, &m_Swapchain);
	if (result != VK_SUCCESS) abort();

	uint32_t scImageCount;
	m_Dispatch.vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &scImageCount, nullptr);
	m_ScImages.resize(scImageCount);
	m_Dispatch.vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &scImageCount, m_ScImages.data());

	m_Dispatch.vkGetDeviceQueue(m_Device, m_GraphicsQueueFamily, 0, &m_Queue);

	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	m_Dispatch.vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_PresentSemaphore);
	m_Dispatch.vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_SubmitSemaphore);

	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.queueFamilyIndex = m_GraphicsQueueFamily;

	m_Dispatch.vkCreateCommandPool(m_Device, &commandPoolCreateInfo, nullptr, &m_CommandPool);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = m_CommandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;

	m_Dispatch.vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, &m_ClearCmd);
}

CRender::~CRender() {
	m_Dispatch.vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
	m_Dispatch.vkDestroySemaphore(m_Device, m_PresentSemaphore, nullptr);
	m_Dispatch.vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
	m_Dispatch.vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	m_Dispatch.vkDestroyDevice(m_Device, nullptr);
	m_Dispatch.vkDestroyInstance(m_Instance, nullptr);
	SDL_Vulkan_UnloadLibrary();
}

void CRender::Update() {
	uint32_t nextImage;
	m_Dispatch.vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, m_SubmitSemaphore, nullptr, &nextImage);

	RecordClear();

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &m_SubmitSemaphore;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_ClearCmd;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_PresentSemaphore;
	submitInfo.pWaitDstStageMask = waitStages;

	m_Dispatch.vkQueueSubmit(m_Queue, 1, &submitInfo, nullptr);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_PresentSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_Swapchain;
	presentInfo.pImageIndices = &nextImage;
	
	m_Dispatch.vkQueuePresentKHR(m_Queue, &presentInfo);
	m_Dispatch.vkQueueWaitIdle(m_Queue);

	m_bInitialFrame = false;
}

void CRender::RecordClear(){
	m_Dispatch.vkResetCommandBuffer(m_ClearCmd, 0);
	
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	m_Dispatch.vkBeginCommandBuffer(m_ClearCmd, &beginInfo);

	if (m_bInitialFrame) {
		VkImageMemoryBarrier initialBarrier{};
		initialBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		initialBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		initialBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
		initialBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		initialBarrier.subresourceRange.levelCount = 1;
		initialBarrier.subresourceRange.layerCount = 1;

		for (VkImage image : m_ScImages) {
			initialBarrier.image = image;

			m_Dispatch.vkCmdPipelineBarrier(m_ClearCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &initialBarrier);
		}
	}

	m_Dispatch.vkEndCommandBuffer(m_ClearCmd);
}
