#include "Game.h"
#include "Resources.h"
#include "SpriteRenderer.h"

SpriteRenderer *renderer; 

void Game::Init(){
    // load shaders
    Resources::LoadShader("src/vertex.vert", "src/fragment.frag", nullptr, "sprite");
    // shader config
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    Resources::GetShader("sprite").Use().SetInteger("image", 0);
    Resources::GetShader("sprite").SetMatrix4("projection", projection);

    renderer = new SpriteRenderer(Resources::GetShader("sprite"));
    Resources::LoadTexture("textures/board.png", false, "board");

}
void Game::Render(){
    renderer->DrawSprite(Resources::GetTexture("board"), glm::vec2(100.0f, 100.0f), glm::vec2(600.0f, 600.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}
