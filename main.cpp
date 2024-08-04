#include <array>
#include <iostream>
#include <string>
#include "include/raylib.h"
#define k_gravity 1  // 1 pixel per frame
using namespace std;

const int nWindowWidth{1024};
const int nWindowHeight{512};

// player dimensions

class GameObj {
 public:
  // Object box
  int nHeight;
  int nWidth;
  Color oColor;
  // Object sprite
  Rectangle oSpriteRect;
  Texture2D oSpriteTexture;
  Vector2 vSpritePos;
  // Object speed
  int nVelocity{0};
  int nY;
  int nX;
  int nJumpVelocity{-10};

  GameObj(int width, int height, Color color, int x, const char* TexturePath) {
    // Initialise box
    nWidth = width;
    nHeight = height;
    oColor = color;
    nX = x;
    nY = nWindowHeight - nHeight;
    // Initialise texture
    oSpriteTexture = LoadTexture(TexturePath);
    oSpriteRect.width = oSpriteTexture.width / 3;
    oSpriteRect.height = oSpriteTexture.height / 3;
    oSpriteRect.x = 0;
    oSpriteRect.y = 0;
    vSpritePos.x = nWindowWidth/2-oSpriteRect.width/2;
    vSpritePos.y = nWindowHeight-oSpriteRect.height;
  }

  ~GameObj() { UnloadTexture(oSpriteTexture); }

  void Draw() {
    DrawRectangle(nX, nY, nWidth, nHeight, oColor);
    DrawTextureRec(oSpriteTexture, oSpriteRect, vSpritePos, WHITE);
  }

  void ApplyGravity() {
    if (nY >= nWindowHeight - nHeight) {
      // Rectangle is on ground
      nVelocity = 0;
    } else {
      // Rectangle is in the air
      nVelocity += k_gravity;
    }
    nY += nVelocity;
  }

  void Jump() {
    if (nY >= nWindowHeight - nHeight) {
      // object on the ground
      nVelocity += nJumpVelocity;
      nY += nVelocity;
    }
  }
};

int main() {
  InitWindow(nWindowWidth, nWindowHeight, "Dasher Game");
  SetTargetFPS(60);
  GameObj oPlayer(10, 20, RED, nWindowWidth / 2, "assets/gnome.png");

  // Game Loop
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);

    if (IsKeyPressed(KEY_SPACE)) {
      oPlayer.Jump();
    }

    oPlayer.ApplyGravity();

    oPlayer.Draw();

    EndDrawing();
  }

  return 0;
}


