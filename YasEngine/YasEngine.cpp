#include"stdafx.hpp"
#include"YasEngine.hpp"
#include"VariousTools.hpp"
//-----------------------------------------------------------------------------|---------------------------------------|

int YasEngine::windowPositionX				= 64;
int YasEngine::windowPositionY				= 64;
int YasEngine::windowWidth					= 640;
int YasEngine::windowHeight					= 480;

const int MAX_FRAMES_IN_FLIGHT = 2;
bool YasEngine::framebufferResized = false;

LRESULT CALLBACK windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DISPLAYCHANGE:
			YasEngine::framebufferResized = true;
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return(0);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

VkResult createDebugReportCallbackEXT
	(
		VkInstance& vulkanInstance,
		const VkDebugReportCallbackCreateInfoEXT* createInfo,
		const VkAllocationCallbacks* allocator,
		VkDebugReportCallbackEXT* callback
	)
{
	PFN_vkCreateDebugReportCallbackEXT debugReportCallbackFunction = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugReportCallbackEXT");
	if(debugReportCallbackFunction != nullptr)
	{
		return debugReportCallbackFunction(vulkanInstance, createInfo, allocator, callback);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void destroyDebugReportCallbackEXT(VkInstance vulkanInstance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* allocator)
{
	PFN_vkDestroyDebugReportCallbackEXT destroyFunction = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vulkanInstance, "vkDestroyDebugReportCallbackEXT");
	if(destroyFunction != nullptr)
	{
		destroyFunction(vulkanInstance, callback, allocator);
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL YasEngine::debugCallback
	(
	VkDebugReportFlagsEXT debugReportFlags,
	VkDebugReportObjectTypeEXT objectType,
	uint64_t object,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData
	)
{
	std::cerr << "Validation layer: " << msg << std::endl;
	//If return true, then call is aborted with the VK_ERROR_VALIDATION_FAILED_EXT
	//because this is used to test the validation layers themeselves
	//then for now always return false
	return VK_FALSE;
}

YasEngine::YasEngine()
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE* file;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "w", stderr);
	SetConsoleTitle("YasEngine logging");
}

void YasEngine::setupDebugCallback()
{
	if(!enableValidationLayers)
	{
		return;
	}
	
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;
	
	if(createDebugReportCallbackEXT(vulkanInstance.instance, &createInfo, nullptr, &callback) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to set up debug callback function");
	}
}

void YasEngine::run(HINSTANCE hInstance)
{
	createWindow(hInstance);
	initializeVulkan();
	mainLoop();
	cleanUp();
}

//Private Methods

void YasEngine::createWindow(HINSTANCE hInstance)
{
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

	window =  CreateWindowEx
				(
					NULL,
					"YASEngine window class",
					"YASEngine",
					WS_OVERLAPPEDWINDOW,
					windowPositionX, windowPositionY,
					windowWidth, windowHeight,
					NULL,
					NULL,
					application,
					NULL
				);

	ShowWindow(window, SW_NORMAL);
	SetForegroundWindow(window);
	SetFocus(window);
}

void YasEngine::mainLoop()
{
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

	while(message.message != WM_QUIT)
	{
		if(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			newTime = timePicker->getSeconds();
			deltaTime = newTime - time;
			time = newTime;

			// Calculating and drawing
			drawFrame();			

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

void YasEngine::initializeVulkan()
{
	createVulkanInstance();
	setupDebugCallback();
	createSurface();
	vulkanDevice = new VulkanDevice(vulkanInstance, surface, graphicsQueue, presentationQueue, enableValidationLayers);
	createSwapchain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createCommandBuffers();
	createSyncObjects();
}

void YasEngine::createVulkanInstance()
{
	vulkanInstance.createVulkanInstance(enableValidationLayers);
}

void YasEngine::selectPhysicalDevice()
{
	vulkanDevice->selectPhysicalDevice(vulkanInstance, surface);
}

void YasEngine::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(vulkanDevice->physicalDevice, surface);
	
	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	commandPoolCreateInfo.flags = 0;
	if(vkCreateCommandPool(vulkanDevice->logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command pool!");
	}
}

void YasEngine::createCommandBuffers()
{
	commandBuffers.resize(swapchainFramebuffers.size());

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();
	if(vkAllocateCommandBuffers(vulkanDevice->logicalDevice, &commandBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocatae command buffers.");
	}

	for(size_t i=0; i<commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		if(vkBeginCommandBuffer(commandBuffers[i], &commandBufferBeginInfo) != VK_SUCCESS)
		{
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
		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers[i]);
		if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer");
		}
		
	}

}

void YasEngine::drawFrame()
{
	vkWaitForFences(vulkanDevice->logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	
	
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(vulkanDevice->logicalDevice, vulkanSwapchain.swapchain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	
	if(result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapchain();
		return;
	}
	else
	{
		if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Failed to acquire swap chain image!");
		}
	}

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

	if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
	{
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

	if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || YasEngine::framebufferResized)
	{
		YasEngine::framebufferResized = false;
		recreateSwapchain();
	}
	else
	{
		if(result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to presesnt swap chain image.");
		}
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void YasEngine::createSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(size_t i = 0; i< MAX_FRAMES_IN_FLIGHT; i++)
	{
		if(vkCreateSemaphore(vulkanDevice->logicalDevice, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(vulkanDevice->logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(vulkanDevice->logicalDevice, &fenceCreateInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS
		)
		{
			throw std::runtime_error("Failed to create semaphores for a frame.");
		}
	}

}

void YasEngine::createLogicalDevice()
{
	vulkanDevice->createLogicalDevice(vulkanInstance, surface, graphicsQueue, presentationQueue, enableValidationLayers);
}

void YasEngine::createSurface()
{
	VkWin32SurfaceCreateInfoKHR	surfaceCreateInfo;
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.pNext = NULL;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.hinstance = application;
	surfaceCreateInfo.hwnd = window;

	VkResult result = vkCreateWin32SurfaceKHR(vulkanInstance.instance, &surfaceCreateInfo, NULL, &surface);
	if(!(result == VK_SUCCESS))
	{
		throw std::runtime_error("Failed to create Vulkan surface!");
	}
}

void YasEngine::createSwapchain()
{
	QueueFamilyIndices queueIndices = findQueueFamilies(vulkanDevice->physicalDevice, surface);
	vulkanSwapchain.createSwapchain(vulkanDevice->physicalDevice, surface, vulkanDevice->logicalDevice, queueIndices, window);
}

void YasEngine::recreateSwapchain()
{
	vkDeviceWaitIdle(vulkanDevice->logicalDevice);
	cleanupSwapchain();
	createSwapchain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandBuffers();
}

void YasEngine::createImageViews()
{
	vulkanSwapchain.createImageViews(vulkanDevice->logicalDevice);
}

void YasEngine::cleanupSwapchain()
{
	for(size_t i=0; i < swapchainFramebuffers.size(); i++)
	{
		vkDestroyFramebuffer(vulkanDevice->logicalDevice, swapchainFramebuffers[i], nullptr);
	}

	vkFreeCommandBuffers(vulkanDevice->logicalDevice, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	vkDestroyPipeline(vulkanDevice->logicalDevice, graphicsPipeline, nullptr);
	vkDestroyRenderPass(vulkanDevice->logicalDevice, renderPass, nullptr);

	for(size_t i=0; i<vulkanSwapchain.swapchainImageViews.size(); i++)
	{
		vkDestroyImageView(vulkanDevice->logicalDevice, vulkanSwapchain.swapchainImageViews[i], nullptr);
	}
	
	vkDestroySwapchainKHR(vulkanDevice->logicalDevice, vulkanSwapchain.swapchain, nullptr);
}

void YasEngine::createRenderPass()
{
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
	renderPassInfo.dependencyCount = 0;
	renderPassInfo.pDependencies = &subpassDependency;

	if(vkCreateRenderPass(vulkanDevice->logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create renderpass.");
	}
}

void YasEngine::createGraphicsPipeline()
{
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
	// For now vertex data are hard coded in vertex shader so
	// VkPiplineVertexInputStateCreateInfo is specified that there is no
	// vertex data to load for now.

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;

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
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	//rasterizer.depthBiasConstantFactor = 0.0F;
	//rasterizer.depthBiasClamp = 0.0F;
	//rasterizer.depthBiasSlopeFactor = 0.0F;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0F;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	//VkPipelineDepthStencilStateCreateInfo

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
	VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

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
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if(vkCreatePipelineLayout(vulkanDevice->logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
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
	graphicsPiplineCreateInfo.pDepthStencilState = nullptr;
	graphicsPiplineCreateInfo.pColorBlendState = &colorBlending;
	graphicsPiplineCreateInfo.pDynamicState = nullptr;
	graphicsPiplineCreateInfo.layout = pipelineLayout;
	graphicsPiplineCreateInfo.renderPass = renderPass;
	graphicsPiplineCreateInfo.subpass = 0;
	graphicsPiplineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	if(vkCreateGraphicsPipelines(vulkanDevice->logicalDevice, VK_NULL_HANDLE, 1, &graphicsPiplineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphics pipeline");
	}

	vkDestroyShaderModule(vulkanDevice->logicalDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(vulkanDevice->logicalDevice, vertShaderModule, nullptr);
}

void YasEngine::createFramebuffers()
{
	swapchainFramebuffers.resize(vulkanSwapchain.swapchainImageViews.size());
	for(size_t i=0; i<swapchainFramebuffers.size(); i++)
	{
		VkImageView attachments[] = { vulkanSwapchain.swapchainImageViews[i] };

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = vulkanSwapchain.swapchainExtent.width;
		framebufferCreateInfo.height = vulkanSwapchain.swapchainExtent.height;
		framebufferCreateInfo.layers = 1;

		if(vkCreateFramebuffer(vulkanDevice->logicalDevice, &framebufferCreateInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create framebuffer.");
		}

	}

}

VkShaderModule YasEngine::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	
	VkShaderModule shaderModule;
	if(vkCreateShaderModule(vulkanDevice->logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module");
	}
	return shaderModule;
}

void YasEngine::destroySwapchain()
{
	vulkanSwapchain.destroySwapchain(vulkanDevice->logicalDevice);
}

void YasEngine::cleanUp()
{
	cleanupSwapchain();	

	for(size_t i = 0; i<MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(vulkanDevice->logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(vulkanDevice->logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(vulkanDevice->logicalDevice, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(vulkanDevice->logicalDevice, commandPool, nullptr);
	for(VkFramebuffer framebuffer: swapchainFramebuffers)
	{
		vkDestroyFramebuffer(vulkanDevice->logicalDevice, framebuffer, nullptr);
	}
	vkDestroyPipeline(vulkanDevice->logicalDevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(vulkanDevice->logicalDevice, pipelineLayout, nullptr);
	vkDestroyRenderPass(vulkanDevice->logicalDevice, renderPass, nullptr);
	
	destroySwapchain();
	
	vkDestroyDevice(vulkanDevice->logicalDevice, nullptr);
	
	if(enableValidationLayers)
	{
		destroyDebugReportCallbackEXT(vulkanInstance.instance, callback, nullptr);
	}
	vkDestroySurfaceKHR(vulkanInstance.instance, surface, nullptr);
	//Second parameter is optional allocator which for this version will not be used.
	vkDestroyInstance(vulkanInstance.instance, nullptr);
	DestroyWindow(window);
}

//-----------------------------------------------------------------------------|---------------------------------------|