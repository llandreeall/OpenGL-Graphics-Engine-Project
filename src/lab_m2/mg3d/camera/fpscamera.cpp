#include "fpscamera.h"

#include <iostream>

#include "core/engine.h"
#include "utils/memory_utils.h"


m2::FPSCamera::FPSCamera(Camera* cam)
{
    this->cam = cam;
}


void m2::FPSCamera::OnInputUpdate(float deltaTime, int mods)
{
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) return;

    if (window->GetSpecialKeyState() & GLFW_MOD_SHIFT)
    {
        deltaTime *= 2;
    }

    if (window->KeyHold(GLFW_KEY_W))            cam->MoveForward(deltaTime);
    if (window->KeyHold(GLFW_KEY_S))            cam->MoveBackward(deltaTime);
    if (window->KeyHold(GLFW_KEY_A))            cam->MoveLeft(deltaTime);
    if (window->KeyHold(GLFW_KEY_D))            cam->MoveRight(deltaTime);
    if (window->KeyHold(GLFW_KEY_Q))            cam->MoveDown(deltaTime);
    if (window->KeyHold(GLFW_KEY_E))            cam->MoveUp(deltaTime);

    if (window->KeyHold(GLFW_KEY_KP_MULTIPLY))  cam->UpdateSpeed();
    if (window->KeyHold(GLFW_KEY_KP_DIVIDE))    cam->UpdateSpeed(-0.2f);

    if (window->KeyHold(GLFW_KEY_KP_4))         cam->RotateOY(500 * deltaTime);
    if (window->KeyHold(GLFW_KEY_KP_6))         cam->RotateOY(-500 * deltaTime);
    if (window->KeyHold(GLFW_KEY_KP_8))         cam->RotateOX(700 * deltaTime);
    if (window->KeyHold(GLFW_KEY_KP_5))         cam->RotateOX(-700 * deltaTime);

    cam->Update();
}


void m2::FPSCamera::OnKeyPress(int key, int mods) {
    if (mods)
    {
        return;
    }

    if (key == GLFW_KEY_C)
    {
        cam->Log();
    }
}


void m2::FPSCamera::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        cam->RotateOY(-(float)deltaX);
        cam->RotateOX(-(float)deltaY);
        cam->Update();
    }
}


void m2::FPSCamera::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
    {
        window->DisablePointer();
    }
}


void m2::FPSCamera::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
    {
        window->ShowPointer();
    }
}

Camera* m2::FPSCamera::GetCamera()
{
    return cam;
}

