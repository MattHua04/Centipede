#ifndef CENTIPEDE_H
#define CENTIPEDE_H

#include <SFML/Graphics.hpp>
#include <list>
#include <tuple>
#include <queue>
#include "mushroom.h"
#include "globals.h"
#include "laserBlaster.h"

using namespace sf;

/**
 * @class ECE_CentipedeSegment
 * @brief Represents a segment of a centipede in the game.
 * 
 * This class is responsible for handling the behavior and properties of a centipede segment,
 * including movement, collision detection, and texture orientation.
 */
class ECE_CentipedeSegment : public Sprite {
    public:
        /**
         * @brief Constructs a new ECE_CentipedeSegment object.
         * 
         * @param isHead A boolean indicating whether this segment is the head of the centipede.
         * @param initialSpeed The initial speed of the centipede.
         */
        ECE_CentipedeSegment(bool isHead, int initialSpeed);

        /**
         * @brief Moves the head of the centipede.
         */
        void headMove();

        /**
         * @brief Checks for collisions with the centipede and determine the next move.
         */
        void checkCollisions();


        /**
         * @brief Checks if a head segment can move to a given bounds without collisions
         *        excluding its trailing bodies.
         * 
         * @param bounds The area of interest.
         * @return true if moving to bounds will not result in a collision, false otherwise.
         */
        bool segmentCanMove(FloatRect bounds);

        /**
         * @brief Returns the trailing bodies of the centipede segment.
         * 
         * @param segment The segment to find the trailing bodies of.
         * @return std::list<ECE_CentipedeSegment> The trailing bodies of the segment.
         */
        std::list<ECE_CentipedeSegment> getTrailingBodies(ECE_CentipedeSegment& segment);

        /**
         * @brief Moves the body of the centipede.
         * 
         * @param dx The horizontal direction to move.
         * @param dy The vertical direction to move.
         * @param x The x-coordinate of the destination.
         * @param y The y-coordinate of the destination.
         */
        void bodyMove(int dx, int dy, float x, float y);

        /**
         * @brief Returns the type of the segment.
         * 
         * @return SegmentType The type of the segment.
         */
        SegmentType getType() {return type;};

        /**
         * @brief Sets the type of the segment.
         * 
         * @param type The new type of the segment.
         */
        void setType(SegmentType type) {this->type = type;};

        /**
         * @brief Sets the direction of the segment.
         * 
         * @param dx The horizontal direction.
         * @param dy The vertical direction.
         */
        void setDirection(int dx, int dy) {this->dx = dx; this->dy = dy;};

        /**
         * @brief Returns the direction of the segment.
         * 
         * @return std::tuple<int, int> The horizontal and vertical directions.
         */
        std::tuple<int, int> getDirection() {return std::make_tuple(dx, dy);};

        /**
         * @brief Returns the next segment's bounds based on the current direction.
         * 
         * @param useDx The horizontal direction to use.
         * @param useDy The vertical direction to use.
         * @return FloatRect The bounds of the next segment.
         */
        FloatRect getNextSegmentBounds(int useDx=999, int useDy=999);

        /**
         * @brief Returns the ID of the segment.
         * 
         * @return int The ID of the segment.
         */
        int getId() {return id;};

        /**
         * @brief To check if two given segments are the same.
         * 
         * @param id The new ID of the other segment.
         */
        bool operator==(const ECE_CentipedeSegment& other) const {return id == other.id;};

        /**
         * @brief Updates the texture orientation based on the current direction.
         */
        void updateTextureOrientation();

        /**
         * @brief Sets the status of the segment.
         * 
         * @param status The new status of the segment.
         */
        void setStatus(CharacterStatus status) {this->status = status;};

        /**
         * @brief Returns the status of the segment.
         * 
         * @return CharacterStatus The status of the segment.
         */
        CharacterStatus getStatus() {return status;};

        /**
         * @brief Switches texture to next animation.
         */
        void animate();

        /**
         * @brief Returns the animation texture index of the segment.
         * 
         * @return int The texture index of the segment.
         */
        int getTextureIndex() {return textureIndex;};

        /**
         * @brief Finds and returns a list of all open spots.
         * 
         * @return std::vector<sf::Vector2f> List of all open spots.
         */
        std::vector<sf::Vector2f> findOpenSpots();

        /**
         * @brief Finds the closest open spot to the segment.
         * 
         * @return sf::Vector2f The closest open spot.
         */
        sf::Vector2f findClosestOpenSpot();

        /**
         * @brief Sets the speed of the segment.
         * 
         * @param speed The new speed of the segment.
         */
        void setSpeed(int speed) {this->speed = speed;};

        /**
         * @brief Returns the speed of the segment.
         * 
         * @return float The speed of the segment.
         */
        int getSpeed() {return speed;};

        /**
         * @brief Sets the maximum delay ticks for the segment.
         * 
         * @param maxDelayTicks The new maximum delay ticks.
         */
        void setMaxDelayTicks(int maxDelayTicks) {this->maxDelayTicks = maxDelayTicks;};

        /**
         * @brief Returns the maximum delay ticks of the segment.
         * 
         * @return int The maximum delay ticks of the segment.
         */
        int getMaxDelayTicks() {return maxDelayTicks;};

        /**
         * @brief Returns the moves queue of the segment.
         * 
         * @return std::queue<std::tuple<float, float, int, int>> The moves queue.
         */
        std::queue<std::tuple<float, float, int, int>>& getMoves() {return moves;};

        /**
         * @brief Sets the saved vertical direction of the segment.
         * 
         * @param savedDy The new saved vertical direction.
         */
        void setSavedDy(int savedDy) {this->savedDy = savedDy;};

        /**
         * @brief Sets the vertical direction during random walk.
         * 
         * @param randomWalkDy The new random walk veritcal direction.
         */
        void setRandomWalkDy(int randomWalkDy) {this->randomWalkDy = randomWalkDy;};

        /**
         * @brief Returns the vertical direction during random walk.
         * 
         * @return int The vertical direction for random walk.
         */
        int getRandomWalkDy() {return randomWalkDy;};

    private:
        SegmentType type; ///< The type of the segment.
        int dx, dy; ///< The horizontal and vertical directions.
        int savedDy; ///< The saved vertical direction for when a head gets stuck.
        int speed; ///< The speed of the segment.
        int delayTicks, maxDelayTicks; ///< The delay ticks for the segment.
        std::queue<std::tuple<float, float, int, int>> moves; ///< The moves queue for the segment.
        int id; ///< The ID of the segment.
        CharacterStatus status; ///< The status of the segment.
        int textureIndex; ///< The texture index of the segment.
        int animationTick; ///< The animation tick of the segment.
        int randomWalkDy; ///< The vertical direction for random walk.
};

/**
 * @class ECE_Centipede
 * @brief Represents a centipede in the game, composed of multiple segments.
 * 
 * The ECE_Centipede class inherits from the Sprite class and manages a centipede
 * entity in the game. It provides functionalities to move, reset, and draw the centipede,
 * as well as access its segments.
 */
class ECE_Centipede : public Sprite {
    public:
        /**
         * @brief Constructs a new ECE_Centipede object with a specified number of segments.
         * 
         * @param segmentCount The number of segments in the centipede.
         * @param initialSpeed The initial speed of the centipede.
         */
        ECE_Centipede(int segmentCount, int initialSpeed);

        /**
         * @brief Moves the centipede.
         */
        void move();

        /**
         * @brief Resets the centipede to its initial state.
         */
        void reset(bool resetSpeed=true);

        /**
         * @brief Draws the centipede on the window.
         */
        void draw();

        /**
         * @brief Returns the list of segments in the centipede.
         * 
         * @return std::list<ECE_CentipedeSegment> The list of segments.
         */
        std::list<ECE_CentipedeSegment>& getSegments() {return segments;};
        
        /**
         * @brief Sets the speed of the centipede.
         * 
         * @param speed The new speed of the centipede.
         */
        void setSpeed(int speed);

        /**
         * @brief Returns the speed of the centipede.
         * 
         * @return int The speed of the centipede.
         */
        int getSpeed() {return speed;};

        /**
         * @brief Sets the random walk behavior of the centipede.
         * 
         * @param randomWalk A boolean indicating whether the centipede should randomly walk.
         */
        void setRandomWalk(bool randomWalk);

        /**
         * @brief Returns the random walk behavior of the centipede.
         * 
         * @return bool A boolean indicating whether the centipede should randomly walk.
         */
        bool getRandomWalk() {return randomWalk;};

    private:
        std::list<ECE_CentipedeSegment> segments; ///< The list of segments in the centipede.
        int length; ///< The number of segments in the centipede.
        int initialSpeed; ///< The initial speed of the centipede.
        int speed; ///< The speed of the centipede.
        bool randomWalk; ///< A boolean indicating whether the centipede should randomly walk.
};


/**
 * @brief Determines the sign of a given floating-point value.
 *
 * @param value The floating-point value whose sign is to be determined.
 * @return An integer representing the sign of the input value.
 */
int getSign(float value);

/**
 * @brief Initializes the textures and centipede entity.
 * 
 * @param length The number of segments in the centipede.
 * @param initialSpeed The initial speed of the centipede.
 */
void centipedeInit(int length, int initialSpeed);

extern std::vector<Texture> headTextures, bodyTextures;
extern ECE_Centipede centipede;

#endif