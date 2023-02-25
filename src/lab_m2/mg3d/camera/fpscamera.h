#pragma once
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <vector>
#include "../../../components/camera.h"
#include "../../../core/window/input_controller.h"
using namespace gfxc;

namespace m2
{
   
    class FPSCamera : public InputController
    {
        public:
            explicit FPSCamera(Camera* cam);
            void OnInputUpdate(float deltaTime, int mods) override;
            void OnKeyPress(int key, int mods) override;
            void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
            void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
            void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
            Camera* GetCamera();

        public:
            Camera* cam;
    };
}
