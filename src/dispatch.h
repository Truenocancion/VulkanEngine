#pragma once

struct SDispatch {
	PFN_vkGetInstanceProcAddr GetInstanceProcAddr;
	PFN_vkCreateInstance CreateInstance;
	
	PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
	PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
	PFN_vkCreateDevice vkCreateDevice;
	PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
	PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
	PFN_vkDestroyInstance vkDestroyInstance; 

	PFN_vkGetDeviceQueue vkGetDeviceQueue;
	PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
	PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
	PFN_vkCreateImageView vkCreateImageView;
	PFN_vkCreateRenderPass vkCreateRenderPass;
	PFN_vkCreateShaderModule vkCreateShaderModule;
	PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
	PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
	PFN_vkCreateFramebuffer vkCreateFramebuffer;
	PFN_vkCreateCommandPool vkCreateCommandPool;
	PFN_vkCreateBuffer vkCreateBuffer;
	PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
	PFN_vkAllocateMemory vkAllocateMemory;
	PFN_vkBindBufferMemory vkBindBufferMemory;
	PFN_vkMapMemory vkMapMemory;
	PFN_vkUnmapMemory vkUnmapMemory;
	PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
	PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
	PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
	PFN_vkCmdBindPipeline vkCmdBindPipeline;
	PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
	PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
	PFN_vkCmdDraw vkCmdDraw;
	PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
	PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
	PFN_vkEndCommandBuffer vkEndCommandBuffer;
	PFN_vkCreateSemaphore vkCreateSemaphore;
	PFN_vkCreateFence vkCreateFence;
	PFN_vkWaitForFences vkWaitForFences;
	PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
	PFN_vkResetFences vkResetFences;
	PFN_vkQueueSubmit vkQueueSubmit;
	PFN_vkQueuePresentKHR vkQueuePresentKHR;
	PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
	PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
	PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
	PFN_vkDestroyPipeline vkDestroyPipeline;
	PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
	PFN_vkDestroyRenderPass vkDestroyRenderPass;
	PFN_vkDestroyImageView vkDestroyImageView;
	PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
	PFN_vkDestroyBuffer vkDestroyBuffer;
	PFN_vkFreeMemory vkFreeMemory;
	PFN_vkDestroyFence vkDestroyFence;
	PFN_vkDestroySemaphore vkDestroySemaphore;
	PFN_vkDestroyCommandPool vkDestroyCommandPool;
	PFN_vkDestroyShaderModule vkDestroyShaderModule;
	PFN_vkDestroyDevice vkDestroyDevice;

	PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
	PFN_vkQueueWaitIdle vkQueueWaitIdle;

	void InitGlobal() {
		CreateInstance = (PFN_vkCreateInstance)GetInstanceProcAddr(nullptr, "vkCreateInstance");
	};

	void InitInstance(VkInstance instance) {
#define I(name) name = (PFN_##name)GetInstanceProcAddr(instance, #name) 

		I(vkEnumeratePhysicalDevices);
		I(vkGetPhysicalDeviceQueueFamilyProperties);
		I(vkCreateDevice);
		I(vkGetDeviceProcAddr);
		I(vkGetPhysicalDeviceSurfaceSupportKHR);
		I(vkGetPhysicalDeviceSurfaceFormatsKHR);
		I(vkGetPhysicalDeviceMemoryProperties);
		I(vkDestroyInstance);
		I(vkDestroySurfaceKHR);
	}

	void InitDevice(VkDevice device) {
#define D(name) name = (PFN_##name)vkGetDeviceProcAddr(device, #name) 

		D(vkGetDeviceQueue);
		D(vkCreateSwapchainKHR);
		D(vkGetSwapchainImagesKHR);
		D(vkCreateImageView);
		D(vkCreateRenderPass);
		D(vkCreateShaderModule);
		D(vkCreatePipelineLayout);
		D(vkCreateGraphicsPipelines);
		D(vkCreateFramebuffer);
		D(vkCreateCommandPool);
		D(vkCreateBuffer);
		D(vkGetBufferMemoryRequirements);
		D(vkAllocateMemory);
		D(vkBindBufferMemory);
		D(vkMapMemory);
		D(vkUnmapMemory);
		D(vkAllocateCommandBuffers);
		D(vkBeginCommandBuffer);
		D(vkCmdBeginRenderPass);
		D(vkCmdBindPipeline);
		D(vkCmdBindVertexBuffers);
		D(vkCmdBindIndexBuffer);
		D(vkCmdDraw);
		D(vkCmdDrawIndexed);
		D(vkCmdEndRenderPass);
		D(vkEndCommandBuffer);
		D(vkCreateSemaphore);
		D(vkCreateFence);
		D(vkWaitForFences);
		D(vkAcquireNextImageKHR);
		D(vkResetFences);
		D(vkQueueSubmit);
		D(vkQueuePresentKHR);
		D(vkDeviceWaitIdle);
		D(vkDestroyFramebuffer);
		D(vkFreeCommandBuffers);
		D(vkDestroyPipeline);
		D(vkDestroyPipelineLayout);
		D(vkDestroyRenderPass);
		D(vkDestroyImageView);
		D(vkDestroySwapchainKHR);
		D(vkDestroyBuffer);
		D(vkFreeMemory);
		D(vkDestroyFence);
		D(vkDestroySemaphore);
		D(vkDestroyCommandPool);
		D(vkDestroyShaderModule); 
		D(vkDestroyDevice);

		D(vkCmdCopyBuffer);
		D(vkQueueWaitIdle);
	}
};