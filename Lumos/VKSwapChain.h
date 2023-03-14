#pragma once
#include "VK.h"
#include "VKTexture2D.h"
#include "VKCommandBuffer.h"
#include "VKCommandPool.h"
#include "Reference.h"
#include <vector>
#include <GLFW/glfw3.h>

#define MAX_SWAPCHAIN_BUFFERS 3

namespace Lumos
{
	struct FrameData
	{
		VkSemaphore PresentSemaphore = VK_NULL_HANDLE;
		SharedPtr<VKCommandPool> CommandPool;
		SharedPtr<VKCommandBuffer> MainCommandBuffer;
	};

	class VKSwapChain
	{
	public:
		VKSwapChain(uint32_t width, uint32_t height);
		~VKSwapChain();

		bool Init(bool vsync);
		bool Init(bool vsync, GLFWwindow* windowHandle);
		void CreateFrameData();

		VkSurfaceKHR CreatePlatformSurface(VkInstance vkInstance, GLFWwindow* window);


		size_t GetSwapChainBufferCount() const { return m_SwapChainBufferCount; }
		VKTexture2D* GetImage(uint32_t index) { return m_SwapChainBuffers[index]; }
		VkSurfaceKHR GetSurface() const { return m_Surface; }
		VkSwapchainKHR GetSwapChain() const { return m_SwapChain; }
		VkFormat GetScreenFormat() const { return m_ColourFormat; }

		//TODO
		FrameData& GetCurrentFrameData(int index) { return m_Frames[index]; }

	private:
		FrameData m_Frames[MAX_SWAPCHAIN_BUFFERS];
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
