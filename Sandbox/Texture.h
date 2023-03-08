#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "EditorPanel.h"

class Texture :public EditorPanel
{
public:
	VkDescriptorSet descriptorSet;
	void Load(const char* path);
	void Free();
	void Destroy();

private:
	void createTextureImage(const char* path);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	void createTextureImageView();
	void createTextureSampler();
	void createTextureDescriptorSet();

private:
	uint32_t mipLevels;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
};