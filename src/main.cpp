#include "merian-nodes/graph/graph.hpp"
#include "merian-nodes/nodes/glfw_window/glfw_window.hpp"
#include "merian-nodes/nodes/image_write/image_write.hpp"
#include "merian-nodes/nodes/shadertoy/shadertoy.hpp"
#include "merian/utils/configuration_imgui.hpp"
#include "merian/vk/context.hpp"
#include "merian/vk/extension/extension_resources.hpp"
#include "merian/vk/extension/extension_vk_debug_utils.hpp"
#include "merian/vk/extension/extension_vk_glfw.hpp"
#include "merian/vk/shader/shader_compiler_shaderc.hpp"
#include "merian/vk/window/glfw_imgui.hpp"

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::debug);

    if (argc != 2) {
        SPDLOG_INFO("Usage: merian-shadertoy <path/to/shader.glsl>");
        return 0;
    }

    // Setup Vulkan context.
    const auto debug_utils = std::make_shared<merian::ExtensionVkDebugUtils>(false);
    const auto extGLFW = std::make_shared<merian::ExtensionVkGLFW>();
    const auto resources = std::make_shared<merian::ExtensionResources>();
    const std::vector<std::shared_ptr<merian::Extension>> extensions = {extGLFW, resources,
                                                                        debug_utils};
    const merian::SharedContext context =
        merian::Context::make_context(extensions, "merian-shadertoy");

    merian::ResourceAllocatorHandle alloc = resources->resource_allocator();
    merian::QueueHandle queue = context->get_queue_GCT();
    merian::ShaderCompilerHandle compiler = std::make_shared<merian::ShadercCompiler>();

    // Setup processing graph.
    merian_nodes::Graph graph{context, alloc};
    auto shadertoy = std::make_shared<merian_nodes::Shadertoy>(context, argv[1], compiler);
    auto window = std::make_shared<merian_nodes::GLFWWindow>(context);
    auto image_writer = std::make_shared<merian_nodes::ImageWrite>(context, alloc);

    graph.add_node(shadertoy, "shadertoy");
    graph.add_node(window, "window");
    graph.add_node(image_writer, "image out");
    graph.add_connection(shadertoy, window, "out", "src");
    graph.add_connection(shadertoy, image_writer, "out", "src");

    // Setup debug window, by hooking into the window node.
    merian::ImGuiContextWrapperHandle debug_ctx = std::make_shared<merian::ImGuiContextWrapper>();
    merian::GLFWImGui imgui(context, debug_ctx, true);
    merian::ImGuiConfiguration config;
    merian::Stopwatch frametime;
    window->set_on_blit_completed(
        [&](const vk::CommandBuffer& cmd, merian::SwapchainAcquireResult& acquire_result) {
            imgui.new_frame(queue, cmd, *window->get_window(), acquire_result);

            const double frametime_ms = frametime.millis();
            frametime.reset();
            ImGui::Begin(fmt::format("Shadertoy ({:.02f}ms, {:.02f} fps)###ShadertoyWindow",
                                     frametime_ms, 1000 / frametime_ms)
                             .c_str(),
                         NULL, ImGuiWindowFlags_NoFocusOnAppearing);

            graph.configuration(config);

            ImGui::End();
            imgui.render(cmd);
        });

    // Run the graph
    while (!window->get_window()->should_close()) {
        glfwPollEvents();
        graph.run();
    }

    return 0;
}
