#include "merian-nodes/graph/graph.hpp"
#include "merian-nodes/nodes/glfw_window/glfw_window.hpp"
#include "merian/utils/properties_imgui.hpp"
#include "merian/utils/properties_json_dump.hpp"
#include "merian/utils/properties_json_load.hpp"
#include "merian/vk/context.hpp"
#include "merian/vk/extension/extension_glfw.hpp"
#include "merian/vk/extension/extension_resources.hpp"
#include "merian/vk/extension/extension_vk_debug_utils.hpp"
#include "merian/vk/window/glfw_imgui.hpp"

int main() {
    spdlog::set_level(spdlog::level::debug);

    // Setup Vulkan context.
    const auto debug_utils = std::make_shared<merian::ExtensionVkDebugUtils>(false);
    const auto extGLFW = std::make_shared<merian::ExtensionGLFW>();
    const auto resources = std::make_shared<merian::ExtensionResources>();
    const auto core = std::make_shared<merian::ExtensionVkCore>();
    const std::vector<std::shared_ptr<merian::Extension>> extensions = {extGLFW, resources,
                                                                        debug_utils, core};
    const merian::ContextHandle context = merian::Context::create(extensions, "merian-shadertoy");

    merian::ResourceAllocatorHandle alloc = resources->resource_allocator();
    merian::QueueHandle queue = context->get_queue_GCT();

    // Setup processing graph.
    merian_nodes::Graph graph{context, alloc};

    auto config_file = context->file_loader.find_file("graph_config.json");
    if (!config_file) {
        throw std::runtime_error{"graph_config.json not found"};
    }
    SPDLOG_INFO("Using config file: {}", config_file->string());
    auto load = merian::JSONLoadProperties(*config_file);
    graph.properties(load);

    std::shared_ptr<merian_nodes::GLFWWindow> window =
        graph.find_node_for_identifier_and_type<merian_nodes::GLFWWindow>("output");

    // Setup debug window, by hooking into the window node.
    merian::ImGuiContextWrapperHandle debug_ctx = std::make_shared<merian::ImGuiContextWrapper>();
    merian::GLFWImGui imgui(context, debug_ctx, true);
    merian::ImGuiProperties config;
    merian::Stopwatch frametime;
    if (window)
        window->set_on_blit_completed(
            [&](const vk::CommandBuffer& cmd, merian::SwapchainAcquireResult& acquire_result) {
                imgui.new_frame(queue, cmd, *window->get_window(), acquire_result);

                const double frametime_ms = frametime.millis();
                frametime.reset();
                ImGui::Begin(fmt::format("Shadertoy ({:.02f}ms, {:.02f} fps)###ShadertoyWindow",
                                         frametime_ms, 1000 / frametime_ms)
                                 .c_str(),
                             NULL, ImGuiWindowFlags_NoFocusOnAppearing);

                graph.properties(config);

                ImGui::End();
                imgui.render(cmd);
            });

    // Run the graph
    while (window && !window->get_window()->should_close()) {
        glfwPollEvents();
        graph.run();
    }

    auto dump = merian::JSONDumpProperties(*config_file);
    graph.properties(dump);

    return 0;
}
