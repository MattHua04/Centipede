#ifndef SPIDER_H
#define SPIDER_H

#include <SFML/Graphics.hpp>
#include <random>
#include "mushroom.h"
#include "globals.h"

using namespace sf;

/**
 * @class Spider
 * @brief Represents a spider character in the game, inheriting from Sprite.
 * 
 * The Spider class handles the behavior and properties of a spider character,
 * including movement, collision detection, and rendering.
 */
class Spider : public Sprite {
    public:
        /**
         * @brief Constructs a Spider object with a specified speed.
         * @param speed The speed at which the spider moves.
         */
        Spider(int speed);

        /**
         * @brief Updates the spider's state.
         */
        void update();

        /**
         * @brief Resets the spider to its initial state.
         */
        void reset(bool resetSpeed=true);

        /**
         * @brief Changes the direction of the spider.
         */
        void changeDirection();

        /**
         * @brief Checks for collisions with mushrooms.
         */
        void checkMushroomCollision();

        /**
         * @brief Draws the spider on the screen.
         */
        void draw();

        /**
         * @brief Sets the status of the spider.
         * @param status The new status of the spider.
         */
        void setStatus(CharacterStatus status);

        /**
         * @brief Gets the current status of the spider.
         * @return The current status of the spider.
         */
        CharacterStatus getStatus() {return status;};

        /**
         * @brief Rotates the spider's texture.
         */
        void rotateTexture();

        /** 
         * @brief Handles collisions with other objects.
         */
        void handleCollision();
        
        /**
         * @brief Gets the next position of the spider.
         * @return The next position of the spider.
         */
        Vector2f getNextPosition();

        /**
         * @brief Gets the speed of the spider.
         * @return The speed of the spider.
         */
        int getSpeed() {return speed;};

        /**
         * @brief Sets the speed of the spider.
         * @param speed The new speed of the spider.
         */
        void setSpeed(int speed) {this->speed = speed;};

    private:
        int initialSpeed; ///< The initial speed of the spider.
        int speed; ///< The speed at which the spider moves.
        int dx, dy; ///< The direction of the spider's movement.
        CharacterStatus status; ///< The current status of the spider.
        int animationTick; ///< The current animation tick.
        int textureIndex; ///< The index of the current texture.
        int spawnDelay; ///< The delay before the spider spawns.
        Clock spawnClock; ///< The clock used to track spawn timing.
};

/**
 * @brief Initializes the spider character.
 */
void spiderInit(int initialSpeed);

/**
 * @brief Generates a random direction.
 * @return An integer representing the random direction.
 */
int getRandomDirection();

/**
 * @brief Determines if a random event occurs based on a given percentage.
 * @param percentage The chance percentage for the event to occur.
 * @return True if the event occurs, false otherwise.
 */
bool getRandomChance(int percentage);

/**
 * @brief Generates a random float within a specified range.
 * @param min The minimum value of the range.
 * @param max The maximum value of the range.
 * @return A random float between min and max.
 */
float getRandomFloat(float min, float max);

extern std::vector<Texture> spiderTextures;
extern Spider spider;

#endif