#pragma once

class Layer
{
public:
    virtual void OnAttach() = 0;
    virtual void OnDetach() = 0;
    virtual void Update(double delta) = 0;
    virtual void Render(double delta) = 0;
};
