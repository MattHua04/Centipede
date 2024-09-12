/*
Author: Matthew Hua
Class: ECE4122
Last Date Modified: 09-09-2024
Description:
Lab 1: Centipede Game
*/

#include "spider.h"

std::vector<Texture> spiderTextures;
Spider spider(0);

void spiderInit(int initialSpeed) {
    // Load the spider textures
    std::vector<std::string> spiderFileNames = {"assets/textures/Spider0.png", "assets/textures/Spider1.png"};
    for (const auto& fileName : spiderFileNames) {
        Texture texture;
        if (!texture.loadFromFile(fileName)) {
            printf("Failed to load texture from %s\n", fileName.c_str());
        }
        spiderTextures.push_back(texture);
    }

    // Initialize the spider
    spider = Spider(initialSpeed);
    spider.setTexture(spiderTextures[0]);
}

Spider::Spider(int initialSpeed) {
    animationTick = 0;
    speed = initialSpeed;
    this->initialSpeed = initialSpeed;
    textureIndex = 0;
    spawnClock.restart();
    spawnDelay = 3;
    status = CharacterStatus::ALIVE;
    reset();
}

void Spider::rotateTexture() {
    if (animationTick % 10 == 0) {
        textureIndex = (textureIndex + 1) % spiderTextures.size();
        setTexture(spiderTextures[textureIndex]);
    }
    animationTick++;
}

void Spider::update() {
    // If the spider is dead, reset it after the spawn delay
    if (status == CharacterStatus::DEAD && spawnClock.getElapsedTime().asSeconds() > spawnDelay) {
        spider.reset(false);
    } else if (status == CharacterStatus::DEAD) {
        return;
    }

    // Normalize the direction when moving diagonally
    float magnitude = std::sqrt(dx * dx + dy * dy);
    if (magnitude > 0) {
        // Scale dx and dy to ensure the spider's speed is consistent
        float scale = speed / magnitude;
        dx *= scale;
        dy *= scale;
    }

    // Update the spider's position
    setPosition(getPosition().x + dx, getPosition().y + dy);

    // Check if the spider will move off the screen and reverse direction if necessary
    Vector2f nextPosition = getNextPosition();
    // Check horizontal bounds
    if (nextPosition.x < 0 || nextPosition.x + getGlobalBounds().width > windowWidth) {
        dx = -dx;
    }
    // Check vertical bounds
    if (nextPosition.y < 0.5f * windowHeight || nextPosition.y + getGlobalBounds().height > windowHeight) {
        dy = -dy;
    }
    
    // Randomly change direction occasionally
    if (getRandomChance(2)) {
        changeDirection();
    }

    // Check for collisions with mushrooms
    checkMushroomCollision();
}

Vector2f Spider::getNextPosition() {
    return Vector2f(getPosition().x + dx, getPosition().y + dy);
}

void Spider::handleCollision() {
    status = CharacterStatus::DEAD;
    spawnClock.restart();
}

void Spider::reset(bool resetSpeed) {
    status = CharacterStatus::ALIVE;

    // Randomly choose either the left or right side of the screen for the X position
    float newX = getRandomChance(50) ? 0.0f : windowWidth - getGlobalBounds().width;
    // Y position is randomly selected in the bottom portion of the screen
    float newY = getRandomFloat(windowHeight * 0.5f, windowHeight - getGlobalBounds().height);
    // Set the new position
    setPosition(newX, newY);

    // Determine the initial direction: move towards the center of the screen
    dx = (newX == 0.0f) ? 1 : -1;

    // Set a random vertical direction (-1, 0, or 1)
    dy = getRandomDirection();
    
    // Ensure the spider always moves
    if (dy == 0 && dx == 0) {
        dx = (newX == 0.0f) ? 1 : -1;
        dy = dx;
    }

    // Reset the speed if specified
    if (resetSpeed) {
        speed = initialSpeed;
    }
}

void Spider::changeDirection() {
    // Randomly set the horizontal and vertical direction (-1, 0, or 1)
    dx = getRandomDirection();
    dy = getRandomDirection();

    // Ensure the spider always moves
    if (dx == 0 && dy == 0) {
        dx = 1;
        dy = 1;
    }
}

void Spider::checkMushroomCollision() {
    for (Mushroom& mushroom : mushrooms) {
        if (getGlobalBounds().intersects(mushroom.getGlobalBounds())) {
            mushroom.setHealth(0);
        }
    }
}

void Spider::draw() {
    if (status == CharacterStatus::DEAD) {
        return;
    }
    rotateTexture();
    window.draw(*this);
}

int getRandomDirection() {
    // Generate a random int between -1 and 1
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(-1, 1);
    return dist(gen);
}

bool getRandomChance(int percentage) {
    // Generate a random int between 1 and 100
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 100);
    return dist(gen) <= percentage;
}

float getRandomFloat(float min, float max) {
    // Generate a random float between min and max
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}
