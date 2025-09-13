/*******************************************************************************************
 *
 * Game-jam game made for minijam 193 using raylib, aseprite,and code -OSS
 *
 ********************************************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_WEB)
#include <emscripten.h>
#endif

//--
// Local Variables Definition (local to this module)
//--
Camera2D camera = {0};
const int screenWidth = 800;
const int screenHeight = 450;
float padding = 10.0f;
bool playerTurn = true;
bool enemyTurn = false;

typedef struct {
  Rectangle base;
  Color color;
  float health, damage;
} Player;
Player *player = NULL;

typedef struct {
  Rectangle base;
  Color color;
  float health, damage;
} Enemy;
Enemy *enemy = NULL;

//--
// Local Functions Declaration
//--
static void UpdateDrawFrame(void);       // Update and draw one frame
static void UpdatePlayerHealthBar(void); // Update and draw player health bar
static void UpdateEnemyHealthBar(void);  // Update and draw player health bar

//--
// Main entry point
//--
int main() {

  InitWindow(screenWidth, screenHeight, "mini193");

  player = malloc(sizeof(Player));
  if (player == NULL) {
    printf("Error: failed to allocate memory for player");
  }
  player->base = (Rectangle){150, 200, 50, 75};
  player->color = GREEN;
  player->health = 100.0f;
  player->damage = 10.0f;

  enemy = malloc(sizeof(Player));
  if (enemy == NULL) {
    printf("Error: failed to allocate memory for enemy");
  }
  enemy->base = (Rectangle){550, 200, 50, 75};
  enemy->color = RED;
  enemy->health = 150.0f;
  enemy->damage = 10.0f;

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    UpdateDrawFrame();
  }
#endif

  // De-Init
  CloseWindow(); // Close window and OpenGL context

  return 0;
}

// Update and draw game frame
static void UpdateDrawFrame(void) {
  // Update
  if (IsKeyPressed(KEY_SPACE)) {
    player->health -= 10;
  }
  if (IsKeyPressed(KEY_Q)) {
    enemy->health -= 10;
  }

  // Draw
  BeginDrawing();

  ClearBackground(SKYBLUE);

  // floor
  DrawRectangle(0, screenHeight / 2, screenWidth, screenHeight / 2, ORANGE);
  // player
  DrawRectangle(player->base.x, player->base.y, player->base.width,
                player->base.height, player->color); // Player
  UpdatePlayerHealthBar();

  // Enemy
  DrawRectangle(enemy->base.x, enemy->base.y, enemy->base.width,
                enemy->base.height, enemy->color);
  UpdateEnemyHealthBar();

  char text[] = "1 to Attack";
  int fontSize = 20;
  DrawText(text, fontSize - padding, player->base.y - fontSize - padding,
           fontSize, RAYWHITE);

  DrawFPS(10, 10);

  EndDrawing();
}

static void UpdatePlayerHealthBar(void) {
  // Define health bar dimensions and position
  int barX = player->base.x - 25; // X position
  int barY = player->base.y - 50; // Y position (above player)
  int barWidth = 100;             // Bar width in pixels
  int barHeight = 20;             // Bar height in pixels

  // Draw bar background and border
  DrawRectangle(barX - 2, barY - 2, barWidth + 4, barHeight + 4,
                DARKGRAY);                               // Border
  DrawRectangle(barX, barY, barWidth, barHeight, BLACK); // Background

  // Calculate how much of the bar to fill based on health (0-100)
  // Ensure health is within valid range
  float clampedHealth = player->health;
  if (clampedHealth < 0.0f)
    clampedHealth = 0.0f;
  if (clampedHealth > 100.0f)
    clampedHealth = 100.0f;

  int fillWidth = (int)((clampedHealth / 100.0f) * barWidth);

  // Choose bar color based on health level
  Color fillColor;
  if (clampedHealth > 70.0f) {
    fillColor = GREEN; // Healthy - green color
  } else if (clampedHealth > 30.0f) {
    fillColor = YELLOW; // Moderate - yellow color
  } else {
    fillColor = RED; // Critical - red color
  }

  // Draw the filled portion of the bar (only if there's health to show)
  if (fillWidth > 0) {
    DrawRectangle(barX, barY, fillWidth, barHeight, fillColor);
  }

  // Draw text labels for the health bar
  const char *text = "Health";
  int fontSize = 12;
  int textWidth = MeasureText(text, fontSize);
  DrawText(text, barX + (barWidth - textWidth) / 2, barY - fontSize - 5,
           fontSize, DARKGRAY);

  // Draw health percentage text
  char healthText[16];
  snprintf(healthText, sizeof(healthText), "%.0f%%", clampedHealth);
  int healthTextWidth = MeasureText(healthText, 10);
  DrawText(healthText, barX + (barWidth - healthTextWidth) / 2,
           barY + (barHeight - 10) / 2, 10, WHITE);
}

static void UpdateEnemyHealthBar(void) {
  // Define health bar dimensions and position
  int barX = enemy->base.x - 25; // X position
  int barY = enemy->base.y - 50; // Y position (above enemy)
  int barWidth = 100;            // Bar width in pixels
  int barHeight = 20;            // Bar height in pixels

  // Draw bar background and border
  DrawRectangle(barX - 2, barY - 2, barWidth + 4, barHeight + 4,
                DARKGRAY);                               // Border
  DrawRectangle(barX, barY, barWidth, barHeight, BLACK); // Background

  // Calculate how much of the bar to fill based on health (0-100)
  // Ensure health is within valid range
  float clampedHealth = enemy->health;
  if (clampedHealth < 0.0f)
    clampedHealth = 0.0f;
  if (clampedHealth > 100.0f)
    clampedHealth = 100.0f;

  int fillWidth = (int)((clampedHealth / 100.0f) * barWidth);

  // Choose bar color based on health level
  Color fillColor;
  if (clampedHealth > 70.0f) {
    fillColor = GREEN; // Healthy - green color
  } else if (clampedHealth > 30.0f) {
    fillColor = YELLOW; // Moderate - yellow color
  } else {
    fillColor = RED; // Critical - red color
  }

  // Draw the filled portion of the bar (only if there's health to show)
  if (fillWidth > 0) {
    DrawRectangle(barX, barY, fillWidth, barHeight, fillColor);
  }

  // Draw text labels for the health bar
  const char *text = "Health";
  int fontSize = 12;
  int textWidth = MeasureText(text, fontSize);
  DrawText(text, barX + (barWidth - textWidth) / 2, barY - fontSize - 5,
           fontSize, DARKGRAY);

  // Draw health percentage text
  char healthText[16];
  snprintf(healthText, sizeof(healthText), "%.0f%%", clampedHealth);
  int healthTextWidth = MeasureText(healthText, 10);
  DrawText(healthText, barX + (barWidth - healthTextWidth) / 2,
           barY + (barHeight - 10) / 2, 10, WHITE);
}
