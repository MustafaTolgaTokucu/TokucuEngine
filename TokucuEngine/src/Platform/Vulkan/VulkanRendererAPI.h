#pragma once
#include "Tokucu/Renderer/RendererAPI.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vulkan/vulkan.h"
#include "optional"
#include <fstream>

//libs attached while vulkan implementation
#include <variant>

#include "Tokucu/Renderer/ObjectAttributes.h"

namespace Tokucu {
	static std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}
		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}
	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct ColorUniform {
		glm::vec3 color;
	};

	struct shadowUBO {
		glm::mat4 pl_lightSpaceMatrix[6];

	};

	struct lightIndexUBO {
		int lightIndex;
	};

	struct LightAttributes
	{
		alignas(16) glm::vec3 pl_color;
		alignas(16) glm::vec3 pl_position;
		alignas(16) glm::vec3 pl_ambient;
		alignas(16) glm::vec3 pl_diffuse;
		alignas(16) glm::vec3 pl_specular;
		alignas(16) glm::vec3 pl_viewpos;
		float pl_constant;
		float pl_linear;
		float pl_quadratic;
		float pl_pointlightNumber;
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct Pipeline {
		std::string name;
		char vertexShader[64];
		char fragmentShader[64];
		char geometryShader[64];
		std::vector<std::pair<VkDescriptorType, VkShaderStageFlagBits>> descriptorLayout;
		std::vector<std::pair<VkDescriptorBufferInfo*, VkBufferUsageFlagBits>> descriptionBufferInfo;  // Store actual structs, not pointers
		std::vector<VkDescriptorImageInfo*> descriptionSampleInfo;
		std::vector<VkDeviceSize> bufferSize;

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;

	};

	struct VulkanObject {
		std::string name;  // Unique name for the object
		std::vector<Vertex> vertexData;
		std::vector<uint32_t> indexData;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		VkVertexInputBindingDescription bindingDescriptions{};
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
		Pipeline* pipeline;

		std::optional<std::string> modelLocation;
		std::vector<std::pair<std::string, std::string>> textureLocations = { {"ambient", "assets/textures/default.jpg"},{"diffuse", "assets/textures/default.jpg"},{"specular", "assets/textures/default.jpg"},
			{"normal", "assets/textures/defaultNormal.jpg"} }; // Store multiple texture paths

		std::vector<std::vector<VkBuffer>> uniformBuffers;
		std::vector<std::vector<VkDeviceMemory>> uniformBuffersMemory;
		std::vector<std::vector<void*>> uniformBuffersMapped;

		std::vector<VkDescriptorSet> descriptorSets;
		struct Textures {
			VkImage ambient;
			VkDeviceMemory ambientMemory;
			VkImage diffuse;
			VkDeviceMemory diffuseMemory;
			VkImage specular;
			VkDeviceMemory specularMemory;
			VkImage mormal;
			VkDeviceMemory normalMemory;

			std::array<VkImageView, 7> imageViews;//ambient,diffuse,specular respectively
		} textures;
	};

	struct BufferData {
		VkBuffer buffer;
		VkDeviceMemory memory;
	};
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		VulkanRendererAPI();
		virtual void Init(const std::shared_ptr<Window>& window);
		virtual void SetClearColor(const glm::vec4& color);
		virtual void Clear();
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray);
		virtual void Render();
		virtual void Resize(const std::shared_ptr<Window>& window);
	private:
		////////////////////////////////////
		////VULKAN INITIALIZERS (TEMPORARY)
		//////////////////////////////////
		///VULKAN FUNCTIONS
		/////////////////////////////////
		void createInstance();
		void pickPhysicalDevice();
		bool checkValidationLayerSupport();
		void createSwapChain();
		void recreateSwapChain();
		void cleanupSwapChain();
		std::vector<const char*> getRequiredExtensions();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		void setupDebugMessenger();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, 
			VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
		bool isDeviceSuitable(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		void createLogicalDevice();
		void createSurface();
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void createImageViews();
		void createDescriptorSetLayout();
		void createGraphicsPipeline();
		void createRenderPass();
		VkShaderModule createShaderModule(const std::vector<char>& code);
		void createFramebuffers();
		void createCommandPool();
		void createDepthResources();
		void createTextureImage();
		void createTextureSampler();
		void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t layerCount);
		void createCommandBuffers();
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentImage);
		void createSyncObjects();
		//Model Loading with assimp. (FBX is set to default for now) 
		//ModelData loadModel(std::string modelLocation);
		BufferData createVertexBuffer(const std::vector<Vertex>& vertexData);
		BufferData createIndexBuffer(const std::vector<uint32_t>& intexData);
		void createUniformBuffers();
		void updateUniformBuffer(uint32_t currentImage);
		void createDescriptorPool();
		void createDescriptorSets();
		//we need to allocate memory for the buffer and choose rith memory type
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, int layerCount, uint32_t mipLevels, uint32_t baseMipLevel);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t baseArrayLayer);
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat findDepthFormat();
		bool hasStencilComponent(VkFormat format);
		void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
		VkSampleCountFlagBits getMaxUsableSampleCount();
		void createColorResources();
		////////////////////////////////////////////
		/////VULKAN CLASS MEMBERS
		////////////////////////////////////////////
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device;
		VkQueue graphicsQueue;
		VkSurfaceKHR surface;
		VkQueue presentQueue;

		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		std::vector<VkImageView> swapChainImageViews;
		VkRenderPass renderPass;
		VkRenderPass shadowRenderPass;
		VkRenderPass renderPassHDR;
		VkRenderPass BRDFRenderPass;

		std::vector<VkFramebuffer> swapChainFramebuffers;
		VkFramebuffer shadowFrameBuffer;
		VkFramebuffer HDRCubeFrameBuffer;
		VkFramebuffer CubeConvolutionFrameBuffer;
		VkFramebuffer BRDFFrameBuffer;

		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		const int MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t currentFrame = 0;
		bool framebufferResized = false;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		VkSampler textureSampler;
		VkSampler shadowSampler;
		VkSampler prefilterTextureSampler;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		uint32_t mipLevels = 1.0;
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

		VkImage colorImage;
		VkDeviceMemory colorImageMemory;
		VkImageView colorImageView;

		////////////////////////////////
		/////////////TOKUCU test functions & variables
		///////////////////////////////
		GLFWwindow* glfwWindow;

		std::vector<VulkanObject> Objects;
		std::vector<Pipeline*> Pipelines;

		Pipeline m_Pipeline;
		Pipeline m_Pipeline2;
		Pipeline m_PipelineSkybox;
		Pipeline m_PipelineShadow;
		Pipeline m_PipelineSkyboxHDR;
		Pipeline m_PipelineCubeConv;
		Pipeline m_PipelinePrefilter;
		Pipeline m_PipelineBRDF;

		VulkanObject target;
		VulkanObject realCube;

		std::vector<glm::vec3> cubePositions;
		void registerPipeline();
		void createObject();
		void createModel(std::string modelName, std::string modelLocation, std::string textureLocation);
		void createShadowImage();
		void createCubemapImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t layerCount);
		void createShadowFramebuffer();
		VkFramebuffer createPrefilterFrameBuffer(uint16_t width, uint16_t height, uint16_t miplevel);
		//void createSkyboxFramebuffers();
		VkImageView createShadowImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		VkImageView createCubemapImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t baseMipLevel);
		VkVertexInputBindingDescription getBindingDescription();
		std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();

		std::vector<Vertex> secondVertices;
		std::vector<uint32_t> secondIndices;

		std::vector<Vertex> cubeVertices;
		std::vector<uint32_t> cubeIndices;

		std::unordered_map<std::string, glm::mat4> objectTransformations; //Holding transformation information for each object
		std::unordered_map<std::string, glm::vec3> objectColor;

		LightAttributes pointLight1;
		LightAttributes pointLight2;
		std::vector<LightAttributes> pointLights = { pointLight1,pointLight2 };

		VkImage shadowImage;
		VkDeviceMemory shadowImageMemory;
		VkDeviceMemory skyboxImageMemory;
		VkDeviceMemory skyboxHDRImageMemory;
		VkDeviceMemory HDRCubeImageMemory;
		VkDeviceMemory CubeConvolutionImageMemory;
		VkDeviceMemory prefilterMapImageMemory;
		VkDeviceMemory BRDFImageMemory;

		VkImageView shadowImageView;
		VkImageView HDRCubeView;
		VkImageView prefilterMapView;
		VkImageView BRDFImageView;
		VkImageView skyboxImageView;
		VkImageView skyboxHDRImageView;
		VkImageView CubeConvolutionImageView;

		int shadowMapSize = 2048;
		VkImage skyboxImage;
		VkImage skyboxHDRImage;
		VkImage HDRCubeImage;
		VkImage CubeConvolutionImage;
		VkImage prefilterMapImage;
		VkImage BRDFImage;

		//Buffer infos
		VkDescriptorBufferInfo transformBufferInfo{};
		VkDescriptorBufferInfo colorBufferInfo{};
		VkDescriptorBufferInfo pointLightsInfo{};
		VkDescriptorBufferInfo shadowUBOInfo{};
		VkDescriptorBufferInfo lightIndexUBOInfo{};
		VkDescriptorBufferInfo cubemapPosInfo{};
		VkDescriptorBufferInfo cubemapPosMatInfo{};
		VkDescriptorBufferInfo prefilterPosInfo{};
		//Image infos
		VkDescriptorImageInfo ambientInfo{};
		VkDescriptorImageInfo diffuseInfo{};
		VkDescriptorImageInfo specularInfo{};
		VkDescriptorImageInfo shadowInfo{};
		VkDescriptorImageInfo normalInfo{};
		VkDescriptorImageInfo skyboxInfo{};
		VkDescriptorImageInfo cubeConvInfo{};
		VkDescriptorImageInfo skyboxHDRInfo{};
		VkDescriptorImageInfo irradianceInfo{};
		VkDescriptorImageInfo prefilterInfo{};
		VkDescriptorImageInfo prefilterMapInfo{};
		VkDescriptorImageInfo BRDFInfo{};
		bool b_cubeconvulation = false;
		float prefilterMipLevels = 1.0;
		float prefilterMapResolution = 128.0;
		float resizeMipLevels;
	};

}

