#include "VKTexture2D.h"

namespace Lumos
{
	VKTexture2D::VKTexture2D(VkImage image,VkImageView imageView,VkFormat format,uint32_t width,uint32_t height)
		:m_TextureImage(image)
		,m_TextureImageView(imageView)
		,m_TextureSampler(VK_NULL_HANDLE)
		,m_Width(width)
		,m_Height(height)
		,m_VKFormat(format)
		,m_DeleteImage(false)
		,m_ImageLayout(VK_IMAGE_LAYOUT_UNDEFINED)
	{
		m_TextureImageMemory = VK_NULL_HANDLE;

		UpdateDescriptor();
	}

	VKTexture2D::~VKTexture2D()
	{

	}

	void VKTexture2D::UpdateDescriptor()
	{
		m_Descriptor.sampler = m_TextureSampler;
		m_Descriptor.imageView = m_TextureImageView;
		m_Descriptor.imageLayout = m_ImageLayout;
	}


}