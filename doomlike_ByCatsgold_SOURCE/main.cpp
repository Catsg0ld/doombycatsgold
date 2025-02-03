#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include "include/json.hpp"

using json = nlohmann::json;

const int screenWidth = 800;
const int screenHeight = 600;
const int textureSize = 128;
const float collisionRadius = 0.1f;
const float M_PI = 3.14159f;

std::vector<std::vector<int>> map;
int mapWidth;
int mapHeight;

float playerX = 4.0f;
float playerY = 4.0f;
float playerAngle = 0.0f;
sf::Texture wallTexture;

bool loadMapFromJSON(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "������ �������� �����: " << filename << std::endl;
        return false;
    }

    json data;
    file >> data;

    mapWidth = data["width"];
    mapHeight = data["height"];
    map = data["map"].get<std::vector<std::vector<int>>>();

    return true;
}

bool checkCollision(float x, float y) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int checkX = static_cast<int>(x + i * collisionRadius);
            int checkY = static_cast<int>(y + j * collisionRadius);
            if (checkX < 0 || checkX >= mapWidth || checkY < 0 || checkY >= mapHeight) continue;
            if (map[checkY][checkX] == 1) return true;
        }
    }
    return false;
}

void movePlayer(float& x, float& y, float dx, float dy) {
    const float stepSize = 0.05f;
    float moveDist = sqrt(dx * dx + dy * dy);
    float steps = ceil(moveDist / stepSize);
    dx /= steps;
    dy /= steps;

    for (int i = 0; i < steps; i++) {
        float newX = x + dx;
        float newY = y + dy;

        if (!checkCollision(newX, newY)) {
            x = newX;
            y = newY;
        }
        else {
            if (!checkCollision(newX, y)) x = newX;
            if (!checkCollision(x, newY)) y = newY;
        }
    }
}

bool isWall(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return true;
    return map[y][x] == 1;
}

int main() {
    if (!loadMapFromJSON("map.json")) return -1;

    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "CATSGOLD DOOM");
    if (!wallTexture.loadFromFile("wall.png")) return -1;

    wallTexture.setRepeated(true);
    window.setFramerateLimit(60);

    sf::View view(sf::FloatRect(0, 0, screenWidth, screenHeight));
    window.setView(view);

    sf::RectangleShape sky(sf::Vector2f(screenWidth, screenHeight / 2));
    sky.setFillColor(sf::Color(100, 200, 255));

    sf::RectangleShape floor(sf::Vector2f(screenWidth, screenHeight / 2));
    floor.setPosition(0, screenHeight / 2);
    floor.setFillColor(sf::Color(50, 150, 50));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::Resized) {
                float aspectRatio = static_cast<float>(event.size.width) / event.size.height;
                view.setSize(screenWidth * aspectRatio, screenHeight);
                window.setView(view);
            }
        }

        float moveSpeed = 0.05f;
        float rotSpeed = 0.05f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            float dx = cos(playerAngle) * moveSpeed;
            float dy = sin(playerAngle) * moveSpeed;
            movePlayer(playerX, playerY, dx, dy);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            float dx = -cos(playerAngle) * moveSpeed;
            float dy = -sin(playerAngle) * moveSpeed;
            movePlayer(playerX, playerY, dx, dy);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) playerAngle -= rotSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) playerAngle += rotSpeed;

        window.clear();
        window.draw(sky);
        window.draw(floor);

        for (int x = 0; x < screenWidth; x++) {
            float rayAngle = (playerAngle - M_PI / 6) + (x * M_PI / 3 / screenWidth);
            float dirX = cos(rayAngle);
            float dirY = sin(rayAngle);
            float distanceToWall = 0.0f;
            bool hitWall = false;

            while (!hitWall && distanceToWall < 20.0f) {
                distanceToWall += 0.01f;
                int testX = static_cast<int>(playerX + dirX * distanceToWall);
                int testY = static_cast<int>(playerY + dirY * distanceToWall);

                if (isWall(testX, testY)) {
                    hitWall = true;
                    int wallHeight = static_cast<int>(screenHeight / (distanceToWall * cos(rayAngle - playerAngle)));

                    float hitX = playerX + dirX * distanceToWall;
                    float hitY = playerY + dirY * distanceToWall;
                    float texX = hitX + hitY;
                    texX -= std::floor(texX);

                    sf::Sprite wall(wallTexture);
                    wall.setTextureRect(sf::IntRect(static_cast<int>(texX * textureSize), 0, 1, textureSize));
                    wall.setScale(1, wallHeight / static_cast<float>(textureSize));
                    wall.setPosition(x, screenHeight / 2 - wallHeight / 2);
                    window.draw(wall);
                }
            }
        }

        window.display();
    }
    return 0;
}