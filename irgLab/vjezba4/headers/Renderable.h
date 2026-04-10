#pragma once

class Renderable {
protected:
    int VAO;

public:
    Renderable() : VAO(0) {}
    virtual ~Renderable() = default;

    virtual void draw() = 0;
    
    int getVAO();
};