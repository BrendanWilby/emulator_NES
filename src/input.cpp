#include "input.h"

bool Input::HandleInput(){
    SDL_Event sdlEvent;

    while(SDL_PollEvent(&sdlEvent) != 0){
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

        if(sdlEvent.type == SDL_QUIT){
            return false;
        }
    }
    return true;
}