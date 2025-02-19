#include "render.h"
#include "engine.h"
#include "viewport.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"

VulkanRender::VulkanRender() {
	SDL_Vulkan_LoadLibrary(nullptr);
	init_Instance();
	init_PhysicalDevice();
	init_Device();
	init_Surface();
	init_Swapchain();
	init_ImageViews();
	init_RenderPass();
	init_GraphicsPipeline();
	init_Framebuffers();
	init_CommandPool();
	init_VertexBuffer();
	init_IndexBuffer();
	init_CommandBuffers();
	init_SyncObjects();
}

void VulkanRender::init_Instance() {
	unsigned instanceExtensionCount;
	SDL_Vulkan_GetInstanceExtensions(gEngine->GetViewport()->m_Window, &instanceExtensionCount, nullptr);

	std::vector<const char*> instanceExtensions;
	instanceExtensions.resize(instanceExtensionCount);
	SDL_Vulkan_GetInstanceExtensions(gEngine->GetViewport()->m_Window, &instanceExtensionCount, instanceExtensions.data());

	m_Dispatch.GetInstanceProcAddr = static_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr());
	m_Dispatch.InitGlobal();

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Vulkan engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo instanceInfo{};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;
#ifdef _DEBUG 
	instanceInfo.enabledLayerCount = 1;
	const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
	instanceInfo.ppEnabledLayerNames = layers;
#endif
	instanceInfo.enabledExtensionCount = instanceExtensions.size();
	instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();

	if (m_Dispatch.CreateInstance(&instanceInfo, nullptr, &m_Instance) != VK_SUCCESS) abort();
	m_Dispatch.InitInstance(m_Instance);
}

void VulkanRender::init_PhysicalDevice() {
	uint32_t physicalDeviceCount;
	if ((m_Dispatch.vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr) != VK_SUCCESS) || (physicalDeviceCount == 0)) abort();

	std::vector<VkPhysicalDevice> physicalDevices;
	physicalDevices.resize(physicalDeviceCount);

	if (m_Dispatch.vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, physicalDevices.data()) != VK_SUCCESS) abort();

	m_PhysicalDevice = physicalDevices[0];

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
}

void VulkanRender::init_Device() {
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
	deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (m_Dispatch.vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device) != VK_SUCCESS) abort();

	m_Dispatch.InitDevice(m_Device);

	m_Dispatch.vkGetDeviceQueue(m_Device, m_GraphicsQueueFamily, 0, &m_Queue);
}

void VulkanRender::init_Surface() {
	if (SDL_Vulkan_CreateSurface(gEngine->GetViewport()->m_Window, m_Instance, &m_Surface) != SDL_TRUE) abort();

	VkBool32 bSurfaceSupported;
	if (m_Dispatch.vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, m_GraphicsQueueFamily, m_Surface, &bSurfaceSupported) != VK_SUCCESS) abort();
	if (bSurfaceSupported != VK_TRUE) abort();

	m_Width = kWindowWidth;
	m_Height = kWindowHeight;
}

void VulkanRender::init_Swapchain() {
	VkSwapchainCreateInfoKHR scCreateInfo{};
	scCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	scCreateInfo.surface = m_Surface;
	scCreateInfo.minImageCount = 2;
	scCreateInfo.imageFormat = get_SwapSurface_Format().format;
	scCreateInfo.imageColorSpace = get_SwapSurface_Format().colorSpace;
	scCreateInfo.imageExtent.width = m_Width;
	scCreateInfo.imageExtent.height = m_Height;
	scCreateInfo.imageArrayLayers = 1;
	scCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	scCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	scCreateInfo.queueFamilyIndexCount = 1;
	scCreateInfo.pQueueFamilyIndices = reinterpret_cast<uint32_t*>(&m_GraphicsQueueFamily);
	scCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	scCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	scCreateInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	scCreateInfo.clipped = VK_TRUE;

	if (m_Dispatch.vkCreateSwapchainKHR(m_Device, &scCreateInfo, nullptr, &m_Swapchain) != VK_SUCCESS) abort();
}

void VulkanRender::init_ImageViews() {
	uint32_t scImageCount;
	m_Dispatch.vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &scImageCount, nullptr);
	m_ScImages.resize(scImageCount);
	m_Dispatch.vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &scImageCount, m_ScImages.data());
 
	m_ScImageViews.resize(scImageCount); 

	for (int i = 0; i < scImageCount; i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_ScImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = get_SwapSurface_Format().format;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.layerCount = 1;

		if (m_Dispatch.vkCreateImageView(m_Device, &createInfo, nullptr, &m_ScImageViews[i]) != VK_SUCCESS) abort();
	}
}

void VulkanRender::init_RenderPass() {
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = get_SwapSurface_Format().format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (m_Dispatch.vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) abort();
}

void VulkanRender::init_GraphicsPipeline() {
	std::vector<char> vertShaderCode = read_shader_file("src/shaders/vert.spv");
	std::vector<char> fragShaderCode = read_shader_file("src/shaders/frag.spv");

	m_VertShader = init_ShaderModule(vertShaderCode);
	m_FragShader = init_ShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = m_VertShader;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = m_FragShader;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();
	
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_Width);
	viewport.height = static_cast<float>(m_Height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) };

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = m_MsaaSamples;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

	if (m_Dispatch.vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) abort();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = m_PipelineLayout;
	pipelineInfo.renderPass = m_RenderPass;
	pipelineInfo.subpass = 0;

	if (m_Dispatch.vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS) abort();
}

void VulkanRender::init_Framebuffers() {
	m_Framebuffers.resize(m_ScImageViews.size());

	for (size_t i = 0; i < m_ScImageViews.size(); i++) {
		VkImageView attachments[] = { m_ScImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_Width;
		framebufferInfo.height = m_Height;
		framebufferInfo.layers = 1;

		if (m_Dispatch.vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS) abort();
	}
}

void VulkanRender::init_CommandPool() {
	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.queueFamilyIndex = m_GraphicsQueueFamily;

	if (m_Dispatch.vkCreateCommandPool(m_Device, &commandPoolCreateInfo, nullptr, &m_CommandPool) != VK_SUCCESS) abort();
}

void VulkanRender::init_VertexBuffer(){
	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	
	init_Buffer(bufferSize, 
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
				stagingBuffer,
				stagingBufferMemory);
	
	void* data;
	m_Dispatch.vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Vertices.data(), static_cast<size_t>(bufferSize));
	m_Dispatch.vkUnmapMemory(m_Device, stagingBufferMemory);

	init_Buffer(bufferSize, 
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
				m_VertexBuffer, 
				m_VertexBufferMemory);

	copy_Buffer(stagingBuffer, m_VertexBuffer, bufferSize);

	m_Dispatch.vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
	m_Dispatch.vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
}

void VulkanRender::init_IndexBuffer(){
	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	
	init_Buffer(bufferSize, 
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
				stagingBuffer, 
				stagingBufferMemory);

	void* data;
	m_Dispatch.vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Indices.data(), static_cast<size_t>(bufferSize));
	m_Dispatch.vkUnmapMemory(m_Device, stagingBufferMemory);

	init_Buffer(bufferSize, 
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_IndexBuffer,
				m_IndexBufferMemory);

	copy_Buffer(stagingBuffer, m_IndexBuffer, bufferSize);

	m_Dispatch.vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
	m_Dispatch.vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
}

void VulkanRender::init_CommandBuffers() {
	m_CommandBuffers.resize(m_Framebuffers.size());

	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = m_CommandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = m_CommandBuffers.size();

	if (m_Dispatch.vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, m_CommandBuffers.data()) != VK_SUCCESS) abort();

	for (size_t i = 0; i < m_CommandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (m_Dispatch.vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS) abort();

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		 
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = m_RenderPass;
		renderPassBeginInfo.framebuffer = m_Framebuffers[i];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		m_Dispatch.vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		m_Dispatch.vkCmdBindPipeline(m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
		
		VkDeviceSize offsets[] = { 0 };
		m_Dispatch.vkCmdBindVertexBuffers(m_CommandBuffers[i], 0, 1, &m_VertexBuffer, offsets);

		m_Dispatch.vkCmdBindIndexBuffer(m_CommandBuffers[i], m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
		
		m_Dispatch.vkCmdDrawIndexed(m_CommandBuffers[i], static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
		
		m_Dispatch.vkCmdEndRenderPass(m_CommandBuffers[i]);

		if (m_Dispatch.vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS) abort();
	}
}

void VulkanRender::init_SyncObjects() {
	m_PresentSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_SubmitSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_Fences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(m_ScImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (m_Dispatch.vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_PresentSemaphores[i]) != VK_SUCCESS) abort();
		if (m_Dispatch.vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_SubmitSemaphores[i]) != VK_SUCCESS) abort();
		if (m_Dispatch.vkCreateFence(m_Device, &fenceInfo, nullptr, &m_Fences[i]) != VK_SUCCESS) abort();
	}
}

VulkanRender::~VulkanRender() {
	m_Dispatch.vkDeviceWaitIdle(m_Device);
	
	cleanup_Swapchain();

	m_Dispatch.vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
	m_Dispatch.vkFreeMemory(m_Device, m_IndexBufferMemory, nullptr);
	
	m_Dispatch.vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
	m_Dispatch.vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);
	
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_Dispatch.vkDestroyFence(m_Device, m_Fences[i], nullptr);
		m_Dispatch.vkDestroySemaphore(m_Device, m_SubmitSemaphores[i], nullptr);
		m_Dispatch.vkDestroySemaphore(m_Device, m_PresentSemaphores[i], nullptr);
	}

	m_Dispatch.vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
	
	m_Dispatch.vkDestroyShaderModule(m_Device, m_FragShader, nullptr);
	m_Dispatch.vkDestroyShaderModule(m_Device, m_VertShader, nullptr);
	
	m_Dispatch.vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	m_Dispatch.vkDestroyDevice(m_Device, nullptr);
	m_Dispatch.vkDestroyInstance(m_Instance, nullptr);
	SDL_Vulkan_UnloadLibrary();
}

void VulkanRender::Update() {
	m_Dispatch.vkWaitForFences(m_Device, 1, &m_Fences[currentFrame], VK_TRUE, UINT64_MAX);
	
	uint32_t nextImage;
	VkResult result = m_Dispatch.vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, m_PresentSemaphores[currentFrame], nullptr, &nextImage);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		reinit_Swapchain();
		return;
	}
	
	if (imagesInFlight[nextImage] != VK_NULL_HANDLE) {
		m_Dispatch.vkWaitForFences(m_Device, 1, &imagesInFlight[nextImage], VK_TRUE, UINT64_MAX);
	}

	imagesInFlight[nextImage] = m_Fences[currentFrame];

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &m_PresentSemaphores[currentFrame];
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffers[nextImage];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_SubmitSemaphores[currentFrame];
	submitInfo.pWaitDstStageMask = waitStages;

	m_Dispatch.vkResetFences(m_Device, 1, &m_Fences[currentFrame]);
	
	m_Dispatch.vkQueueSubmit(m_Queue, 1, &submitInfo, m_Fences[currentFrame]);

	m_Dispatch.vkWaitForFences(m_Device, 1, &m_Fences[currentFrame], VK_TRUE, UINT64_MAX);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &m_SubmitSemaphores[currentFrame];
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &m_Swapchain;
	presentInfo.pImageIndices = &nextImage;
	
	m_Dispatch.vkQueuePresentKHR(m_Queue, &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

VkSurfaceFormatKHR VulkanRender::get_SwapSurface_Format() {
	uint32_t surfaceFormatsCount;
	if (m_Dispatch.vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatsCount, nullptr) != VK_SUCCESS) abort();

	std::vector<VkSurfaceFormatKHR> sSurfaceFormats;
	sSurfaceFormats.resize(surfaceFormatsCount);
	if (m_Dispatch.vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &surfaceFormatsCount, sSurfaceFormats.data()) != VK_SUCCESS) abort();

	for (int i = 0; i < surfaceFormatsCount; i++) {
		if ((sSurfaceFormats[i].colorSpace & VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) && (sSurfaceFormats[i].format & kBackBufferFormat)) {
			return sSurfaceFormats[i];
		}
	}
	return sSurfaceFormats[0];
}

std::vector<char> VulkanRender::read_shader_file(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open()) abort();

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

VkShaderModule VulkanRender::init_ShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (m_Dispatch.vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) abort();

	return shaderModule;
}

uint32_t VulkanRender::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties){
	VkPhysicalDeviceMemoryProperties memoryProperties;
	m_Dispatch.vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if ((type_filter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
}

void VulkanRender::init_Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory){
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	m_Dispatch.vkCreateBuffer(m_Device, &bufferCreateInfo, nullptr, &buffer);

	VkMemoryRequirements memoryRequirements;
	m_Dispatch.vkGetBufferMemoryRequirements(m_Device, buffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocationInfo{};
	memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocationInfo.allocationSize = memoryRequirements.size;
	memoryAllocationInfo.memoryTypeIndex = find_memory_type(memoryRequirements.memoryTypeBits, properties);

	if (m_Dispatch.vkAllocateMemory(m_Device, &memoryAllocationInfo, nullptr, &bufferMemory) != VK_SUCCESS) abort();

	m_Dispatch.vkBindBufferMemory(m_Device, buffer, bufferMemory, 0);
}

void VulkanRender::copy_Buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size){
	VkCommandBufferAllocateInfo bufferAllocateInfo{};
	bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	bufferAllocateInfo.commandPool = m_CommandPool;
	bufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	m_Dispatch.vkAllocateCommandBuffers(m_Device, &bufferAllocateInfo, &commandBuffer);

	VkCommandBufferBeginInfo bufferBeginInfo{};
	bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	bufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	m_Dispatch.vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	
	m_Dispatch.vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	m_Dispatch.vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	m_Dispatch.vkQueueSubmit(m_Queue, 1, &submitInfo, VK_NULL_HANDLE);
	m_Dispatch.vkQueueWaitIdle(m_Queue);

	m_Dispatch.vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &commandBuffer);
}

void VulkanRender::cleanup_Swapchain(){
	for (auto framebuffer : m_Framebuffers) {
		m_Dispatch.vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
	}
	m_Dispatch.vkFreeCommandBuffers(m_Device, m_CommandPool, static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
	m_Dispatch.vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
	m_Dispatch.vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
	m_Dispatch.vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
	for (auto imageView : m_ScImageViews) {
		m_Dispatch.vkDestroyImageView(m_Device, imageView, nullptr);
	}
	m_Dispatch.vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
}

void VulkanRender::reinit_Swapchain(){
	m_Dispatch.vkDeviceWaitIdle(m_Device);

	cleanup_Swapchain();

	init_Swapchain();
	init_ImageViews();
	init_RenderPass();
	init_GraphicsPipeline();
	init_Framebuffers();
	init_CommandBuffers();
}

void VulkanRender::onWindowResize(int width, int height){
	m_Width = width;
	m_Height = height;
}
