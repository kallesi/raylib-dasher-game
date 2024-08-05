#include <array>
#include <iostream>
#include <random>
#include <string>

#include "include/raylib.h"
using namespace std;
// acceleration due to gravity (pixels/s^2)
const int nGravity{4'900};
const int nWindowWidth{600};
const int nWindowHeight{480};

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
  // Object speeds
  int nVelocityX{0};
  int nVelocityY{0};
  int nJumpVelocity{-1'600};

  // Animation control
  int nFrame{0};
  float fElapsedTime{0};
  const float fUpdateTime{0.2};

  GameCharacter(int width, int height, int x, int nDefaultVelocityX,
                const char* texturePath, int nSpriteRows, int nTextureFlip) {
    // Initialise box
    oColor = BLANK;  // box color for debug
    nVelocityX = nDefaultVelocityX;
    oBox = (Rectangle){(float)x, (float)(nWindowHeight - height), (float)width,
                       (float)height};
    // Initialise texture
    oSpriteTexture = LoadTexture(texturePath);
    oSpriteRect = (Rectangle){0, 0, nTextureFlip * (float)(oSpriteTexture.width / 3),
                              (float)(oSpriteTexture.height / nSpriteRows)};
    vSpritePos = (Vector2){oBox.x, oBox.y};
  }

  ~GameCharacter() { UnloadTexture(oSpriteTexture); }

  void Draw(float fDeltaTime) {
    fElapsedTime += fDeltaTime;
    if (fElapsedTime >= fUpdateTime && oBox.y >= nWindowHeight - oBox.height) {
      nFrame++;
      fElapsedTime = 0;
    }
    if (nFrame > 2) {
      nFrame = 0;
    }
    oSpriteRect.x = nFrame * oSpriteRect.width;
    DrawRectangle(oBox.x, oBox.y, oBox.width, oBox.height, oColor);
    DrawTexturePro(oSpriteTexture, oSpriteRect, oBox, (Vector2){0, 0}, 0.0f,
                   WHITE);
  }

  void ApplyGravity(float fDeltaTime) {
    if (oBox.y >= nWindowHeight - oBox.height) {
      // Rectangle is on ground
      nVelocityY = 0;
    } else {
      // Rectangle is in the air
      nVelocityY += nGravity * fDeltaTime;
    }
    oBox.y += nVelocityY * fDeltaTime;
    vSpritePos.y = oBox.y;
  }

  void Jump(float fDeltaTime) {
    if (oBox.y >= nWindowHeight - oBox.height) {
      // object on the ground
      nVelocityY += nJumpVelocity;
      oBox.y += nVelocityY * fDeltaTime;
      vSpritePos.y = oBox.y;
    }
  }

  void KnightMoveLeft(float fDeltaTime) {
    oBox.x += nVelocityX * fDeltaTime;
    vSpritePos.x = oBox.x;
    if (oBox.x < -(oBox.width)) {
      // Push enemy out to the void
      random_device rd;
      mt19937 gen(rd());
      uniform_real_distribution<> dis(0.0, 1.0);
      float fRandom = dis(gen);
      oBox.x = nWindowWidth + (fRandom * 2 * nWindowWidth);
    }
  }
};

class Background {
 public:
  Vector2 vPos1{0, 0};
  Vector2 vPos2{0, 0};
  Texture2D oTexture;
  float fScale;
  int nScrollSpeed;

  Background(const char* texturePath, float scale, int speed = 80) {
    oTexture = LoadTexture(texturePath);
    fScale = scale;
    nScrollSpeed = speed;
    if (speed > 0) {
      vPos2.x = vPos1.x - oTexture.width * fScale + 2;
    } else {
      vPos2.x = vPos1.x + oTexture.width * fScale - 2;
    }
  }
  ~Background() { UnloadTexture(oTexture); }

  void Draw(float fDeltaTime) {
    vPos1.x += nScrollSpeed * fDeltaTime;
    vPos2.x += nScrollSpeed * fDeltaTime;
    if (nScrollSpeed > 0) {
      if (vPos1.x >= nWindowWidth) {
        vPos1.x = -(oTexture.width * fScale);
      }
      if (vPos2.x >= nWindowWidth) {
        vPos2.x = -(oTexture.width * fScale);
      }
    } else {
      if (vPos1.x <= -(oTexture.width * fScale)) {
        vPos1.x = vPos2.x + oTexture.width * fScale - 1;
      }
      if (vPos2.x <= -(oTexture.width * fScale)) {
        vPos2.x = vPos1.x + oTexture.width * fScale - 1;
      }
    }
    DrawTextureEx(oTexture, vPos1, 0.0f, fScale, WHITE);
    DrawTextureEx(oTexture, vPos2, 0.0f, fScale, WHITE);
  }
};

bool fnCheckOverlap(Rectangle rect1, Rectangle rect2, float xThreshold,
                    float yThreshold) {
  // Calculate the overlap area on x-axis
  float xOverlap = max(0.0f, min(rect1.x + rect1.width, rect2.x + rect2.width) -
                                 max(rect1.x, rect2.x));
  // Calculate the overlap area on y-axis
  float yOverlap =
      max(0.0f, min(rect1.y + rect1.height, rect2.y + rect2.height) -
                    max(rect1.y, rect2.y));
  // Calculate the percentage overlap on x-axis
  float xPercentage = xOverlap / std::min(rect1.width, rect2.width);
  // Calculate the percentage overlap on y-axis
  float yPercentage = yOverlap / std::min(rect1.height, rect2.height);
  // Check if the overlap percentage meets the thresholds for both axes
  return (xPercentage >= xThreshold) && (yPercentage >= yThreshold);
}

int main() {
  InitWindow(nWindowWidth, nWindowHeight, "Gnome Runner");
  SetTargetFPS(120);

  GameCharacter oPlayer(120, 140, nWindowWidth * 0.1f, 0, "assets/gnome.png", 3,
                        -1);
  GameCharacter oKnight(150, 150, nWindowWidth * 1.1f, -400,
                        "assets/knight.png", 1, 1);

  Background bgTrees1("assets/trees-layer-1.png", 6.0f, -60);
  Background bgTrees2("assets/trees-layer-2.png", 6.0f, 50);
  Background bgTrees3("assets/trees-layer-3.png", 6.0f, +40);
  Background bgCastle("assets/castle.png", 2.0f, -80);

  // Game Loop
  while (!WindowShouldClose()) {
    const float fDeltaTime{GetFrameTime()};
    BeginDrawing();
    ClearBackground(BLACK);
    bgTrees1.Draw(fDeltaTime);
    bgTrees2.Draw(fDeltaTime);
    bgTrees3.Draw(fDeltaTime);
    bgCastle.Draw(fDeltaTime);

    if (IsKeyPressed(KEY_SPACE)) {
      oPlayer.Jump(fDeltaTime);
    }
    oPlayer.ApplyGravity(fDeltaTime);

    if (fnCheckOverlap(oPlayer.oBox, oKnight.oBox, 0.65, 0.2)) {
      DrawText("Game Over", nWindowWidth * 0.2, nWindowHeight * 0.5, 60, RED);
      if (IsKeyPressed(KEY_SPACE)) {
        break;
      }
    } else {
      oPlayer.Draw(fDeltaTime);
      oKnight.Draw(fDeltaTime);
      oKnight.KnightMoveLeft(fDeltaTime);
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
