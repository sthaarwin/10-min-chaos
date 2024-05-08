#include <iostream>
#include <raylib.h>
#include <math.h>
#include <vector>

using namespace std;

const int screenWidth = 960;
const int screenHeight = 540;

class player {
public:
    float x, y;
    float width, height;
    float speed;

    void draw() {
        DrawRectangle(x, y, width, height, PURPLE);
    }

    void update() {
        if (IsKeyDown(KEY_W)) {
            y -= speed;
        }
        else if (IsKeyDown(KEY_S)) {
            y += speed;
        }
        else if (IsKeyDown(KEY_A)) {
            x -= speed;
        }
        else if (IsKeyDown(KEY_D)) {
            x += speed;
        }

        // Screen wrapping
        if (y <= 10) {
            y = 10;
        }
        else if (y >= screenHeight - height - 10) {
            y = screenHeight - height   - 10;
        }
        if (x <= 10) {
            x = 10;
        }
        else if (x >= screenWidth - width - 10) {
            x = screenWidth - width - 10;
        }
    }
};

class bot : public player {
public:
    bool active = true;

    bot() : active(true) {}

    void draw() {
        if (active) // Draw only if active
            DrawRectangle(x, y, width, height, RED);
    }

    void update(player& target, int frame, int minutesCounter) {
        if (!active)
            return; // Don't update if not active

        float dx = target.x - x;
        float dy = target.y - y;

        float length = sqrt(dx * dx + dy * dy);
        if (length != 0) {
            dx /= length;
            dy /= length;
        }

        x += dx * speed;
        y += dy * speed;

        float maxSpeed = 7.0f;
        float minSpeed = 1.0f;
        float speedIncrement = (maxSpeed - minSpeed) / (5 * 60);

        if (minutesCounter < 7) {
            speed = minSpeed + frame * speedIncrement * 0.005;
        }
        else {
            speed = maxSpeed;
        }
    }
};

int main() {
    InitWindow(screenWidth, screenHeight, "10 MIN CHAOS");
    SetTargetFPS(60);

    player user;
    user.x = screenWidth / 2;
    user.y = screenHeight / 2;
    user.width = 20;
    user.height = 20;
    user.speed = 7;

    vector<bot> enemies;
    unsigned int framesCounter = 0;
    unsigned int spawnTimer = 0;
    unsigned int spawnInterval = 300; // Spawn a new enemy every 300 frames (5 seconds)

    // Spawn initial enemies
    for (int i = 0; i < 10; ++i) {
        bot newEnemy;
        newEnemy.x = GetRandomValue(0, screenWidth - 20);
        newEnemy.y = GetRandomValue(0, screenHeight - 20);
        newEnemy.width = 20;
        newEnemy.height = 20;
        newEnemy.speed = 1;
        enemies.push_back(newEnemy);
    }

    while (!WindowShouldClose()) {
        framesCounter++;
        int secondsCounter = (framesCounter / 60) % 60;
        int minutesCounter = (framesCounter / 60) / 60;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        user.update();
        user.draw();

        // Update and draw bots
        for (size_t i = 0; i < enemies.size(); ++i) {
            enemies[i].update(user, framesCounter, minutesCounter);
            enemies[i].draw();

            // Collision detection with player
            if (CheckCollisionRecs(
                { user.x, user.y, user.width, user.height },
                { enemies[i].x, enemies[i].y, enemies[i].width, enemies[i].height })) {
                // Handle collision with player
                cout << "Collision with player" << endl;
                enemies[i].active = false; // Deactivate the bot
            }
            
            // Collision detection with other bots
            for (size_t j = i + 1; j < enemies.size(); ++j) {
                if (CheckCollisionRecs(
                    { enemies[i].x, enemies[i].y, enemies[i].width, enemies[i].height },
                    { enemies[j].x, enemies[j].y, enemies[j].width, enemies[j].height })) {
                    // Respawn collided bots
                    enemies[i].x = GetRandomValue(0, screenWidth - 20);
                    enemies[i].y = GetRandomValue(0, screenHeight - 20);
                    enemies[j].x = GetRandomValue(0, screenWidth - 20);
                    enemies[j].y = GetRandomValue(0, screenHeight - 20);
                }
            }
        }

        // Check if it's time to spawn a new enemy
        spawnTimer++;
        if (spawnTimer >= spawnInterval && enemies.size() < 20) {
            bot newEnemy;
            newEnemy.x = GetRandomValue(0, screenWidth - 20);
            newEnemy.y = GetRandomValue(0, screenHeight - 20);
            newEnemy.width = 20;
            newEnemy.height = 20;
            newEnemy.speed = 1;
            enemies.push_back(newEnemy);
            spawnTimer = 0; // Reset spawn timer
        }

        DrawText((to_string(minutesCounter) + " : " + 
        to_string(secondsCounter)).c_str(), screenWidth / 2, screenHeight / 4, 25, GRAY);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
