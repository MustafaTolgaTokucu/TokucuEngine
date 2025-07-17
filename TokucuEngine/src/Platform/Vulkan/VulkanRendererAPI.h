#pragma once
#include "Tokucu/Renderer/RendererAPI.h"
#include "VulkanCore.h"
#include <fstream>

// ImGui includes
#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"

//libs attached while vulkan implementation
#include <variant>
#include <unordered_map>

namespace Tokucu {
	// Forward declarations
	class VulkanSwapChain;
	class VulkanCreateImage;
	class VulkanGraphicsPipeline;
	class VulkanBuffer;
	class VulkanFramebuffer;
	class VulkanRenderPass;
	class VulkanTextureManager;


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
		// Load an FBX model (and its textures) at runtime. The model will be added to the scene with
		// default textures (or engine fallbacks) and can later be modified via the existing
		// UpdateObjectTexture helpers.
		void LoadModelFromFBX(const std::string& modelPath, const std::string& textureDir);
		//void ResizeViewportFramebuffer(uint32_t width, uint32_t height);

		void initImGui();

		// --- Scene Editor Helpers ---
		// Access the list of rendered objects (modifiable)
		std::vector<VulkanObject>& GetObjects() { return Objects; }
		// Access transformation map so UI can read/modify it
		std::unordered_map<std::string, glm::mat4>& GetObjectTransformations() { return objectTransformations; }
		// Mark an object as modified so renderer skips overriding the transform each frame
		void MarkObjectAsModified(const std::string& objectName) { m_ModifiedObjects[objectName] = true; }
		// Update / replace a texture for a given object and material channel (by object pointer)
		void UpdateObjectTexture(VulkanObject* targetObj, const std::string& textureType, const std::string& newPath);
		// Convenience overload: find object by name and forward to the pointer version
		void UpdateObjectTexture(const std::string& objectName, const std::string& textureType, const std::string& newPath);

		// Viewport texture integration
		//const std::vector<VkImageView>& GetSwapChainImageViews() const;
		VkSampler GetTextureSampler() const { return textureSampler; }
		VkImageView GetImGuiTextureView() const { return m_OffscreenResolveImageView; }

		// Offscreen resources creation is now handled inside existing initialisation / framebuffer code.
		// (former CreateOffscreenResources / CreateOffscreenRenderPass removed)
		
		// Notify when offscreen resources are recreated
		//void NotifyOffscreenResourcesRecreated();
		
		// Update descriptor sets for offscreen resources
		//void UpdateDescriptorSetsForOffscreenResources();
		
		// Check if offscreen resources need to be recreated
		//bool NeedsOffscreenResourceRecreation() const { return !m_OffscreenResourcesCreated; }
		
		// Performance optimization: Cache object lists for faster iteration
		void CacheObjectLists();
		
		// Offscreen rendering for ImGui viewport
		VkImage m_OffscreenImage = VK_NULL_HANDLE;
		VkDeviceMemory m_OffscreenImageMemory = VK_NULL_HANDLE;
		VkImageView m_OffscreenImageView = VK_NULL_HANDLE;
		VkFramebuffer m_OffscreenFramebuffer = VK_NULL_HANDLE;
		VkRenderPass m_OffscreenRenderPass = VK_NULL_HANDLE;
		VkSampler m_OffscreenSampler = VK_NULL_HANDLE;
		VkExtent2D m_OffscreenExtent = {};
		VkFormat m_OffscreenFormat = VK_FORMAT_UNDEFINED;
		
		// --- BEGIN: Offscreen MSAA/Resolve/Depth for ImGui Viewport ---
		VkImage m_OffscreenColorImage = VK_NULL_HANDLE;
		VkDeviceMemory m_OffscreenColorImageMemory = VK_NULL_HANDLE;
		VkImageView m_OffscreenColorImageView = VK_NULL_HANDLE;

		VkImage m_OffscreenResolveImage = VK_NULL_HANDLE;
		VkDeviceMemory m_OffscreenResolveImageMemory = VK_NULL_HANDLE;
		VkImageView m_OffscreenResolveImageView = VK_NULL_HANDLE;

		VkImage m_OffscreenDepthImage = VK_NULL_HANDLE;
		VkDeviceMemory m_OffscreenDepthImageMemory = VK_NULL_HANDLE;
		VkImageView m_OffscreenDepthImageView = VK_NULL_HANDLE;
		// --- END: Offscreen MSAA/Resolve/Depth for ImGui Viewport ---

		// ImGui Vulkan resources
		VkDescriptorPool imGuiDescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_ImGuiDescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorSet m_ImGuiDescriptorSet = VK_NULL_HANDLE;
		VkSampler m_ImGuiSampler = VK_NULL_HANDLE;
		ImTextureID m_ImGuiTextureId = 0;
		bool imGuiInitialized = false;
		bool m_OffscreenResourcesCreated = false; // Track if offscreen resources have been created
		
		// Public method to get ImGui texture ID
		ImTextureID GetImGuiTextureId() const { return m_ImGuiTextureId; };
		
		// Method to register texture with ImGui (call after ImGui is fully initialized)
		//void RegisterImGuiTexture();

		ImTextureID GetOrCreateImGuiTexture(VkSampler sampler, VkImageView view);

	private:
		//////////////////////////////////
		///VULKAN FUNCTIONS
		/////////////////////////////////

		void createGraphicsPipeline();
		void createRenderPass();

		void createTextureImage();
		void createTextureSampler();

		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentImage);

		void createUniformBuffers();
		void updateUniformBuffer(uint32_t currentImage);
		void createDescriptorPool();
		void createDescriptorSets();

		//////////////////////////
		//MTT Additional Functions
		//////////////////////////
		void registerPipeline();
		void createObject();
		void registerModel();
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

		// Shadow and IBL framebuffers
		VkFramebuffer shadowFrameBuffer = VK_NULL_HANDLE;
		VkFramebuffer HDRCubeFrameBuffer = VK_NULL_HANDLE;
		VkFramebuffer CubeConvolutionFrameBuffer = VK_NULL_HANDLE;
		VkFramebuffer BRDFFrameBuffer = VK_NULL_HANDLE;
		std::vector<VkFramebuffer> prefilterMapFramebuffers;

		std::vector<Vertex> vertices = {};
		std::vector<uint32_t> indices = {};

		//std::vector<glm::vec3> cubePositions;
		


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

		// SceneEditor integration
		bool m_SceneEditorModified = false;
		std::unordered_map<std::string, bool> m_ModifiedObjects; // Track which objects have been modified by SceneEditor

		GLFWwindow* glfwWindow = nullptr;

		int MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t currentFrame = 0;
		bool framebufferResized = false;

		// IBL and shadow configuration
		uint32_t mipLevels = 1.0;
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
		bool b_cubeconvulation = false;
		uint32_t shadowMapSize = 2048;
		uint32_t prefilterMipLevels = 5;
		uint32_t prefilterMapResolution = 128;
		uint32_t resizeMipLevels = 1;

		// Performance optimization flags
		bool m_ShadowPassEnabled = true;  // Can be disabled if no shadow casters
		bool m_IBLPreprocessingComplete = false;  // Track if IBL preprocessing is done
		bool m_SceneObjectsCached = false;  // Cache scene object lists for faster iteration
		std::vector<VulkanObject*> m_ShadowCasters;  // Cached list of objects that cast shadows
		std::vector<VulkanObject*> m_SceneObjects;   // Cached list of scene objects
		std::vector<VulkanObject*> m_LightObjects;   // Cached list of light objects


		//ABSTRACTION TEST MEMBERS
		std::unique_ptr<VulkanCore> m_VulkanCore;
		std::unique_ptr<VulkanSwapChain> m_VulkanSwapChain;
		std::unique_ptr<VulkanCreateImage> m_VulkanCreateImage;
		std::unique_ptr<VulkanGraphicsPipeline> m_VulkanGraphicsPipeline;
		std::unique_ptr<VulkanBuffer> m_VulkanBuffer;
		std::unique_ptr<VulkanFramebuffer> m_VulkanFramebuffer;
		std::unique_ptr<VulkanRenderPass> m_VulkanRenderPass;
		std::unique_ptr<VulkanTextureManager> m_VulkanTextureManager;

		void RefreshImGuiOffscreenTexture();
		

		
		
	};

}