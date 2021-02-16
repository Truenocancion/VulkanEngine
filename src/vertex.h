#pragma once

#include <vulkan.h>
#include <array>

struct Vertex {
	float position[2]; // change to 3 later for 3D
	float color[3]; // if not working look at glm

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		// confusingly - formats here are specified using same enum as color
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		// vec2: VK_FORMAT_R32G32_SFLOAT
		// vec3 : VK_FORMAT_R32G32B32_SFLOAT
		attributeDescriptions[0].offset = offsetof(Vertex, position);
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		
		return attributeDescriptions;
	}
};
