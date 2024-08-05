#include <array>
#include <iostream>
#include <string>

#include "include/raylib.h"
using namespace std;
// acceleration due to gravity (pixels/s^2)
const int nGravity{2'000};
const int nWindowWidth{1024};
const int nWindowHeight{512};

// player dimensions

class GameCharacter {
 public:
  // Object box
  Rectangle oBox;
  Color oColor;
  // Object sprite
  Rectangle oSpriteRect;
  Texture2D oSpriteTexture;
  Vector2 vSpritePos;
  // Object speed
  int nVelocity{0};
  int nJumpVelocity{-600};

  GameCharacter(int width, int height, int x, const char* texturePath) {
    // Initialise box
    oColor = RED;  // box color for debug
    oBox = (Rectangle){x, nWindowHeight - height, width, height};
    // Initialise texture
    oSpriteTexture = LoadTexture(texturePath);
    oSpriteRect =
        (Rectangle){0, 0, oSpriteTexture.width / 3, oSpriteTexture.height / 3};
    vSpritePos = (Vector2){oBox.x, oBox.y};
  }

  ~GameCharacter() { UnloadTexture(oSpriteTexture); }

  void Draw() {
    DrawRectangle(oBox.x, oBox.y, oBox.width, oBox.height, oColor);
    DrawTexturePro(oSpriteTexture, oSpriteRect, oBox, (Vector2){0, 0}, 0.0f,
                   WHITE);
  }

  void ApplyGravity(float fDeltaTime) {
    if (oBox.y >= nWindowHeight - oBox.height) {
      // Rectangle is on ground
      nVelocity = 0;
    } else {
      // Rectangle is in the air
      nVelocity += nGravity * fDeltaTime;
    }
    oBox.y += nVelocity * fDeltaTime;
    vSpritePos.y = oBox.y;
  }

  void Jump(float fDeltaTime) {
    if (oBox.y >= nWindowHeight - oBox.height) {
      // object on the ground
      nVelocity += nJumpVelocity;
      oBox.y += nVelocity * fDeltaTime;
      vSpritePos.y = oBox.y;
    }
  }
};

int main() {
  InitWindow(nWindowWidth, nWindowHeight, "Dasher Game");
  SetTargetFPS(60);
  GameCharacter oPlayer(72, 96, nWindowWidth / 2, "assets/gnome.png");

  // Game Loop
  while (!WindowShouldClose()) {
    const float fDeltaTime{GetFrameTime()};
    BeginDrawing();
    ClearBackground(WHITE);
    if (IsKeyPressed(KEY_SPACE)) {
      oPlayer.Jump(fDeltaTime);
    }
    oPlayer.ApplyGravity(fDeltaTime);
    oPlayer.Draw();
    EndDrawing();
  }

  return 0;
}
