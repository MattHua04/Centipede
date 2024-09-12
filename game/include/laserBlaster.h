/*
Author: Matthew Hua
Class: ECE4122
Last Date Modified: 09-09-2024
Description:
Lab 1: Centipede Game
*/

#ifndef LASERBLASTER_H
#define LASERBLASTER_H

#include <SFML/Graphics.hpp>
#include "globals.h"
#include "centipede.h"
#include "spider.h"
#include "mushroom.h"
#include <list>

using namespace sf;

/**
 * @class ECE_LaserBlast
 * @brief Represents a laser blast in the game.
 * 
 * This class inherits from the Sprite class and provides functionality
 * for a laser blast, including movement and collision handling.
 */
class ECE_LaserBlast : public Sprite {
    public:
        /**
         * @brief Constructs an ECE_LaserBlast object with a specified blast speed.
         * 
         * @param blastSpeed The speed at which the laser blast travels.
         */
        ECE_LaserBlast(float blastSpeed);

        /**
         * @brief Moves the laser blast.
         * @return True if the laser blast is still on the screen, false otherwise.
         */
        bool move();

        /**
         * @brief Handles collision detection for the laser blast.
         * @return True if a collision occurred and laser blast should be removed, false otherwise.
         */
        bool handleCollision();

        /**
         * @brief Check if two given laser blasts are the same.
         * @return The ID of the other laser blast.
         */
        bool operator==(const ECE_LaserBlast& other) const {return id == other.id;};
    private:
        float speed; ///< The speed of the laser blast.
        int id; ///< The ID of the laser blast.
};

/**
 * @class ECE_LaserBlaster
 * @brief Represents a laser blaster in the game, inheriting from the Sprite class.
 * 
 * The ECE_LaserBlaster class provides functionalities for updating the blaster's state,
 * shooting laser blasts, managing scores and lives, and drawing the blaster on the screen.
 */
class ECE_LaserBlaster : public Sprite {
    public:
        /**
         * @brief Constructs a new ECE_LaserBlaster object with a specified speed, blast speed, and reload time.
         * 
         * @param speed The speed of the laser blaster.
         * @param blastSpeed The speed of the laser blasts.
         * @param reloadTime The time between shots.
         */
        ECE_LaserBlaster(float speed, float blastSpeed, float reloadTime);

        /**
         * @brief Updates the state of the laser blaster based on the given direction.
         * 
         * @param direction The direction to move the laser blaster.
         */
        void update(Direction direction);

        /**
         * @brief Shoots a laser blast from the position of the player.
         */
        void shoot();

        /**
         * @brief Returns the list of laser blasts.
         * 
         * @return std::list<ECE_LaserBlast> The list of laser blasts.
         */
        std::list<ECE_LaserBlast>& getBlasts() {return blasts;};

        /**
         * @brief Resets the score, lives, and laser blaster to their initial states.
         */
        void resetScore() {score = 0;};

        /**
         * @brief Increments the score by a given value.
         * 
         * @param i The value to increment the score by.
         */
        void incrementScore(int i) {score += i;};

        /**
         * @brief Returns the current score.
         * 
         * @return int The current score.
         */
        int getScore() {return score;};

        /**
         * @brief Updates the high score if the current score is higher.
         * 
         * @param highScore The new high score to compare.
         */
        void updateHighScore() {highScore = std::max(score, highScore);};

        /**
         * @brief Returns the high score.
         * 
         * @return int The high score.
         */
        int getHighScore() {return highScore;};

        /**
         * @brief Returns the number of lives remaining.
         * 
         * @return int The number of lives.
         */
        int getLives() {return lives;};

        /**
         * @brief Resets the number of lives to 3.
         */
        void resetLives() {lives = 3;};

        /**
         * @brief Decrements the number of lives by 1.
         */
        void decrementLives() {lives--;};

        /**
         * @brief Resets the player to its initial state.
         */
        void reset();

        /**
         * @brief Draws the player on the screen.
         */
        void draw();

        /**
         * @brief Resets the position of the player to the bottom center of the screen.
         */
        void resetPosition();

    private:
        std::list<ECE_LaserBlast> blasts; ///< The list of laser blasts.
        float speed, blastSpeed, shotDelay; ///< The speed of the laser blaster, the speed of the laser blasts, and the time between shots.
        int lives, score, highScore; ///< The number of lives, the current score, and the high score.
        Clock shotClock; ///< The clock to keep track of the time between shots.
};

/**
 * @brief Initializes the laser blaster textures and player entity.
 */
void laserBlasterInit();

extern Texture laserTexture, starShipTexture;
extern ECE_LaserBlaster player;

#endif