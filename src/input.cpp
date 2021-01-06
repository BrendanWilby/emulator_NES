#include "input.h"

bool Input::HandleInput(){
    SDL_Event sdlEvent;
    ImGuiIO& io = ImGui::GetIO(); 
    int wheel = 0;

    while(SDL_PollEvent(&sdlEvent) != 0){
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

        if(sdlEvent.type == SDL_QUIT){
            return false;
        }else if(sdlEvent.type == SDL_MOUSEWHEEL){
            wheel = sdlEvent.wheel.y;
        }
    }

    int mouseX;
    int mouseY;
    const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

    io.DeltaTime = 1.0f / 60.0f;
    io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
    io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
    io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
    io.MouseWheel = static_cast<float>(wheel);

    return true;
}