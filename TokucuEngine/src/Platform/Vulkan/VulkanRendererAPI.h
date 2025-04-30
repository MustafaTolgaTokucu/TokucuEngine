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
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);
	};

	struct ColorUniform {
		glm::vec3 color = { 1.0f,1.0f,1.0f };
	};

	struct shadowUBO {
		glm::mat4 pl_lightSpaceMatrix[6] = {};

	};

	struct lightIndexUBO {
		int lightIndex = 0;
	};

	struct LightAttributes
	{
		alignas(16) glm::vec3 pl_color = { 1.0f,1.0f,1.0f };
		alignas(16) glm::vec3 pl_position = { 0.0f,0.0f,0.0f };
		alignas(16) glm::vec3 pl_ambient = { 0.2f,0.2f,0.2f };
		alignas(16) glm::vec3 pl_diffuse = { 1.0f,1.0f,1.0f };
		alignas(16) glm::vec3 pl_specular = { 1.0f,1.0f,1.0f };
		alignas(16) glm::vec3 pl_viewpot = { 0.0f,0.0f,0.0f };
		float pl_constant = 1.0f;
		float pl_linear = 0.09f;
		float pl_quadratic = 0.032f;
		float pl_pointlightNumber = 0.0f;
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily = std::nullopt;
		std::optional<uint32_t> presentFamily = std::nullopt;
		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities = {};
		std::vector<VkSurfaceFormatKHR> formats = {};
		std::vector<VkPresentModeKHR> presentModes = {};
	};

	struct Pipeline {
		std::string name = "default";
		char vertexShader[64];
		char fragmentShader[64];
		char geometryShader[64];
		std::vector<std::pair<VkDescriptorType, VkShaderStageFlagBits>> descriptorLayout = {}; // Store descriptor types and their corresponding shader stages
		std::vector<std::pair<VkDescriptorBufferInfo*, VkBufferUsageFlagBits>> descriptionBufferInfo = {}; // Store buffer info and usage flags
		std::vector<VkDescriptorImageInfo*> descriptionSampleInfo = {}; // Store image info
		std::vector<VkDeviceSize> bufferSize = {}; // Store buffer sizes

		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

	};

	struct VulkanObject {
		std::string name = "default";
		std::vector<Vertex> vertexData = {};
		std::vector<uint32_t> indexData = {};
		VkBuffer vertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
		VkBuffer indexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
		VkVertexInputBindingDescription bindingDescriptions{};
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
		Pipeline* pipeline = nullptr; // Pointer to the pipeline associated with this object
		bool b_PBR = false; // Flag to indicate if the object uses PBR
		std::optional<std::string> modelLocation = std::nullopt; // Optional model location
		std::vector<std::pair<std::string, std::string>> textureLocations = { {"ambient", "assets/textures/default.jpg"},{"diffuse", "assets/textures/default.jpg"},{"specular", "assets/textures/default.jpg"},
			{"normal", "assets/textures/defaultNormal.jpg"} }; // Store multiple texture paths

		std::vector<std::vector<VkBuffer>> uniformBuffers = {};
		std::vector<std::vector<VkDeviceMemory>> uniformBuffersMemory = {};
		std::vector<std::vector<void*>> uniformBuffersMapped = {};

		std::vector<VkDescriptorSet> descriptorSets = {};
		struct Textures {
			VkImage ambient = VK_NULL_HANDLE;
			VkDeviceMemory ambientMemory = VK_NULL_HANDLE;
			VkImage diffuse = VK_NULL_HANDLE;
			VkDeviceMemory diffuseMemory = VK_NULL_HANDLE;
			VkImage specular = VK_NULL_HANDLE;
			VkDeviceMemory specularMemory = VK_NULL_HANDLE;
			VkImage normal = VK_NULL_HANDLE;
			VkDeviceMemory normalMemory = VK_NULL_HANDLE;

			std::array<VkImageView, 4> imageViews;//ambient,diffuse,specular respectively
		} textures;
	};

	struct BufferData {
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
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
		//////////////////////////
		//MTT Additional Functions
		//////////////////////////
		void registerPipeline();
		void createObject();
		void createModel(std::string modelName, std::string modelLocation, std::string textureLocation);
		void createShadowImage();
		void createCubemapImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
			VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t layerCount);
		void createShadowFramebuffer();
		VkImageView createShadowImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
		VkImageView createCubemapImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, uint32_t baseMipLevel);
		VkVertexInputBindingDescription getBindingDescription();
		std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();
		////////////////////////////////////////////
		/////VULKAN CLASS MEMBERS
		////////////////////////////////////////////
		VkInstance instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;

		VkSwapchainKHR swapChain = VK_NULL_HANDLE;
		std::vector<VkImage> swapChainImages = {};
		VkFormat swapChainImageFormat = VK_FORMAT_UNDEFINED;
		VkExtent2D swapChainExtent = {};

		std::vector<VkImageView> swapChainImageViews = {};
		VkRenderPass renderPass = VK_NULL_HANDLE;
		VkRenderPass shadowRenderPass = VK_NULL_HANDLE;
		VkRenderPass renderPassHDR = VK_NULL_HANDLE;
		VkRenderPass BRDFRenderPass = VK_NULL_HANDLE;

		std::vector<VkFramebuffer> swapChainFramebuffers = {};
		VkFramebuffer shadowFrameBuffer = VK_NULL_HANDLE;
		VkFramebuffer HDRCubeFrameBuffer = VK_NULL_HANDLE;
		VkFramebuffer CubeConvolutionFrameBuffer = VK_NULL_HANDLE;
		VkFramebuffer BRDFFrameBuffer = VK_NULL_HANDLE;
		std::vector<VkFramebuffer> prefilterMapFramebuffers = {};

		VkCommandPool commandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> commandBuffers = {};
		std::vector<VkSemaphore> imageAvailableSemaphores = {};
		std::vector<VkSemaphore> renderFinishedSemaphores = {};
		std::vector<VkFence> inFlightFences = {};
		
		std::vector<Vertex> vertices = {};
		std::vector<uint32_t> indices = {};

		std::vector<glm::vec3> cubePositions;
		std::vector<Vertex> secondVertices;
		std::vector<uint32_t> secondIndices;

		std::vector<Vertex> cubeVertices;
		std::vector<uint32_t> cubeIndices;

		std::vector<BufferData> objectCreationBuffers = {};

		std::unordered_map<std::string, glm::mat4> objectTransformations; //Holding transformation information for each object
		std::unordered_map<std::string, glm::vec3> objectColor;

		std::vector<LightAttributes> pointLights = { pointLight1,pointLight2 };

		std::vector<Pipeline*> Pipelines;
		Pipeline m_Pipeline;
		Pipeline m_Pipeline2;
		Pipeline m_PipelineSkybox;
		Pipeline m_PipelineShadow;
		Pipeline m_PipelineSkyboxHDR;
		Pipeline m_PipelineCubeConv;
		Pipeline m_PipelinePrefilter;
		Pipeline m_PipelineBRDF;

		std::vector<VulkanObject> Objects;

		LightAttributes pointLight1;
		LightAttributes pointLight2;

		VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory colorImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory shadowImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory skyboxImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory skyboxHDRImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory HDRCubeImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory CubeConvolutionImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory prefilterMapImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory BRDFImageMemory = VK_NULL_HANDLE;

		VkImageView depthImageView = VK_NULL_HANDLE;
		VkImageView colorImageView = VK_NULL_HANDLE;
		VkImageView shadowImageView = VK_NULL_HANDLE;
		VkImageView HDRCubeView = VK_NULL_HANDLE;
		VkImageView prefilterMapView = VK_NULL_HANDLE;
		VkImageView BRDFImageView = VK_NULL_HANDLE;
		VkImageView skyboxImageView = VK_NULL_HANDLE;
		VkImageView skyboxHDRImageView = VK_NULL_HANDLE;
		VkImageView CubeConvolutionImageView = VK_NULL_HANDLE;

		std::vector<VkImageView> prefilterMapTempViews = {};
		std::vector<VkImage> textureImages = {};
		std::vector<VkDeviceMemory> textureImagesMemory = {};
		std::vector<VkImageView> textureImagesView = {};

		VkImage depthImage = VK_NULL_HANDLE;
		VkImage colorImage = VK_NULL_HANDLE;
		VkImage shadowImage = VK_NULL_HANDLE;
		VkImage skyboxImage = VK_NULL_HANDLE;
		VkImage skyboxHDRImage = VK_NULL_HANDLE;
		VkImage HDRCubeImage = VK_NULL_HANDLE;
		VkImage CubeConvolutionImage = VK_NULL_HANDLE;
		VkImage prefilterMapImage = VK_NULL_HANDLE;
		VkImage BRDFImage = VK_NULL_HANDLE;
	
		VkSampler textureSampler = VK_NULL_HANDLE;
		VkSampler shadowSampler = VK_NULL_HANDLE;
		VkSampler prefilterTextureSampler = VK_NULL_HANDLE;

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

		GLFWwindow* glfwWindow = nullptr;
		
		const int MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t currentFrame = 0;
		bool framebufferResized = false;

		bool b_cubeconvulation = false;
		float prefilterMipLevels = 1.0;
		float prefilterMapResolution = 128.0;
		float resizeMipLevels = 1.0;
		int shadowMapSize = 2048;
		uint32_t mipLevels = 1.0;
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	};

}

