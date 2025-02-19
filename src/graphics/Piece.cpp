#include "Piece.hpp"
#include "Resources.hpp"
#include "../Game.hpp"

void Piece::Draw(SpriteRenderer *r){
    if(isClicked)
        r->DrawSprite(Resources::GetTexture(texture), localPos, size, 0.2f, glm::vec3(0.5, 0.5, 0.5));
    else r->DrawSprite(Resources::GetTexture(texture), localPos, size, 0.0f, glm::vec3((float)color));
    
}
Piece::Piece(int x, int y, bool color, char type, string texture, Game *g){
    pos.x = x;
    pos.y = y;
    this->color = color;
    this->type = type;
    this->texture = texture;
    size.x = g->boardW / 8;
    size.y = g->boardH / 8;
    localPos.x = ((g->Width - g->boardW) / 2) + (size.x * x);
    localPos.y = ((g->Height - g->boardH) / 2) + (size.y * y);

}
void Piece::UpdatePos(int x, int y, Game *g){
    pos.x = x;
    pos.y = y;
    localPos.x = ((g->Width - g->boardW) / 2) + (size.x * x);
    localPos.y = ((g->Height - g->boardH) / 2) + (size.y * y);
}
