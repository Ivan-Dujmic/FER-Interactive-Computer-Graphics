#include "WindowState.h"

WindowState::WindowState(int windowWidth, int windowHeight) :
    windowWidth(windowWidth),
    windowHeight(windowHeight),
    cursorPosition(0, 0)
{}
    
int WindowState::getWindowWidth() const {
    return windowWidth;
};

int WindowState::getWindowHeight() const {
    return windowHeight;
};

glm::vec2 WindowState::getCursorPosition() const {
    return cursorPosition;
};

void WindowState::setWindowWidth(int w) {
    windowWidth = w;
}

void WindowState::setWindowHeight(int h) {
    windowHeight = h;
}

void WindowState::setCursorPosition(glm::vec2 p) {
    cursorPosition = p;
}