#pragma once

struct SDispatch {
	PFN_vkGetInstanceProcAddr GetInstanceProcAddr;
	PFN_vkCreateInstance CreateInstance;

	PFN_vkDestroyInstance vkDestroyInstance; 
	PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
	PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
	PFN_vkCreateDevice vkCreateDevice;
	PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
	PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;

	PFN_vkDestroyDevice vkDestroyDevice;
	PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
	PFN_vkGetDeviceQueue vkGetDeviceQueue;
	PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
	PFN_vkQueueWaitIdle vkQueueWaitIdle;
	PFN_vkQueuePresentKHR vkQueuePresentKHR;
	PFN_vkCreateSemaphore vkCreateSemaphore;
	PFN_vkDestroySemaphore vkDestroySemaphore;
	PFN_vkCreateCommandPool vkCreateCommandPool;
	PFN_vkDestroyCommandPool vkDestroyCommandPool;
	PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
	PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
	PFN_vkEndCommandBuffer vkEndCommandBuffer;
	PFN_vkResetCommandBuffer vkResetCommandBuffer;
	PFN_vkCmdClearColorImage vkCmdClearColorImage;
	PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
	PFN_vkQueueSubmit vkQueueSubmit;

	void InitGlobal() {
		CreateInstance = (PFN_vkCreateInstance)GetInstanceProcAddr(nullptr, "vkCreateInstance");
	};

	void InitInstance(VkInstance instance) {
#define I(name) name = (PFN_##name)GetInstanceProcAddr(instance, #name) 

		I(vkDestroyInstance); 
		I(vkEnumeratePhysicalDevices);
		I(vkGetPhysicalDeviceProperties);
		I(vkGetPhysicalDeviceQueueFamilyProperties);
		I(vkCreateDevice);
		I(vkGetDeviceProcAddr);
		I(vkDestroySurfaceKHR);
		I(vkGetPhysicalDeviceSurfaceSupportKHR);
	}

	void InitDevice(VkDevice device) {
#define D(name) name = (PFN_##name)vkGetDeviceProcAddr(device, #name) 

		D(vkDestroyDevice);
		D(vkCreateSwapchainKHR);
		D(vkDestroySwapchainKHR);
		D(vkGetSwapchainImagesKHR);
		D(vkGetDeviceQueue);
		D(vkAcquireNextImageKHR);
		D(vkQueueWaitIdle);
		D(vkQueuePresentKHR);
		D(vkCreateSemaphore);
		D(vkDestroySemaphore);
		D(vkCreateCommandPool);
		D(vkDestroyCommandPool);
		D(vkAllocateCommandBuffers);
		D(vkBeginCommandBuffer);
		D(vkEndCommandBuffer);
		D(vkResetCommandBuffer);
		D(vkCmdClearColorImage);
		D(vkCmdPipelineBarrier);
		D(vkQueueSubmit);
	}
};