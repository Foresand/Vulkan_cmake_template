#include<stdVkTemplate.h>


void stdVkTemplate::run(){
    this->initWindow();
    this->initVulkan();
    this->mainLoop();
    this->cleanup();
}

void stdVkTemplate::initWindow(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    this->window = glfwCreateWindow(WIDTH, HEIGHT, "Render Result", nullptr, nullptr);
}

void stdVkTemplate::initVulkan(){
    this->createInstance();
    this->setupDebugMessenger();
    this->createSurface();
    this->pickPhysicalDevice();
    this->createLogicalDevice();
    this->createSwapChain();
    this->createImageViews();
    this->createRenderPass();
    this->createGraphicsPipeline();
    this->createFramebuffers();
    this->createCommandPool();
    this->createcommandBuffers();
    this->createSyncObjects();
}

void stdVkTemplate::createInstance(){
    if (enableValidationLayers && !stdVkTemplate::checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = this->getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        this->populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &this->instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

bool stdVkTemplate::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }
    return true;
}

std::vector<const char*> stdVkTemplate::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void stdVkTemplate::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = this->debugCallback;
}


void stdVkTemplate::setupDebugMessenger(){
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    this->populateDebugMessengerCreateInfo(createInfo);

    if (this->CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkResult stdVkTemplate::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL stdVkTemplate::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

void stdVkTemplate::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator){
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void stdVkTemplate::createSurface(){
    if(glfwCreateWindowSurface(instance,window,nullptr,&surface)!=VK_SUCCESS){
        throw std::runtime_error("failed to create window surface!");
    }
}

void stdVkTemplate::pickPhysicalDevice(){
    uint32_t deviceCount=0;
    vkEnumeratePhysicalDevices(this->instance,&deviceCount,nullptr);
    if(deviceCount==0){
        throw std::runtime_error("fail to find GPUs with Vulkan Support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(this->instance,&deviceCount,devices.data());
    for(const auto&device :devices){
        if(this->isDeviceSuitable(device)){
            this->physicalDevice=device;
            break;
        }
    }
    std::cout << "Available physical devices:\n";
    for (const auto& device : devices) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device, &props);
        std::cout << "- " << props.deviceName << "\n";
    }

    if(this->physicalDevice==VK_NULL_HANDLE){
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

bool stdVkTemplate::isDeviceSuitable(VkPhysicalDevice device){
    QueueFamilyIndices indices=this->findQueueFamilies(device);
    bool extensionsSupported=checkDeviceExtensionSupport(device);
    bool swapChainAdequate=false;
    if(extensionsSupported){
        SwapChainSupportDetails swapChainSupport=this->querySwapChainSupport(device);
        swapChainAdequate=!swapChainSupport.formats.empty()&&!swapChainSupport.presentModes.empty();
    }
    return indices.isComplete()&&extensionsSupported&&swapChainAdequate;
}

QueueFamilyIndices stdVkTemplate::findQueueFamilies(VkPhysicalDevice device){
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount=0;

    vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount,nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount,queueFamilies.data());
    int i=0;
    for(const auto&queueFamily:queueFamilies){
        if(queueFamily.queueCount>0&&queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily=i;
        }
        VkBool32 presentSupport=false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device,i,this->surface,&presentSupport);
        if(queueFamily.queueCount>0&&presentSupport){
            indices.presentFamily=i;
        }
        if(indices.isComplete()){
            break;
        }
        i++;
    }
    return indices;
}

bool stdVkTemplate::checkDeviceExtensionSupport(VkPhysicalDevice device){
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails stdVkTemplate::querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,this->surface,&details.capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device,this->surface,&formatCount,nullptr);

    if(formatCount!=0){
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device,this->surface,&formatCount,details.formats.data());
    }
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->surface, &presentModeCount, details.presentModes.data());
    }
    return details;
}

void stdVkTemplate::createLogicalDevice(){
    QueueFamilyIndices indices=this->findQueueFamilies(this->physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies={indices.graphicsFamily.value(),indices.presentFamily.value()};
    
    float queuePriority=1.0f;
    for(int queueFamily:uniqueQueueFamilies){
        VkDeviceQueueCreateInfo queueCreateInfo={};
        queueCreateInfo.sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex=indices.graphicsFamily.value();
        queueCreateInfo.queueCount=1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(this->physicalDevice, &createInfo, nullptr, &this->device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
    vkGetDeviceQueue(this->device, indices.graphicsFamily.value(), 0, &this->graphicsQueue);
    vkGetDeviceQueue(this->device, indices.presentFamily.value(), 0, &this->presentQueue);
}

void stdVkTemplate::createSwapChain(){
    SwapChainSupportDetails swapChainSupport=querySwapChainSupport(physicalDevice);
    VkSurfaceFormatKHR surfaceFormat=chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode=chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent=chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

VkSurfaceFormatKHR stdVkTemplate::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&availableFormats){
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR stdVkTemplate::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>availablePresentModes){
    VkPresentModeKHR bestMode=VK_PRESENT_MODE_FIFO_KHR;
    for(const auto& availablePresentMode:availablePresentModes){
        if(availablePresentMode==VK_PRESENT_MODE_MAILBOX_KHR){
            return availablePresentMode;
        }else if(availablePresentMode==VK_PRESENT_MODE_IMMEDIATE_KHR){
            bestMode=availablePresentMode;
        }
    }
    return bestMode;  
}

VkExtent2D stdVkTemplate::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities){
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
        return capabilities.currentExtent;
    }else{
        VkExtent2D actualExtent={WIDTH,HEIGHT};
        actualExtent.width=std::max(capabilities.minImageExtent.width,std::min(capabilities.maxImageExtent.width,actualExtent.width));
        actualExtent.height=std::max(capabilities.minImageExtent.height,std::min(capabilities.maxImageExtent.height,actualExtent.height));
        return actualExtent;
    }
}

void stdVkTemplate::createImageViews(){
    this->swapChainImageViews.resize(swapChainImages.size());
    for(size_t i=0; i<swapChainImages.size();i++){
        VkImageViewCreateInfo createInfo={};
        createInfo.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image=swapChainImages[i];
        createInfo.viewType=VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format=swapChainImageFormat;
        createInfo.components.r=VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g=VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b=VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a=VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel=0;
        createInfo.subresourceRange.levelCount=1;
        createInfo.subresourceRange.baseArrayLayer=0;
        createInfo.subresourceRange.layerCount=1;
        if(vkCreateImageView(this->device,&createInfo,nullptr,&this->swapChainImageViews[i])!=VK_SUCCESS){
            throw std::runtime_error("failed to create image view!");
        }
    }
}

void stdVkTemplate::createRenderPass(){
    VkAttachmentDescription colorAttachment={};
    colorAttachment.format=swapChainImageFormat;
    colorAttachment.samples=VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp=VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp=VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp=VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp=VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout=VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef={};
    colorAttachmentRef.attachment=0;
    colorAttachmentRef.layout=VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkSubpassDescription subpass={};
    subpass.pipelineBindPoint=VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount=1;
    subpass.pColorAttachments=&colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo={};
    renderPassInfo.sType=VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount=1;
    renderPassInfo.pAttachments=&colorAttachment;
    renderPassInfo.subpassCount=1;
    renderPassInfo.pSubpasses=&subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    
    if(vkCreateRenderPass(this->device,&renderPassInfo,nullptr,&this->renderPass)!=VK_SUCCESS){
        throw std::runtime_error("failed to create render pass!");
    }
}

void stdVkTemplate::createGraphicsPipeline(){
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    auto vertShaderCode=readFile(".shader/chap14/vert.spv");
    auto fragShaderCode=readFile(".shader/chap14/frag.spv");
    vertShaderModule=this->createShaderModule(vertShaderCode);
    fragShaderModule=this->createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo={};
    vertShaderStageInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage=VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module=vertShaderModule;
    vertShaderStageInfo.pName="main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo={};
    fragShaderStageInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage=VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module=fragShaderModule;
    fragShaderStageInfo.pName="main";

    VkPipelineShaderStageCreateInfo shaderStages[]={vertShaderStageInfo,fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo={};
    vertexInputInfo.sType=VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount=0;
    vertexInputInfo.pVertexBindingDescriptions=nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount=0;
    vertexInputInfo.pVertexAttributeDescriptions=nullptr;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor=0.0f;
    rasterizer.depthBiasClamp=0.0f;
    rasterizer.depthBiasSlopeFactor=0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading=1.0f;
    multisampling.pSampleMask=nullptr;
    multisampling.alphaToCoverageEnable=VK_FALSE;
    multisampling.alphaToOneEnable=VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor=VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor=VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp=VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor=VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor=VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp=VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts=nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges=nullptr;

    if (vkCreatePipelineLayout(this->device, &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo={};
    pipelineInfo.sType=VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount=2;
    pipelineInfo.pStages=shaderStages;

    pipelineInfo.pVertexInputState=&vertexInputInfo;
    pipelineInfo.pInputAssemblyState=&inputAssembly;
    pipelineInfo.pViewportState=&viewportState;
    pipelineInfo.pRasterizationState=&rasterizer;
    pipelineInfo.pMultisampleState=&multisampling;
    pipelineInfo.pDepthStencilState=nullptr;
    pipelineInfo.pColorBlendState=&colorBlending;
    pipelineInfo.pDynamicState=&dynamicState;
    pipelineInfo.layout=pipelineLayout;

    pipelineInfo.renderPass=renderPass;
    pipelineInfo.subpass=0;
    pipelineInfo.basePipelineHandle=VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex=-1;

    if(vkCreateGraphicsPipelines(this->device,VK_NULL_HANDLE,1,&pipelineInfo,nullptr,&this->graphicsPipeline)!=VK_SUCCESS){
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(this->device,fragShaderModule,nullptr);
    vkDestroyShaderModule(this->device,vertShaderModule,nullptr);
}

VkShaderModule stdVkTemplate::createShaderModule(const std::vector<char> & code){
    VkShaderModuleCreateInfo createInfo={};
    createInfo.sType=VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize=code.size();
    createInfo.pCode=reinterpret_cast<const uint32_t*>(code.data());
    VkShaderModule shaderModule;
    if(vkCreateShaderModule(this->device,&createInfo,nullptr,&shaderModule)!=VK_SUCCESS){
        throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
}

void stdVkTemplate::createFramebuffers(){
    swapChainFramebuffers.resize(this->swapChainImageViews.size());
    for(size_t i=0;i<this->swapChainImageViews.size();i++){
        VkImageView attachments[]={this->swapChainImageViews[i]};
        VkFramebufferCreateInfo framebufferInfo={};
        framebufferInfo.sType=VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass=this->renderPass;
        framebufferInfo.attachmentCount=1;
        framebufferInfo.pAttachments=attachments;
        framebufferInfo.width=this->swapChainExtent.width;
        framebufferInfo.height=this->swapChainExtent.height;
        framebufferInfo.layers=1;
        if(vkCreateFramebuffer(this->device,&framebufferInfo,nullptr,&this->swapChainFramebuffers[i])!=VK_SUCCESS){
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void stdVkTemplate::createCommandPool(){
    QueueFamilyIndices queueFamilyIndices=this->findQueueFamilies(physicalDevice);
    VkCommandPoolCreateInfo poolInfo={};
    poolInfo.sType=VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex=queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags=VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if(vkCreateCommandPool(device,&poolInfo,nullptr,&commandPool)!=VK_SUCCESS){
        throw std::runtime_error("failed to create command pool!");
    }
}

void stdVkTemplate::createcommandBuffers(){
    // commandBuffer.resize(swapChainFramebuffers.size());
    VkCommandBufferAllocateInfo allocInfo={};
    allocInfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool=this->commandPool;
    allocInfo.level=VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount=1;//(uint32_t)commandBuffers.size();
    if(vkAllocateCommandBuffers(this->device,&allocInfo,&this->commandBuffer)!=VK_SUCCESS){
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void stdVkTemplate::createSyncObjects(){
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo={};
    semaphoreInfo.sType=VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo={};
    fenceInfo.sType=VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags=VK_FENCE_CREATE_SIGNALED_BIT;

    for(size_t i =0;i<MAX_FRAMES_IN_FLIGHT;i++){
        if (vkCreateSemaphore(this->device, &semaphoreInfo, nullptr, &this->imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(this->device, &semaphoreInfo, nullptr, &this->renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(this->device, &fenceInfo, nullptr, &this->inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create semaphores for a frame!");
        }
    }
}


//==========================================================main loop=======================================================
void stdVkTemplate::mainLoop() {
    while (!glfwWindowShouldClose(this->window)) {
        glfwPollEvents();
        this->drawFrame();
    }
    vkDeviceWaitIdle(this->device);
}

void stdVkTemplate::drawFrame(){
    vkWaitForFences(this->device,1,&this->inFlightFences[this->currentFrame],VK_TRUE,std::numeric_limits<uint64_t>::max());
    vkResetFences(this->device,1,&this->inFlightFences[this->currentFrame]);
    uint32_t imageIndex;
    vkAcquireNextImageKHR(this->device,this->swapChain,std::numeric_limits<uint64_t>::max(),this->imageAvailableSemaphores[this->currentFrame],VK_NULL_HANDLE,&imageIndex);
    
    vkResetCommandBuffer(this->commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);
    this->recordCommandBuffer(this->commandBuffer, imageIndex);

    VkSubmitInfo submitInfo={};
    submitInfo.sType=VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[]={imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[]={VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount=1;
    submitInfo.pWaitSemaphores=waitSemaphores;
    submitInfo.pWaitDstStageMask=waitStages;
    submitInfo.commandBufferCount=1;
    submitInfo.pCommandBuffers=&this->commandBuffer;
    VkSemaphore signalSemaphores[]={this->renderFinishedSemaphores[this->currentFrame]};
    submitInfo.signalSemaphoreCount=1;
    submitInfo.pSignalSemaphores=signalSemaphores;
    if (vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, this->inFlightFences[this->currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {this->swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(this->presentQueue, &presentInfo);
    vkQueueWaitIdle(this->presentQueue);
    this->currentFrame=(this->currentFrame+1)%MAX_FRAMES_IN_FLIGHT;
}

void stdVkTemplate::recordCommandBuffer(VkCommandBuffer commandBuffer,uint32_t imageIndex){
    VkCommandBufferBeginInfo beginInfo={};
    beginInfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags=VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo=nullptr;
    if(vkBeginCommandBuffer(commandBuffer,&beginInfo)!=VK_SUCCESS){
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = this->renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = this->swapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;            
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(this->swapChainExtent.width);
    viewport.height = static_cast<float>(this->swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = this->swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

}

void stdVkTemplate::cleanup() {
    for(size_t i=0;i<MAX_FRAMES_IN_FLIGHT;i++){
        vkDestroySemaphore(this->device,this->renderFinishedSemaphores[i],nullptr);
        vkDestroySemaphore(this->device,this->imageAvailableSemaphores[i],nullptr);
        vkDestroyFence(this->device,this->inFlightFences[i],nullptr);
    }
    vkDestroyCommandPool(this->device,this->commandPool,nullptr);
    for(auto framebuffer:this->swapChainFramebuffers){
        vkDestroyFramebuffer(this->device,framebuffer,nullptr);
    }
    vkDestroyPipeline(this->device,this->graphicsPipeline,nullptr);
    vkDestroyPipelineLayout(this->device,this->pipelineLayout,nullptr);
    vkDestroyRenderPass(this->device,this->renderPass,nullptr);
    for(auto imageView:this->swapChainImageViews){
        vkDestroyImageView(this->device,imageView,nullptr);
    }
    vkDestroySwapchainKHR(this->device, this->swapChain,nullptr);
    vkDestroyDevice(this->device,nullptr);
    if (enableValidationLayers) {
        this->DestroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, nullptr);
    }
    vkDestroySurfaceKHR(this->instance,this->surface,nullptr);

    vkDestroyInstance(this->instance, nullptr);

    glfwDestroyWindow(this->window);

    glfwTerminate();
}
