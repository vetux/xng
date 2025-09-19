/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "xng/adapters/vulkan/vulkan.hpp"

#include <vulkan/vulkan.h>

#include "vkerror.hpp"

#include <cstring>
#include <iostream>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                            "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

namespace xng::vulkan {
    struct InstanceDataImp : public Vulkan::InstanceData {
        VkInstance instance{};
        VkDebugUtilsMessengerEXT debugMessenger{};
    };

    static InstanceDataImp &getData(Vulkan::InstanceData *data) {
        return down_cast<InstanceDataImp &>(*data);
    }

    Vulkan::Vulkan(DisplayEnvironment &displayDriver) {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "xEngine Application";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "xEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        // auto extensions = displayDriver.getRequiredVulkanExtensions();

        auto extensions = std::vector<const char *>{};

        // extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME); // Not supported on android

#ifndef NDEBUG
        //extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

        for (auto &ext: extensions) {
            bool found = false;
            for (auto &aExt: availableExtensions) {
                if (strcmp(aExt.extensionName, ext) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                throw std::runtime_error("Required extension unavailable: " + std::string(ext));
            }
        }

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        createInfo.enabledLayerCount = 0;

        // createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR; // Not supported on android

#ifdef NDEBUG
        createInfo.enabledLayerCount = 0;
#else
        // Make sure to install the vulkan-validationlayers-dev package if using debian 12
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        std::vector<const char *> validationLayers{
            "VK_LAYER_KHRONOS_validation"
        };

        for (auto &layer: validationLayers) {
            bool found = false;
            for (auto &l: availableLayers) {
                if (strcmp(l.layerName, layer) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                throw std::runtime_error("Failed to find validation layer: " + std::string(layer));
            }
        }

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
#endif

        InstanceDataImp data{};

        auto result = vkCreateInstance(&createInfo, nullptr, &data.instance);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create instance: " + vkResultToString(result));
        }

#ifndef NDEBUG
        VkDebugUtilsMessengerCreateInfoEXT msgCreateInfo{};
        msgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        msgCreateInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        msgCreateInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        msgCreateInfo.pfnUserCallback = debugCallback;
        msgCreateInfo.pUserData = nullptr; // Optional

        auto dResult = CreateDebugUtilsMessengerEXT(data.instance, &msgCreateInfo, nullptr, &data.debugMessenger);
        if (dResult != VK_SUCCESS) {
            throw std::runtime_error("Failed to set up debug messenger: " + vkResultToString(dResult));
        }
#endif

        instanceData = std::make_shared<InstanceData>(data);
    }

    Vulkan::~Vulkan() {
#ifndef NDEBUG
        DestroyDebugUtilsMessengerEXT(getData(instanceData.get()).instance,
                                      getData(instanceData.get()).debugMessenger,
                                      nullptr);
#endif
        vkDestroyInstance(getData(instanceData.get()).instance, nullptr);
    }

    void Vulkan::setWindow(Window &window) {
    }

    Vulkan::GraphHandle Vulkan::compile(const RenderGraph &graph) {
        throw std::runtime_error("Not implemented");
    }

    void Vulkan::recompile(GraphHandle handle, const RenderGraph &graph) {
    }

    void Vulkan::execute(GraphHandle graph) {
    }

    void Vulkan::execute(const std::vector<GraphHandle> &graphs) {
    }

    void Vulkan::saveCache(Vulkan::GraphHandle graph, std::ostream &stream) {
    }

    void Vulkan::loadCache(Vulkan::GraphHandle graph, std::istream &stream) {
    }
}
