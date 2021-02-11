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
	PFN_vkCreateImageView vkCreateImageView; 
	PFN_vkDestroyImageView vkDestroyImageView; 
	PFN_vkCreateShaderModule vkCreateShaderModule; 
	PFN_vkDestroyShaderModule vkDestroyShaderModule; 
	PFN_vkCreateRenderPass vkCreateRenderPass;
	PFN_vkDestroyRenderPass vkDestroyRenderPass;
	PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
	PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
	PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
	PFN_vkDestroyPipeline vkDestroyPipeline;
	PFN_vkCreateFramebuffer vkCreateFramebuffer;
	PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
	PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
	PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
	PFN_vkCmdBindPipeline vkCmdBindPipeline;
	PFN_vkCmdDraw vkCmdDraw;
	PFN_vkGetDeviceQueue vkGetDeviceQueue;
	PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
	PFN_vkQueueWaitIdle vkQueueWaitIdle;
	PFN_vkQueuePresentKHR vkQueuePresentKHR;
	PFN_vkCreateSemaphore vkCreateSemaphore;
	PFN_vkDestroySemaphore vkDestroySemaphore;
	PFN_vkCreateFence vkCreateFence;
	PFN_vkDestroyFence vkDestroyFence;
	PFN_vkWaitForFences vkWaitForFences;
	PFN_vkResetFences vkResetFences;
	PFN_vkCreateCommandPool vkCreateCommandPool;
	PFN_vkDestroyCommandPool vkDestroyCommandPool;
	PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
	PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
	PFN_vkEndCommandBuffer vkEndCommandBuffer;
	PFN_vkResetCommandBuffer vkResetCommandBuffer;
	PFN_vkCmdClearColorImage vkCmdClearColorImage;
	PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
	PFN_vkQueueSubmit vkQueueSubmit;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;

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
		I(vkGetPhysicalDeviceSurfaceFormatsKHR);
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
		D(vkCreateImageView); 
		D(vkDestroyImageView); 
		D(vkCreateShaderModule); 
		D(vkDestroyShaderModule); 
		D(vkCreateRenderPass);
		D(vkDestroyRenderPass);
		D(vkCreatePipelineLayout);
		D(vkDestroyPipelineLayout);
		D(vkCreateGraphicsPipelines);
		D(vkDestroyPipeline);
		D(vkCreateFramebuffer);
		D(vkDestroyFramebuffer);
		D(vkCmdBeginRenderPass);
		D(vkCmdEndRenderPass);
		D(vkCmdBindPipeline);
		D(vkCmdDraw);
		D(vkCreateFence);
		D(vkDestroyFence);
		D(vkWaitForFences);
		D(vkResetFences);
	}
};