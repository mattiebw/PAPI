#pragma once
#include "Core/Layer.h"

class PAPIGameLayer : public Layer
{
public:
    void OnAttach() override;
    void OnDetach() override;
    void Update(double delta) override;
    void Render(double delta) override;
};
