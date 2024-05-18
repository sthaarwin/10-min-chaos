#include <iostream>
#include <raylib.h>
#include <math.h>
#include <vector>
#include <chrono>
#include <algorithm> // std::remove_if

using namespace std;
using namespace std::chrono;

const int screenWidth = 960;
const int screenHeight = 540;

enum class GameState
{
    Start,
    Playing,
    GameOver,
    GameWin
};

class Player
{
public:
    float x, y;
    float width, height;
    float speed;
    int health;
    bool collided;
    time_point<steady_clock> lastCollisionTime;

    Player() : x(screenWidth / 2), y(screenHeight / 2), width(20), height(20), speed(7), health(3), collided(false) {}

    void draw()
    {
        DrawRectangle(x, y, width, height, PURPLE);
    }

    void update()
    {
        if (IsKeyDown(KEY_W))
            y -= speed;
        if (IsKeyDown(KEY_S))
            y += speed;
        if (IsKeyDown(KEY_A))
            x -= speed;
        if (IsKeyDown(KEY_D))
            x += speed;

        // Screen wrapping
        if (y < 10)
            y = 10;
        if (y > screenHeight - height - 10)
            y = screenHeight - height - 10;
        if (x < 10)
            x = 10;
        if (x > screenWidth - width - 10)
            x = screenWidth - width - 10;

        // Reset collision flag after cooldown
        auto currentTime = steady_clock::now();
        if (duration_cast<seconds>(currentTime - lastCollisionTime).count() >= 1)
        {
            collided = false;
        }
    }

    void decreaseHealth()
    {
        if (!collided)
        {
            health--;
            collided = true;
            lastCollisionTime = steady_clock::now();
        }
    }

    Rectangle getCollisionRec()
    {
        return {x, y, width, height};
    }
};

class Bot
{
public:
    float x, y;
    float width, height;
    float speed;
    bool active;

    Bot() : x(0), y(0), width(20), height(20), speed(1), active(true) {}

    void draw()
    {
        if (active)
            DrawRectangle(x, y, width, height, RED);
    }

    void update(Player &target, int frame, int minutesCounter)
    {
        if (!active)
            return;

        float dx = target.x - x;
        float dy = target.y - y;
        float length = sqrt(dx * dx + dy * dy);

        if (length != 0)
        {
            dx /= length;
            dy /= length;
        }

        x += dx * speed;
        y += dy * speed;

        float maxSpeed = 7.0f;
        float minSpeed = 1.0f;
        float speedIncrement = (maxSpeed - minSpeed) / (5 * 60 * 60); // 5 minutes in frames

        if (minutesCounter < 5)
        {
            speed = minSpeed + frame * speedIncrement;
        }
        else
        {
            speed = maxSpeed;
        }
    }

    Rectangle getCollisionRec()
    {
        return {x, y, width, height};
    }
};

class Gun
{
public:
    float xOffset, yOffset;
    float size;
    float angle;
    float orbitRadius;
    bool visible;

    Gun() : xOffset(0), yOffset(0), size(20), angle(0), orbitRadius(30), visible(true) {}

    void update(Player &target)
    {
        angle += 0.01f;

        // Calculate position relative to player for orbiting
        xOffset = orbitRadius * cos(angle);
        yOffset = orbitRadius * sin(angle);

        // Align center of orbit with center of player
        xOffset += target.width / 2;
        yOffset += target.height / 2;

        // Calculate angle to mouse cursor
        Vector2 mousePosition = GetMousePosition();
        angle = atan2(mousePosition.y - (target.y + yOffset), mousePosition.x - (target.x + xOffset));
    }

    void draw(Player &target)
    {
        Vector2 points[3];
        points[0] = {target.x + xOffset, target.y + yOffset};
        points[1] = {target.x + xOffset + cos(angle + 0.2f) * size, target.y + yOffset + sin(angle + 0.2f) * size};
        points[2] = {target.x + xOffset + cos(angle - 0.2f) * size, target.y + yOffset + sin(angle - 0.2f) * size};
        DrawTriangleLines(points[0], points[1], points[2], BLUE);
    }
};

class Bullet
{
public:
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;

    Bullet(Vector2 _position, Vector2 _velocity, float _radius) : position(_position), velocity(_velocity), radius(_radius), active(true) {}

    void update()
    {
        position.x += velocity.x;
        position.y += velocity.y;

        if (position.x < 0 || position.x > screenWidth || position.y < 0 || position.y > screenHeight)
        {
            active = false;
        }
    }

    void draw()
    {
        if (active)
        {
            DrawCircleV(position, radius, RED);
        }
    }
};

void drawStartScreen()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Welcome to 10 Min Chaos!", screenWidth / 2 - MeasureText("\nWelcome to 10 Min Chaos!", 40) / 2, screenHeight / 2 - 40, 40, BLACK);
    DrawText("Press SPACE to start", screenWidth / 2 - MeasureText("\nPress SPACE to start", 20) / 2, screenHeight / 2, 20, DARKGRAY);
    EndDrawing();
}

void drawPlayingScreen(Player &user, Gun &gun, vector<Bot> &enemies, vector<Bullet> &bullets, unsigned int framesCounter)
{
    int minutesCounter = (framesCounter / 60) / 60;
    int secondsCounter = (framesCounter / 60) % 60;

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Displaying the played time
    DrawText(TextFormat("%02d : %02d", minutesCounter, secondsCounter), screenWidth / 2 - MeasureText("00 : 00", 20) / 2, screenHeight / 4, 20, LIGHTGRAY);

    DrawText(("Health: " + to_string(user.health)).c_str(), 10, 10, 20, BLACK);
    user.update();
    user.draw();

    for (auto &enemy : enemies)
    {
        enemy.update(user, framesCounter, minutesCounter);
        enemy.draw();

        if (CheckCollisionRecs(user.getCollisionRec(), enemy.getCollisionRec()) && !user.collided)
        {
            user.decreaseHealth();
            enemy.active = false;

            if (user.health <= 0)
            {
                break;
            }
        }
    }

    gun.update(user);
    gun.draw(user);

    for (auto &bullet : bullets)
    {
        bullet.update();
        bullet.draw();

        for (auto &enemy : enemies)
        {
            if (CheckCollisionCircleRec(bullet.position, bullet.radius, enemy.getCollisionRec()))
            {
                bullet.active = false;
                enemy.active = false;
            }
        }
    }

    EndDrawing();
}

void spawnEnemies(vector<Bot> &enemies, unsigned int &spawnTimer, unsigned int spawnInterval)
{
    spawnTimer++;
    if (spawnTimer >= spawnInterval && enemies.size() < 20)
    {
        Bot newEnemy;

        // Randomize spawning position outside the screen
        int side = GetRandomValue(0, 3);
        switch (side)
        {
        case 0:
            newEnemy.x = -newEnemy.width;
            newEnemy.y = GetRandomValue(0, screenHeight - newEnemy.height);
            break;
        case 1:
            newEnemy.x = screenWidth;
            newEnemy.y = GetRandomValue(0, screenHeight - newEnemy.height);
            break;
        case 2:
            newEnemy.x = GetRandomValue(0, screenWidth - newEnemy.width);
            newEnemy.y = -newEnemy.height;
            break;
        case 3:
            newEnemy.x = GetRandomValue(0, screenWidth - newEnemy.width);
            newEnemy.y = screenHeight;
            break;
        }

        enemies.push_back(newEnemy);
        spawnTimer = 0;
    }
}

void handleShooting(Gun &gun, Player &user, vector<Bullet> &bullets, unsigned int &bulletCooldown, const unsigned int bulletCooldownDuration)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && bulletCooldown == 0)
    {
        Vector2 bulletPosition = {user.x + gun.xOffset, user.y + gun.yOffset};
        Vector2 bulletVelocity = {cos(gun.angle) * 15, sin(gun.angle) * 15};
        Bullet newBullet(bulletPosition, bulletVelocity, 3);
        bullets.push_back(newBullet);
        bulletCooldown = bulletCooldownDuration;
    }

    if (bulletCooldown > 0)
    {
        bulletCooldown--;
    }
}

void drawGameOverScreen()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Game Over!", screenWidth / 2 - MeasureText("Game Over!", 40) / 2, screenHeight / 2 - 40, 40, RED);
    DrawText("Press ESC to quit\n\nPress R to replay", screenWidth / 2 - MeasureText("Press ESC to quit\n\nPress R to replay", 20) / 2, screenHeight / 2 + 20, 20, BLACK);
    EndDrawing();
}

void drawGameWinScreen()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("You Win!", screenWidth / 2 - MeasureText("You Win!", 40) / 2, screenHeight / 2 - 40, 40, RED);
    DrawText("Press ESC to quit\n\nPress R to replay", screenWidth / 2 - MeasureText("Press ESC to quit\n\nPress R to replay", 20) / 2, screenHeight / 2 + 20, 20, BLACK);
    EndDrawing();
}

int main()
{
    InitWindow(screenWidth, screenHeight, "10 Min Chaos");
    SetTargetFPS(60);

    GameState gameState = GameState::Start;

    Gun gun;
    Player user;
    vector<Bot> enemies;
    vector<Bullet> bullets;
    unsigned int framesCounter = 0;
    unsigned int spawnTimer = 0;
    unsigned int spawnInterval = 60;
    unsigned int bulletCooldown = 0;
    const unsigned int bulletCooldownDuration = 60;

    while (!WindowShouldClose())
    {
        switch (gameState)
        {
        case GameState::Start:
        {
            drawStartScreen();
            if (IsKeyPressed(KEY_SPACE))
            {
                user = Player();
                gun = Gun();
                enemies.clear();
                bullets.clear();
                framesCounter = 0;
                spawnTimer = 0;
                bulletCooldown = 0;
                gameState = GameState::Playing;
            }
            break;
        }
        case GameState::Playing:
        {
            framesCounter++;
            drawPlayingScreen(user, gun, enemies, bullets, framesCounter);

            handleShooting(gun, user, bullets, bulletCooldown, bulletCooldownDuration);
            spawnEnemies(enemies, spawnTimer, spawnInterval);

            int minutesCounter = (framesCounter / 60) / 60;
            if (minutesCounter >= 10)
            {
                gameState = GameState::GameWin;
            }

            if (user.health <= 0)
            {
                gameState = GameState::GameOver;
            }

            enemies.erase(remove_if(enemies.begin(), enemies.end(), [](Bot &b)
                                    { return !b.active; }),
                          enemies.end());
            bullets.erase(remove_if(bullets.begin(), bullets.end(), [](Bullet &b)
                                    { return !b.active; }),
                          bullets.end());
            break;
        }
        case GameState::GameOver:
        {
            drawGameOverScreen();
            if (IsKeyPressed(KEY_R))
            {
                gameState = GameState::Start;
            }
            if (IsKeyPressed(KEY_ESCAPE))
            {
                CloseWindow();
            }
            break;
        }
        case GameState::GameWin:
        {
            drawGameWinScreen();
            if (IsKeyPressed(KEY_R))
            {
                gameState = GameState::Start;
            }
            if (IsKeyPressed(KEY_ESCAPE))
            {
                CloseWindow();
            }
            break;

        default:
            break;
        }
        }
    }

    CloseWindow();
    return 0;
}
