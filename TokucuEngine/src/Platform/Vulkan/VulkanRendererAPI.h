#pragma once
#include "Tokucu/Renderer/RendererAPI.h"
#include "VulkanCore.h"
#include <fstream>

//libs attached while vulkan implementation
#include <variant>

namespace Tokucu {
	// Forward declarations
	class VulkanSwapChain;
	class VulkanCreateImage;
	class VulkanGraphicsPipeline;
	class VulkanBuffer;
	
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

		void createGraphicsPipeline();
		void createRenderPass();

		void createTextureImage();
		void createTextureSampler();

		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentImage);
		void createSyncObjects();
		//Model Loading with assimp. (FBX is set to default for now) 
		//ModelData loadModel(std::string modelLocation);
		void createUniformBuffers();
		void updateUniformBuffer(uint32_t currentImage);
		void createDescriptorPool();
		void createDescriptorSets();

		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectFlags, int layerCount, uint32_t mipLevels, uint32_t baseMipLevel);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t baseArrayLayer);

		void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

		//////////////////////////
		//MTT Additional Functions
		//////////////////////////
		void registerPipeline();
		void createObject();
		void createModel(std::string modelName, std::string modelLocation, std::string textureLocation);
		void createShadowFramebuffer();
		////////////////////////////////////////////
		/////VULKAN CLASS MEMBERS
		////////////////////////////////////////////
		VkInstance instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		//TEST FOR NOW
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
		
		VkRenderPass swapRenderPass = VK_NULL_HANDLE;
		VkRenderPass shadowRenderPass = VK_NULL_HANDLE;
		VkRenderPass renderPassHDR = VK_NULL_HANDLE;
		VkRenderPass BRDFRenderPass = VK_NULL_HANDLE;
		
		VkFramebuffer shadowFrameBuffer = VK_NULL_HANDLE;
		VkFramebuffer HDRCubeFrameBuffer = VK_NULL_HANDLE;
		VkFramebuffer CubeConvolutionFrameBuffer = VK_NULL_HANDLE;
		VkFramebuffer BRDFFrameBuffer = VK_NULL_HANDLE;
		std::vector<VkFramebuffer> prefilterMapFramebuffers = {};

		//VkCommandPool commandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> commandBuffers = {};
		std::vector<VkSemaphore> imageAvailableSemaphores = {};
		std::vector<VkSemaphore> renderFinishedSemaphores = {};
		std::vector<VkFence> inFlightFences = {};
		
		std::vector<Vertex> vertices = {};
		std::vector<uint32_t> indices = {};

		//added to eCS
		std::vector<glm::vec3> cubePositions;
		std::vector<Vertex> secondVertices;
		std::vector<uint32_t> secondIndices;
		//added to ECS
		std::vector<Vertex> cubeVertices;
		std::vector<uint32_t> cubeIndices;
		//added to ECS
		std::vector<BufferData> objectCreationBuffers = {};

		std::unordered_map<std::string, glm::mat4> objectTransformations; //Holding transformation information for each object
		std::unordered_map<std::string, glm::vec3> objectColor;

		std::vector<LightAttributes> pointLights = { pointLight1,pointLight2 };
		//added to ECS
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

		VkDeviceMemory shadowImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory skyboxImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory skyboxHDRImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory HDRCubeImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory CubeConvolutionImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory prefilterMapImageMemory = VK_NULL_HANDLE;
		VkDeviceMemory BRDFImageMemory = VK_NULL_HANDLE;

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

		GLFWwindow* glfwWindow = nullptr;
		
		int MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t currentFrame = 0;
		bool framebufferResized = false;

		bool b_cubeconvulation = false;
		float prefilterMipLevels = 1.0;
		float prefilterMapResolution = 128.0;
		float resizeMipLevels = 1.0;
		int shadowMapSize = 2048;
		uint32_t mipLevels = 1.0;
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;


		//ABSTRACTION TEST MEMBERS
		std::unique_ptr<VulkanCore> m_VulkanCore;
		std::unique_ptr<VulkanSwapChain> m_VulkanSwapChain;
		std::unique_ptr<VulkanCreateImage> m_VulkanCreateImage;
		std::unique_ptr<VulkanGraphicsPipeline> m_VulkanGraphicsPipeline;
		std::unique_ptr<VulkanBuffer> m_VulkanBuffer;
	};

}

