// #define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

class HelloTriangleApplication
{
private:
    const uint32_t Width{800};
    const uint32_t Height{600};

    GLFWwindow* Window=nullptr;

public:
    void Run()
    {
        InitWindow();
        InitVulkan();
        MainLoop();
        Cleanup();
    }

private:
    void InitWindow()
    {
        glfwInit();

        // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // it is not OpenGL
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);   // Not resizable

        Window = glfwCreateWindow(Width, Height, "Vulkan", nullptr, nullptr);
    }

    void InitVulkan()
    {

    }

    void MainLoop()
    {
        while (!glfwWindowShouldClose(Window))
        {
            glfwPollEvents();
        }
    }

    void Cleanup()
    {
        glfwDestroyWindow(Window);

        glfwTerminate();
    }
};

int main()
{
    HelloTriangleApplication App;

    try {
        App.Run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

