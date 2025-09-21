/**
 *
 * * Game-jam game made for minijam 193 using raylib, aseprite,and code -OSS
 *
 *
 */

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
bool roundWon = false;
int gameState = 0; // 0 = playing, 1 = win screen, 2 = game over
int enemyMaxHealth = 150;
int potions = 10;

typedef struct {
  Rectangle base;
  Color color;
  float health, damage;
  int score;
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
int main(void);
static void UpdateDrawFrame(void);       // Update and draw one frame
static void UpdatePlayerHealthBar(void); // Update and draw player health bar
static void UpdateEnemyHealthBar(void);  // Update and draw enemy health bar
static void ManageTurns(void);
static void DrawGameOver(void);
static void DrawGameWin(void);
static void UpdatePlayerScore(void);
static void InitEnemy(void);
static void InitPlayer(void);
static void NextEnemy(void);
static void ControlText(void);

//--
// Main entry point
//--
int main(void) {

  InitWindow(screenWidth, screenHeight, "mini193");

  SetRandomSeed(time(NULL));

  InitPlayer();
  InitEnemy();

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
  free(player);
  free(enemy);

  return 0;
}

// Update and draw game frame
static void UpdateDrawFrame(void) {
  // Update
  if (player->health > 0 && enemy->health > 0 && gameState == 0) {
    ManageTurns();
  }

  // Check for game state changes
  if (player->health <= 0 && gameState == 0) {
    gameState = 2; // Game over
  }

  if (enemy->health <= 0 && gameState == 0) {
    roundWon = true;
    gameState = 1; // Win screen
    UpdatePlayerScore();
  }

  // Handle win screen input
  if (gameState == 1 && IsKeyPressed(KEY_SPACE)) {
    NextEnemy();
    gameState = 0; // Back to playing
  }

  if (playerTurn && IsKeyPressed(KEY_TWO) && potions > 0) {
    player->health += 20.0f;
    potions -= 1;
  }

  // Draw
  BeginDrawing();

  ClearBackground(SKYBLUE);

  if (gameState == 2) {
    DrawGameOver();
  } else if (gameState == 1) {
    DrawGameWin();
  } else {
    // Normal gameplay drawing
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

    ControlText();
  }

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
  snprintf(healthText, sizeof(healthText), "%.0f", clampedHealth);
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

  // FIXED: Use enemy->health instead of enemyMaxHealth
  float clampedHealth = enemy->health;
  if (clampedHealth < 0.0f)
    clampedHealth = 0.0f;
  if (clampedHealth > enemyMaxHealth)
    clampedHealth = enemyMaxHealth;

  int fillWidth = (int)((clampedHealth / (float)enemyMaxHealth) * barWidth);

  // Choose bar color based on health level
  Color fillColor;
  if (clampedHealth > enemyMaxHealth / 1.5f) {
    fillColor = GREEN; // Healthy - green color
  } else if (clampedHealth > (float)enemyMaxHealth / 2.0f) {
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
  snprintf(healthText, sizeof(healthText), "%.0f", clampedHealth);
  int healthTextWidth = MeasureText(healthText, 10);
  DrawText(healthText, barX + (barWidth - healthTextWidth) / 2,
           barY + (barHeight - 10) / 2, 10, WHITE);
}

static void ManageTurns(void) {
  if (playerTurn) {
    if (IsKeyPressed(KEY_ONE)) { // Changed to IsKeyPressed for better control
      int hitNumber = GetRandomValue(1, 2);
      player->damage = GetRandomValue(10, 20);
      if (hitNumber == 1) {
        enemy->health -= player->damage;
        playerTurn = false;
      }
    }
  } else {
    int hitNumber = GetRandomValue(1, 10);
    enemy->damage = GetRandomValue(5, 10);
    if (hitNumber == 1) {
      player->health -= enemy->damage;
      playerTurn = true;
    }
  }
}

static void DrawGameOver(void) {
  ClearBackground(BLACK);
  int fontSize = 60;

  DrawText("GAME OVER", (screenWidth / 2) - (MeasureText("GAME OVER", fontSize) / 2),
           (screenHeight / 2) - (fontSize / 2), fontSize, RED);

  fontSize = 24;
  DrawText(TextFormat("Score: %d", player->score), (screenWidth / 2) - (MeasureText("Score: %d", fontSize) / 2),
           (screenHeight / 2) + 40, fontSize, GREEN);
}

static void DrawGameWin(void) {
  ClearBackground(BLACK);
  int fontSize = 60;

  DrawText("FOE SLAIN", (screenWidth / 2) - (MeasureText("FOE SLAIN", fontSize) / 2),
           (screenHeight / 2) - (fontSize / 2), fontSize, GREEN);

  fontSize = 24;
  DrawText(TextFormat("Score: %d", player->score), (screenWidth / 2) - (MeasureText("Score: %d", fontSize) / 2),
           (screenHeight / 2) + 40, fontSize, GREEN);

  // font size 24
  DrawText("Press space for next enemy", (screenWidth / 2) - (MeasureText("Press space for next enemy", fontSize) / 2), (screenHeight / 2) + 80,
           fontSize, GREEN);
}

static void UpdatePlayerScore(void) {
  if (roundWon) {
    player->score += GetRandomValue(1, 20);
    roundWon = false;
  }
}

static void InitPlayer(void) {
  player = malloc(sizeof(Player));
  if (player == NULL) {
    printf("Error: failed to allocate memory for player");
    exit(1); // Added exit on allocation failure
  }
  player->base = (Rectangle){150, 200, 50, 75};
  player->color = GREEN;
  player->health = 100.0f;
  player->score = 0;
}

static void InitEnemy(void) {
  enemy = malloc(sizeof(Enemy)); // FIXED: Use Enemy instead of Player
  if (enemy == NULL) {
    printf("Error: failed to allocate memory for enemy");
    exit(1); // Added exit on allocation failure
  }
  enemy->base = (Rectangle){550, 200, 50, 75};
  enemy->color = RED;
  enemy->health = enemyMaxHealth;
  enemy->damage = 10.0f;
}

static void NextEnemy(void) {
  enemy->damage += GetRandomValue(2, 5);
  enemyMaxHealth += GetRandomValue(10, 20);
  enemy->health = enemyMaxHealth;
  int enemyColor = GetRandomValue(1, 9);

  // FIXED: Added break statements
  switch (enemyColor) {
  case 1:
    enemy->color = RED;
    break;
  case 2:
    enemy->color = BLACK;
    break;
  case 3:
    enemy->color = WHITE;
    break;
  case 4:
    enemy->color = GRAY;
    break;
  case 5:
    enemy->color = PINK;
    break;
  case 6:
    enemy->color = BEIGE;
    break;
  case 7:
    enemy->color = PURPLE;
    break;
  case 8:
    enemy->color = RAYWHITE;
    break;
  case 9:
    enemy->color = YELLOW;
    break;
  }
  roundWon = false;
  playerTurn = true; // Reset turn to player
}

static void ControlText(void) {
  int fontSize = 20;

  DrawText("1 to Attack", fontSize - padding, player->base.y - fontSize - padding,
           fontSize, RAYWHITE);

  // font size 20
  DrawText(TextFormat("Potions: %d", potions), fontSize - padding,
           player->base.y - (fontSize * 2) - padding, fontSize, RAYWHITE);

  // font size 20
  DrawText("2 to heal", fontSize - padding,
           player->base.y + ((float)fontSize / 2) - padding, fontSize,
           RAYWHITE);
}
