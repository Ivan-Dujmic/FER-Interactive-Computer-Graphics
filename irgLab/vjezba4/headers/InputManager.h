#pragma once

#include <functional>
struct GLFWwindow;

namespace InputManager {
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void mouse_callback(GLFWwindow* window, int x, int y);
    void scroll_callback(GLFWwindow* window, int x, int y);
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    void poll_events();
    
    using MovableCallback = std::function<void(float dx, float dy)>;
    void register_movable(MovableCallback callback);
};