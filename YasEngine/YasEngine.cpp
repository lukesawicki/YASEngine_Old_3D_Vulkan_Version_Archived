#define STB_IMAGE_IMPLEMENTATION
#include"stdafx.hpp"
#include"YasEngine.hpp"
#include"VariousTools.hpp"

//-----------------------------------------------------------------------------|---------------------------------------|

int YasEngine::windowPositionX				= 64;
int YasEngine::windowPositionY				= 64;
int YasEngine::windowWidth					= 800;
int YasEngine::windowHeight					= 600;
bool YasEngine::framebufferResized = false;
const int MAX_FRAMES_IN_FLIGHT = 2;

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	
	switch(message) {
		case WM_DISPLAYCHANGE:
			YasEngine::framebufferResized = true;
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return(0);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

VkResult createDebugReportCallbackEXT(VkInstance& vulkanInstance, const VkDebugReportCallbackCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugReportCallbackEXT* callback) {

	PFN_vkCreateDebugReportCallbackEXT debugReportCallbackFunction = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugReportCallbackEXT");
	
	if(debugReportCallbackFunction != nullptr) {
		return debugReportCallbackFunction(vulkanInstance, createInfo, allocator, callback);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void destroyDebugReportCallbackEXT(VkInstance vulkanInstance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* allocator) {

	PFN_vkDestroyDebugReportCallbackEXT destroyFunction = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vulkanInstance, "vkDestroyDebugReportCallbackEXT");
	
	if(destroyFunction != nullptr) {
		destroyFunction(vulkanInstance, callback, allocator);
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL YasEngine::debugCallback	(VkDebugReportFlagsEXT debugReportFlags, VkDebugReportObjectTypeEXT objectType,	uint64_t object, size_t location, int32_t code,	const char* layerPrefix, const char* msg, void* userData) {

	std::cerr << "Validation layer: " << msg << std::endl;
	//If return true, then call is aborted with the VK_ERROR_VALIDATION_FAILED_EXT
	//because this is used to test the validation layers themeselves
	//then for now always return false
	return VK_FALSE;
}

YasEngine::YasEngine() {

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE* file;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "w", stderr);
	SetConsoleTitle("YasEngine logging");
}

void YasEngine::setupDebugCallback() {

	if(!enableValidationLayers)	{
		return;
	}
	
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;
	
	if(createDebugReportCallbackEXT(vulkanInstance.instance, &createInfo, nullptr, &callback) != VK_SUCCESS) {
		throw std::runtime_error("Failed to set up debug callback function");
	}
}

void YasEngine::run(HINSTANCE hInstance) {

	createWindow(hInstance);
	initializeVulkan();
	mainLoop();
	cleanUp();
}

//Private Methods

void YasEngine::createWindow(HINSTANCE hInstance) {

	WNDCLASSEX windowClassEx;

	windowClassEx.cbSize				= sizeof(WNDCLASSEX);
	windowClassEx.style					= CS_VREDRAW | CS_HREDRAW;
	windowClassEx.lpfnWndProc			= windowProcedure;
	windowClassEx.cbClsExtra			= 0;
	windowClassEx.cbWndExtra			= 0;
	windowClassEx.hInstance				= hInstance;
	windowClassEx.hIcon					= LoadIcon(0, IDI_APPLICATION);
	windowClassEx.hCursor				= LoadCursor(0, IDC_CROSS);
	windowClassEx.hbrBackground			= (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClassEx.lpszMenuName			= 0;
	windowClassEx.lpszClassName			= "YASEngine window class";
	windowClassEx.hIconSm				= LoadIcon(0, IDI_APPLICATION);

	RegisterClassEx(&windowClassEx);

	application = hInstance;
	window =  CreateWindowEx(NULL, "YASEngine window class", "YASEngine", WS_OVERLAPPEDWINDOW, windowPositionX, windowPositionY, windowWidth, windowHeight, NULL, NULL, application, NULL);

	ShowWindow(window, SW_NORMAL);
	SetForegroundWindow(window);
	SetFocus(window);
}

void YasEngine::mainLoop() {

	float time;
	float newTime;
	float deltaTime;
	float fps, fpsTime;
	unsigned int frames;
	MSG message;

	TimePicker* timePicker = TimePicker::getTimePicker();
	time = timePicker->getSeconds();
	fpsTime = 0.0F;
	frames = 0;
	message.message = WM_NULL;

	while(message.message != WM_QUIT) {

		if(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		} else {
			newTime = timePicker->getSeconds();
			deltaTime = newTime - time;
			time = newTime;
			drawFrame(deltaTime);			
			frames++;
			fpsTime = fpsTime + deltaTime;
			if(fpsTime >= 1.0F)
			{
				fps = frames / fpsTime;
				frames = 0;
				fpsTime = 0.0F;
			}
		}
	}
}

void YasEngine::initializeVulkan() {

	createVulkanInstance();
	setupDebugCallback();
	createSurface();
	vulkanDevice = new VulkanDevice(vulkanInstance, surface, graphicsQueue, presentationQueue, enableValidationLayers);
	createSwapchain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
	createCommandBuffers();
	createSyncObjects();
}

void YasEngine::createVulkanInstance() {

	vulkanInstance.createVulkanInstance(enableValidationLayers);
}

void YasEngine::selectPhysicalDevice() {

	vulkanDevice->selectPhysicalDevice(vulkanInstance, surface);
}

void YasEngine::createCommandPool() {

	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(vulkanDevice->physicalDevice, surface);

	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

	if(vkCreateCommandPool(vulkanDevice->logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool!");
	}
}

void YasEngine::createVertexBuffer() {

	//VkDeviceSize is alias to uint64_t
	VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;

	vkMapMemory(vulkanDevice->logicalDevice, stagingBufferMemory, 0, vertexBufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)vertexBufferSize);
	vkUnmapMemory(vulkanDevice->logicalDevice, stagingBufferMemory);

	createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	copyBuffer(stagingBuffer, vertexBuffer, vertexBufferSize);

	vkDestroyBuffer(vulkanDevice->logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(vulkanDevice->logicalDevice, stagingBufferMemory, nullptr);
}

void YasEngine::createIndexBuffer() {

	VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT	| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;

	vkMapMemory(vulkanDevice->logicalDevice, stagingBufferMemory, 0, indexBufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)indexBufferSize);
	vkUnmapMemory(vulkanDevice->logicalDevice, stagingBufferMemory);

	createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
	copyBuffer(stagingBuffer, indexBuffer, indexBufferSize);

	vkDestroyBuffer(vulkanDevice->logicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(vulkanDevice->logicalDevice, stagingBufferMemory, nullptr);	
}

uint32_t YasEngine::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags memoryPropertiesFlags) {

	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

	vkGetPhysicalDeviceMemoryProperties(vulkanDevice->physicalDevice, &physicalDeviceMemoryProperties);
	
	for(uint32_t i=0; i<physicalDeviceMemoryProperties.memoryTypeCount; i++) {
		if(typeFilter & (1 << i) && (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertiesFlags) == memoryPropertiesFlags) {
			return i;
		}
	}
	throw std::runtime_error("Filed to find suitable memory type.");
}

void YasEngine::createCommandBuffers() {

	commandBuffers.resize(swapchainFramebuffers.size());

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if(vkAllocateCommandBuffers(vulkanDevice->logicalDevice, &commandBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocatae command buffers.");
	}

	for(size_t i=0; i<commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if(vkBeginCommandBuffer(commandBuffers[i], &commandBufferBeginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer.");
		}

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = swapchainFramebuffers[i];
		renderPassBeginInfo.renderArea.offset = {0, 0};
		renderPassBeginInfo.renderArea.extent = vulkanSwapchain.swapchainExtent;
		VkClearValue clearColor = {0.0F, 0.0F, 0.0F, 1.0F};
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		
		VkBuffer vertexBuffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = {0};

		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
		vkCmdEndRenderPass(commandBuffers[i]);

		if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}

	}
}

void YasEngine::drawFrame(float deltaTime) {

	vkWaitForFences(vulkanDevice->logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	
	uint32_t imageIndex;

	VkResult result = vkAcquireNextImageKHR(vulkanDevice->logicalDevice, vulkanSwapchain.swapchain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	
	if(result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
		return;
	} else {
		if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}
	}

	updateUniformBuffer(imageIndex, deltaTime);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(vulkanDevice->logicalDevice, 1, &inFlightFences[currentFrame]);

	if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer.");
	}
	
	VkPresentInfoKHR presentInfoKhr = {};
	presentInfoKhr.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfoKhr.waitSemaphoreCount = 1;
	presentInfoKhr.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {vulkanSwapchain.swapchain};

	presentInfoKhr.swapchainCount = 1;
	presentInfoKhr.pSwapchains = swapChains;
	presentInfoKhr.pImageIndices = &imageIndex;
	result = vkQueuePresentKHR(presentationQueue, &presentInfoKhr);

	if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || YasEngine::framebufferResized) {
		YasEngine::framebufferResized = false;
		recreateSwapchain();
	} else {
		if(result != VK_SUCCESS) {
			throw std::runtime_error("Failed to presesnt swap chain image.");
		}
	}
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void YasEngine::createSyncObjects() {

	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};

	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(size_t i = 0; i< MAX_FRAMES_IN_FLIGHT; i++) {
		if(vkCreateSemaphore(vulkanDevice->logicalDevice, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(vulkanDevice->logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(vulkanDevice->logicalDevice, &fenceCreateInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS
		) {
			throw std::runtime_error("Failed to create semaphores for a frame.");
		}
	}
}

void YasEngine::createBuffer(VkDeviceSize size,VkBufferUsageFlags usage,VkMemoryPropertyFlags properties,VkBuffer &buffer,VkDeviceMemory &bufferMemory) {

	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
	if(vkCreateBuffer(vulkanDevice->logicalDevice, &bufferCreateInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create vertex buffer.");
	}

	VkMemoryRequirements memoryRequirements;

	vkGetBufferMemoryRequirements(vulkanDevice->logicalDevice, buffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType =VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties);

	if(vkAllocateMemory(vulkanDevice->logicalDevice, &memoryAllocateInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(vulkanDevice->logicalDevice, buffer, bufferMemory, 0);
}

void YasEngine::copyBuffer(VkBuffer sourceBuffer,VkBuffer destinationBuffer,VkDeviceSize deviceSize) {

	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = deviceSize;
	vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void YasEngine::createDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding uniformBufferObjectLayoutBinding = {};
	uniformBufferObjectLayoutBinding.binding = 0;
	uniformBufferObjectLayoutBinding.descriptorCount = 1;
	uniformBufferObjectLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferObjectLayoutBinding.pImmutableSamplers = nullptr;
	uniformBufferObjectLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uniformBufferObjectLayoutBinding, samplerLayoutBinding};

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	descriptorSetLayoutCreateInfo.pBindings = bindings.data();

	if(vkCreateDescriptorSetLayout(vulkanDevice->logicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor set layout");
	}
}

void YasEngine::createUniformBuffers() {

	VkDeviceSize uniformBufferSize = sizeof(UniformBufferObject);
	
	uniformBuffers.resize(vulkanSwapchain.swapchainImages.size());
	uniformBuffersMemory.resize(vulkanSwapchain.swapchainImages.size());
	
	for(size_t i = 0; i < vulkanSwapchain.swapchainImages.size(); i++) {
		createBuffer(uniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
	}

}

void YasEngine::updateUniformBuffer(uint32_t currentImage, float deltaTime) {

	float time = zeroTime += deltaTime;

	UniformBufferObject uniformBufferObject = {};
	uniformBufferObject.model = glm::rotate(glm::mat4(1.0F), time * glm::radians(90.0F), glm::vec3(0.0F, 0.0F, 1.0F));
	uniformBufferObject.view = glm::lookAt(glm::vec3(2.0F, 2.0F, 2.0F), glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 0.0F, 1.0F));
	uniformBufferObject.proj = glm::perspective(glm::radians(45.0F), vulkanSwapchain.swapchainExtent.width / (float) vulkanSwapchain.swapchainExtent.height, 0.1f, 10.0F);
	uniformBufferObject.proj[1][1] *= -1;

	void* data;

	vkMapMemory(vulkanDevice->logicalDevice, uniformBuffersMemory[currentImage], 0, sizeof(uniformBufferObject), 0, &data);
	memcpy(data, &uniformBufferObject, sizeof(uniformBufferObject));
	vkUnmapMemory(vulkanDevice->logicalDevice, uniformBuffersMemory[currentImage]);
}

void YasEngine::createLogicalDevice() {

	vulkanDevice->createLogicalDevice(vulkanInstance, surface, graphicsQueue, presentationQueue, enableValidationLayers);
}

void YasEngine::createSurface() {

	VkWin32SurfaceCreateInfoKHR	surfaceCreateInfo;
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = NULL;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = application;
	surfaceCreateInfo.hwnd = window;

	VkResult result = vkCreateWin32SurfaceKHR(vulkanInstance.instance, &surfaceCreateInfo, NULL, &surface);

	if(!(result == VK_SUCCESS)) {
		throw std::runtime_error("Failed to create Vulkan surface!");
	}
}

void YasEngine::createSwapchain() {

	QueueFamilyIndices queueIndices = findQueueFamilies(vulkanDevice->physicalDevice, surface);
	vulkanSwapchain.createSwapchain(vulkanDevice->physicalDevice, surface, vulkanDevice->logicalDevice, queueIndices, window);
}

void YasEngine::recreateSwapchain() {

	vkDeviceWaitIdle(vulkanDevice->logicalDevice);
	cleanupSwapchain();
	createSwapchain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandBuffers();
}

void YasEngine::createImageViews() {

	vulkanSwapchain.createImageViews(vulkanDevice->logicalDevice);
}

void YasEngine::cleanupSwapchain() {

	for(size_t i=0; i < swapchainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(vulkanDevice->logicalDevice, swapchainFramebuffers[i], nullptr);
	}

	vkFreeCommandBuffers(vulkanDevice->logicalDevice, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	vkDestroyPipeline(vulkanDevice->logicalDevice, graphicsPipeline, nullptr);
	vkDestroyRenderPass(vulkanDevice->logicalDevice, renderPass, nullptr);

	for(size_t i=0; i<vulkanSwapchain.swapchainImageViews.size(); i++) {
		vkDestroyImageView(vulkanDevice->logicalDevice, vulkanSwapchain.swapchainImageViews[i], nullptr);
	}
	
	vkDestroySwapchainKHR(vulkanDevice->logicalDevice, vulkanSwapchain.swapchain, nullptr);
}

void YasEngine::createRenderPass() {

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = vulkanSwapchain.swapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentReference = {};
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription = {};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorAttachmentReference;

	VkSubpassDependency subpassDependency = {};
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDescription;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &subpassDependency;

	if(vkCreateRenderPass(vulkanDevice->logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create renderpass.");
	}
}

void YasEngine::createGraphicsPipeline() {

	std::vector<char> vertShaderCode = readFile("Shaders\\vert.spv");
	std::vector<char> fragShaderCode = readFile("Shaders\\frag.spv");

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;

	vertShaderModule = createShaderModule(vertShaderCode);
	fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions =  attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0F;
	viewport.x = 0.0F;
	viewport.width = (float)(vulkanSwapchain.swapchainExtent.width);
	viewport.height = (float)(vulkanSwapchain.swapchainExtent.height);
	viewport.minDepth = 0.0F;
	viewport.maxDepth = 1.0F;

	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = vulkanSwapchain.swapchainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0F;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0F;
	colorBlending.blendConstants[1] = 0.0F;
	colorBlending.blendConstants[2] = 0.0F;
	colorBlending.blendConstants[3] = 0.0F;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	if(vkCreatePipelineLayout(vulkanDevice->logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Filed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo graphicsPiplineCreateInfo = {};
	graphicsPiplineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPiplineCreateInfo.stageCount = 2;
	graphicsPiplineCreateInfo.pStages = shaderStages;
	graphicsPiplineCreateInfo.pVertexInputState = &vertexInputInfo;
	graphicsPiplineCreateInfo.pInputAssemblyState = &inputAssembly;
	graphicsPiplineCreateInfo.pViewportState = &viewportState;
	graphicsPiplineCreateInfo.pRasterizationState = &rasterizer;
	graphicsPiplineCreateInfo.pMultisampleState = &multisampling;
	graphicsPiplineCreateInfo.pColorBlendState = &colorBlending;
	graphicsPiplineCreateInfo.layout = pipelineLayout;
	graphicsPiplineCreateInfo.renderPass = renderPass;
	graphicsPiplineCreateInfo.subpass = 0;
	graphicsPiplineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	if(vkCreateGraphicsPipelines(vulkanDevice->logicalDevice, VK_NULL_HANDLE, 1, &graphicsPiplineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create graphics pipeline");
	}

	vkDestroyShaderModule(vulkanDevice->logicalDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(vulkanDevice->logicalDevice, vertShaderModule, nullptr);
}

void YasEngine::createFramebuffers() {

	swapchainFramebuffers.resize(vulkanSwapchain.swapchainImageViews.size());

	for(size_t i=0; i<vulkanSwapchain.swapchainImageViews.size(); i++) {
		VkImageView attachments[] = { vulkanSwapchain.swapchainImageViews[i] };
		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = vulkanSwapchain.swapchainExtent.width;
		framebufferCreateInfo.height = vulkanSwapchain.swapchainExtent.height;
		framebufferCreateInfo.layers = 1;

		if(vkCreateFramebuffer(vulkanDevice->logicalDevice, &framebufferCreateInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer.");
		}
	}
}

VkShaderModule YasEngine::createShaderModule(const std::vector<char>& code) {

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;

	if(vkCreateShaderModule(vulkanDevice->logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create shader module");
	}
	return shaderModule;
}

void YasEngine::destroySwapchain() {

	vulkanSwapchain.destroySwapchain(vulkanDevice->logicalDevice);
}

void YasEngine::cleanUp() {

	cleanupSwapchain();
	vkDestroySampler(vulkanDevice->logicalDevice, textureSampler, nullptr);
	vkDestroyImageView(vulkanDevice->logicalDevice, textureImageView, nullptr);
    vkDestroyImage(vulkanDevice->logicalDevice, textureImage, nullptr);
    vkFreeMemory(vulkanDevice->logicalDevice, textureImageMemory, nullptr);
	vkDestroyDescriptorPool(vulkanDevice->logicalDevice, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(vulkanDevice->logicalDevice, descriptorSetLayout, nullptr);

	for(size_t i=0; i<vulkanSwapchain.swapchainImages.size(); i++) {
		vkDestroyBuffer(vulkanDevice->logicalDevice, uniformBuffers[i], nullptr);
		vkFreeMemory(vulkanDevice->logicalDevice, uniformBuffersMemory[i], nullptr);
	}

	vkDestroyBuffer(vulkanDevice->logicalDevice, indexBuffer, nullptr);
	vkFreeMemory(vulkanDevice->logicalDevice, indexBufferMemory, nullptr);

	vkDestroyBuffer(vulkanDevice->logicalDevice, vertexBuffer, nullptr);
	vkFreeMemory(vulkanDevice->logicalDevice, vertexBufferMemory, nullptr);

	for(size_t i = 0; i<MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(vulkanDevice->logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(vulkanDevice->logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(vulkanDevice->logicalDevice, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(vulkanDevice->logicalDevice, commandPool, nullptr);
	vkDestroyDevice(vulkanDevice->logicalDevice, nullptr);

	if(enableValidationLayers) {
		destroyDebugReportCallbackEXT(vulkanInstance.instance, callback, nullptr);
	}

	vkDestroySurfaceKHR(vulkanInstance.instance, surface, nullptr);
	vkDestroyInstance(vulkanInstance.instance, nullptr);
	DestroyWindow(window);
}

void YasEngine::createDescriptorPool() {

	std::array<VkDescriptorPoolSize, 2> poolSizes = {};

	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(vulkanSwapchain.swapchainImages.size());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(vulkanSwapchain.swapchainImages.size());	

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
	descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(vulkanSwapchain.swapchainImages.size());

	if(vkCreateDescriptorPool(vulkanDevice->logicalDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor pool.");
	}

}

void YasEngine::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts(vulkanSwapchain.swapchainImages.size(), descriptorSetLayout);

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(vulkanSwapchain.swapchainImages.size());
	descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

	descriptorSets.resize(vulkanSwapchain.swapchainImages.size());

	if(vkAllocateDescriptorSets(vulkanDevice->logicalDevice, &descriptorSetAllocateInfo, &descriptorSets[0]) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for(size_t i=0; i<vulkanSwapchain.swapchainImages.size(); i++) {

		VkDescriptorBufferInfo descriptorBufferInfo = {};
		descriptorBufferInfo.buffer = uniformBuffers[i];
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo descriptorImageInfo = {};
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptorImageInfo.imageView = textureImageView;
		descriptorImageInfo.sampler = textureSampler;

		std::array<VkWriteDescriptorSet, 2> writeDescriptorSets = {};

		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = descriptorSets[i];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfo;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = descriptorSets[i];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].pImageInfo = &descriptorImageInfo;

		vkUpdateDescriptorSets(vulkanDevice->logicalDevice, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}
}

void YasEngine::createTextureImage() {
	int textureWidth;
	int textureHeight;
	int textureChannels;
	stbi_uc* pixels = stbi_load("Textures\\texture.jpg", &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = textureWidth * textureHeight * 4;

	if(!pixels) {
		throw std::runtime_error("Failed to load texture image.");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	
	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void *data;

	vkMapMemory(vulkanDevice->logicalDevice, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(vulkanDevice->logicalDevice, stagingBufferMemory);
	stbi_image_free(pixels);

	createImage(textureWidth, textureHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(vulkanDevice->logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(vulkanDevice->logicalDevice, stagingBufferMemory, nullptr);
}

void YasEngine::createImage(uint32_t textureWidth, uint32_t textureHeight, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags imageUsageFlags, VkMemoryPropertyFlags memoryProperties, VkImage& image, VkDeviceMemory& imageMemory)
{

	VkImageCreateInfo imageCreateInfo = {};

	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = textureWidth;
	imageCreateInfo.extent.height = textureHeight;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = format;
	imageCreateInfo.tiling = imageTiling;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = imageUsageFlags;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	if(vkCreateImage(vulkanDevice->logicalDevice, &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image.");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(vulkanDevice->logicalDevice, image, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, memoryProperties);

	if (vkAllocateMemory(vulkanDevice->logicalDevice, &memoryAllocateInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(vulkanDevice->logicalDevice, image, imageMemory, 0);
}

VkCommandBuffer YasEngine::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(vulkanDevice->logicalDevice, &commandBufferAllocateInfo, &commandBuffer);

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

    return commandBuffer;
}

void YasEngine::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(vulkanDevice->logicalDevice, commandPool, 1, &commandBuffer);
}

void YasEngine::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
{

	VkCommandBuffer commandBuffer = beginSingleTimeCommands();
	VkImageMemoryBarrier imageMemoryBarrier = {};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourcePipelineStageFlag;
	VkPipelineStageFlags destinationPipelineStageFlags;

	if(oldImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourcePipelineStageFlag = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationPipelineStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if(oldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourcePipelineStageFlag = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationPipelineStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		throw std::invalid_argument("Unsuported layout transition.");
	}

	vkCmdPipelineBarrier(commandBuffer, sourcePipelineStageFlag, destinationPipelineStageFlags, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	endSingleTimeCommands(commandBuffer);
}

void YasEngine::copyBufferToImage(VkBuffer buffer,VkImage image,uint32_t imageWidth,uint32_t imageHeight)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy bufferImageCopyRegion = {};
	bufferImageCopyRegion.bufferOffset = 0;
	bufferImageCopyRegion.bufferRowLength = 0;
	bufferImageCopyRegion.bufferImageHeight = 0;
	bufferImageCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	bufferImageCopyRegion.imageSubresource.mipLevel = 0;
	bufferImageCopyRegion.imageSubresource.baseArrayLayer = 0;
	bufferImageCopyRegion.imageSubresource.layerCount = 1;
	bufferImageCopyRegion.imageOffset = {0, 0, 0};
	bufferImageCopyRegion.imageExtent = { imageWidth, imageHeight, 1 };

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopyRegion);

	endSingleTimeCommands(commandBuffer);
}

void YasEngine::createTextureImageView()
{
	textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_UNORM, vulkanDevice->logicalDevice);
}

void YasEngine::createTextureSampler()
{
	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.anisotropyEnable = VK_TRUE;
	samplerCreateInfo.maxAnisotropy = 16;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	
	if(vkCreateSampler(vulkanDevice->logicalDevice, &samplerCreateInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture sampler!");
	}
}

//-----------------------------------------------------------------------------|---------------------------------------|