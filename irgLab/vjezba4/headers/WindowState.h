#pragma once

#include <glm/glm.hpp>

class WindowState {
private:
    int windowWidth;
    int windowHeight;
    glm::vec2 cursorPosition;

public:
    WindowState(int windowWidth, int windowHeight);
    
    int getWindowWidth() const;
    int getWindowHeight() const;
    glm::vec2 getCursorPosition() const;
    void setWindowWidth(int w);
    void setWindowHeight(int h);
    void setCursorPosition(glm::vec2 p);
};