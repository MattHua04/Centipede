#include "mushroom.h"
#include <random>
#include <list>
#include "globals.h"

Texture normalMushroomTexture, damagedMushroomTexture;
std::list<Mushroom> mushrooms;

void mushroomInit() {
    // Load textures
    if (!normalMushroomTexture.loadFromFile("assets/textures/Mushroom0.png")) {
        printf("Failed to load texture from %s\n", "assets/textures/Mushroom0.png");
    }
    if (!damagedMushroomTexture.loadFromFile("assets/textures/Mushroom1.png")) {
        printf("Failed to load texture from %s\n", "assets/textures/Mushroom1.png");
    }
}

Mushroom::Mushroom() {
    health = 2;
    setTexture(normalMushroomTexture);
    id = globalCounter++;
}

void Mushroom::handleCollision() {
    health--;
    if (health == 1) {
        setTexture(damagedMushroomTexture);
    } else if (health == 0) {
        mushrooms.remove(*this);
    }
}

void generateMushrooms() {
    mushrooms.clear();
    int spriteWidth = normalMushroomTexture.getSize().x;
    int spriteHeight = normalMushroomTexture.getSize().y;
    std::random_device rd;
    std::mt19937 gen(rd());

    // Generate a list of possible positions allowing for a 1 sprite top, left, and right border and 3 sprite bottom border
    std::vector<std::pair<int, int>> possiblePositions;
    for (int x = spriteWidth; x < (windowWidth / spriteWidth) * spriteWidth - spriteWidth; x += spriteWidth) {
        for (int y = spriteHeight; y < (windowHeight / spriteHeight) * spriteHeight - spriteHeight * 3; y += spriteHeight) {
            possiblePositions.emplace_back(x, y);
        }
    }
    // Shuffle the possible positions
    std::shuffle(possiblePositions.begin(), possiblePositions.end(), gen);

    // Place 30 mushrooms
    int count = 0;
    for (auto& pos : possiblePositions) {
        if (count >= 30) break;
        int x = pos.first;
        int y = pos.second;
        Mushroom mushroom;
        mushroom.setPosition(x, y);
        mushrooms.push_back(mushroom);
        count++;
    }
}

void addMushroom(int x, int y) {
    Mushroom mushroom;
    mushroom.setPosition(x, y);
    mushrooms.push_back(mushroom);
}

void drawMushrooms() {
    for (auto& mushroom : mushrooms) {
        if (mushroom.getHealth() > 0) {
            window.draw(mushroom);
        }
    }
}