#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Triangle.h"
#include "VulkanHelper.h"

void Model::Load(const char* Path)
{
	LoadModel(Path);
	createVertexBuffer();
	createIndexBuffer();
}

void Model::Destroy()
{
	vkDestroyBuffer(m_Editor->device, indexBuffer, nullptr);
	vkFreeMemory(m_Editor->device, indexBufferMemory, nullptr);

	vkDestroyBuffer(m_Editor->device, vertexBuffer, nullptr);
	vkFreeMemory(m_Editor->device, vertexBufferMemory, nullptr);
}

void Model::Render(VkCommandBuffer commandBuffer)
{
	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

void Model::LoadModel(const char* Path)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, Path)) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}
}

/// <summary>
/// 创建顶点缓冲区
/// </summary>
void Model::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	//VK_BUFFER_USAGE_TRANSFER_SRC_BIT：缓冲区可以用作内存传输操作中的源。
	VulkanHelper::createBuffer(m_Editor->physicalDevice, m_Editor->device, bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory);

	void* data;
	vkMapMemory(m_Editor->device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_Editor->device, stagingBufferMemory);

	//VK_BUFFER_USAGE_TRANSFER_DST_BIT：缓冲区可用作内存传输操作中的目标。
	VulkanHelper::createBuffer(m_Editor->physicalDevice, m_Editor->device, bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer,
		vertexBufferMemory);
	// 数据从暂存缓冲区复制到设备缓冲区
	VulkanHelper::copyBuffer(m_Editor->device, m_Editor->commandPool, m_Editor->graphicsQueue, stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(m_Editor->device, stagingBuffer, nullptr);
	vkFreeMemory(m_Editor->device, stagingBufferMemory, nullptr);
}

/// <summary>
/// 创建顶点索引缓冲区
/// </summary>
void Model::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanHelper::createBuffer(m_Editor->physicalDevice, m_Editor->device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_Editor->device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_Editor->device, stagingBufferMemory);

	VulkanHelper::createBuffer(m_Editor->physicalDevice, m_Editor->device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	VulkanHelper::copyBuffer(m_Editor->device, m_Editor->commandPool, m_Editor->graphicsQueue, stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(m_Editor->device, stagingBuffer, nullptr);
	vkFreeMemory(m_Editor->device, stagingBufferMemory, nullptr);
}