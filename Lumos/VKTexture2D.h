#pragma once
#include "VK.h"

namespace Lumos
{
	class VKTexture2D
	{
	public:
		VKTexture2D(VkImage image, VkImageView imageView, VkFormat format, uint32_t width, uint32_t height);

		~VKTexture2D();

		void UpdateDescriptor();
		//void TransitionImage(VkImageLayout newLayout, VKCommandBuffer* commandBuffer = nullptr);

		VkImageView GetImageView() const { return m_TextureImageView; }
	private:
		uint32_t m_Width{}, m_Height{};
		uint32_t m_MipLevels = 1;
		uint8_t* m_Data = nullptr;

		VkFormat m_VKFormat = VK_FORMAT_R8G8B8A8_UNORM;

		VkImage m_TextureImage{};
		VkImageLayout m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkDeviceMemory m_TextureImageMemory{};
		VkImageView m_TextureImageView;
		VkSampler m_TextureSampler{};
		VkDescriptorImageInfo m_Descriptor{};

		bool m_DeleteImage = true;
	};
}