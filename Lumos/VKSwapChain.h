#pragma once
#include "VK.h"
#include "VKTexture2D.h"

#include <vector>
#include <GLFW/glfw3.h>


namespace Lumos
{
	class VKSwapChain
	{
	public:
		VKSwapChain(uint32_t width, uint32_t height);
		~VKSwapChain();

		bool Init(bool vsync);
		bool Init(bool vsync, GLFWwindow* windowHandle);

		VkSurfaceKHR CreatePlatformSurface(VkInstance vkInstance, GLFWwindow* window);


		size_t GetSwapChainBufferCount() const { return m_SwapChainBufferCount; }
		VKTexture2D* GetImage(uint32_t index) { return m_SwapChainBuffers[index]; }
		VkSurfaceKHR GetSurface() const { return m_Surface; }
		VkSwapchainKHR GetSwapChain() const { return m_SwapChain; }
		VkFormat GetScreenFormat() const { return m_ColourFormat; }

	private:

		void FindImageFormatAndColourSpace();

		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_SwapChainBufferCount;
		bool m_VSyncEnabled = false;

		VkSwapchainKHR m_SwapChain;
		VkSwapchainKHR m_OldSwapChain;
		VkSurfaceKHR m_Surface;
		VkFormat m_ColourFormat;
		VkColorSpaceKHR m_ColourSpace;

		std::vector<VKTexture2D*> m_SwapChainBuffers;
	};
}
