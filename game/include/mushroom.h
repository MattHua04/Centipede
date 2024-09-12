/*
Author: Matthew Hua
Class: ECE4122
Last Date Modified: 09-09-2024
Description:
Lab 1: Centipede Game
*/

#ifndef MUSHROOM_H
#define MUSHROOM_H

#include <SFML/Graphics.hpp>
#include <list>

using namespace sf;

/**
 * @class Mushroom
 * @brief Represents a mushroom in the game, inheriting from the Sprite class.
 * 
 * The Mushroom class provides functionality to handle collisions, manage health,
 * and compare mushrooms based on their unique identifiers.
 */
class Mushroom : public Sprite {
    public:
        /**
         * @brief Default constructor for the Mushroom class.
         * 
         * This constructor initializes a new instance of the Mushroom class.
         */
        Mushroom();
        
        /**
         * @brief Handles mushroom collision with laser blasts.
         */
        void handleCollision();

        /**
         * @brief Gets the health of the mushroom.
         * @return The health of the mushroom.
         */
        int getHealth() {return health;};

        /**
         * @brief Sets the health of the mushroom.
         * @param health The new health value.
         */
        void setHealth(int health) {this->health = health;};

        /**
         * @brief Checks if two given mushrooms are the same.
         * @return The ID of the other mushroom.
         */
        bool operator==(const Mushroom& other) const {return id == other.id;};
        
    private:
        int health; ///< The health of the mushroom.
        int id; ///< The ID of the mushroom.
};

/**
 * @brief Texture initialization for mushrooms.
 */
void mushroomInit();

/**
 * @brief Generates mushrooms at random positions.
 */
void generateMushrooms();

/**
 * @brief Adds a mushroom at a specified position.
 * @param x The x-coordinate of the position.
 * @param y The y-coordinate of the position.
 */
void addMushroom(int x, int y);

/**
 * @brief Draws all mushrooms on the screen.
 */
void drawMushrooms();

extern Texture normalMushroomTexture, damagedMushroomTexture;
extern std::list<Mushroom> mushrooms;

#endif