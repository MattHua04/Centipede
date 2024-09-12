/*
Author: Matthew Hua
Class: ECE4122
Last Date Modified: 09-09-2024
Description:
Lab 1: Centipede Game
*/

#include "centipede.h"

std::vector<Texture> headTextures, bodyTextures;
ECE_Centipede centipede(0, 0);

int getSign(float value) {
    if (value > 0) {
        return 1;
    } else if (value < 0) {
        return -1;
    } else {
        return 0;
    }
}

void centipedeInit(int length, int initialSpeed) {
    // Load head textures
    std::vector<std::string> headFileNames = {"assets/CentipedeHead0.png", "assets/CentipedeHead1.png", "assets/CentipedeHead2.png"};
    for (const auto& fileName : headFileNames) {
        Texture texture;
        if (!texture.loadFromFile(fileName)) {
            printf("Failed to load texture from %s\n", fileName.c_str());
        }
        headTextures.push_back(texture);
    }

    // Load body textures
    std::vector<std::string> bodyFileNames = {"assets/CentipedeBody0.png", "assets/CentipedeBody1.png", "assets/CentipedeBody2.png"};
    for (const auto& fileName : bodyFileNames) {
        Texture texture;
        if (!texture.loadFromFile(fileName)) {
            printf("Failed to load texture from %s\n", fileName.c_str());
        }
        bodyTextures.push_back(texture);
    }

    // Initialize the centipede
    centipede = ECE_Centipede(length, initialSpeed);
}

ECE_CentipedeSegment::ECE_CentipedeSegment(bool isHead, int initialSpeed) {
    dx = 1;
    dy = 1;
    savedDy = 0;
    randomWalkDy = 0;
    speed = initialSpeed;
    delayTicks = 0;
    this->id = globalCounter++;
    animationTick = 0;
    textureIndex = 0;
    setType((isHead) ? SegmentType::HEAD : SegmentType::BODY);
    setTexture((isHead) ? headTextures[textureIndex] : bodyTextures[textureIndex]);
    setStatus(CharacterStatus::ALIVE);
    maxDelayTicks = getGlobalBounds().width / speed;
    // Fill the moves queue with default moves to match the delay ticks
    for (int i = 0; i < maxDelayTicks; i++) {
        moves.push(std::make_tuple(windowWidth / 2.0f, 0.0f, dx, dy));
    }
}

bool inSegmentList(std::list<ECE_CentipedeSegment> list, ECE_CentipedeSegment segment) {
    return std::find(list.begin(), list.end(), segment) != list.end();
}

void ECE_CentipedeSegment::animate() {
    if (animationTick % 15 == 0) {
        textureIndex = (textureIndex + 1) % ((type == SegmentType::HEAD) ? headTextures.size() : bodyTextures.size());
        setTexture((type == SegmentType::HEAD) ? headTextures[textureIndex] : bodyTextures[textureIndex]);
    }
    animationTick++;
}

std::list<ECE_CentipedeSegment> ECE_CentipedeSegment::getTrailingBodies(ECE_CentipedeSegment& segment) {
    // Find the current segment in the centipede
    auto it = std::find(centipede.getSegments().begin(), centipede.getSegments().end(), segment);
    if (it == centipede.getSegments().end()) {
        // No trailing bodies if the segment is last
        return {};
    } else {
        // Find all consecutive living trailing bodies
        std::list<ECE_CentipedeSegment> trailingBodies;
        for (++it; it != centipede.getSegments().end(); ++it) {
            if (it->getType() == SegmentType::BODY && it->getStatus() == CharacterStatus::ALIVE) {
                trailingBodies.push_back(*it);
            } else {
                break;
            }
        }
        return trailingBodies;
    }
}

void ECE_CentipedeSegment::checkCollisions() {
    float playerY = player.getPosition().y;

    // Check for collisions with mushrooms
    for (Mushroom& mushroom : mushrooms) {
        if (mushroom.getHealth() > 0 && getGlobalBounds().intersects(mushroom.getGlobalBounds())) {
            // Teleport head to the closest open spot if it is stuck in a mushroom
            Vector2f newPosition = findClosestOpenSpot();
            setPosition(newPosition);
            break;
        }
        if (mushroom.getHealth() > 0 && getNextSegmentBounds().intersects(mushroom.getGlobalBounds())) {
            // Reverse direction if it is going to collide with a mushroom
            dx = -dx;
            dy = (centipede.getRandomWalk()) ? randomWalkDy : getSign(playerY - getPosition().y);
            break;
        }
    }

    // Check for collisions with other centipede segments that are not in the trailing bodies
    std::list<ECE_CentipedeSegment> trailingBodies = getTrailingBodies(*this);
    for (ECE_CentipedeSegment& segment : centipede.getSegments()) {
        // Cases to skip: same segment, dead segment, or segment in trailing bodies
        if (getId() == segment.getId() || segment.getStatus() == CharacterStatus::DEAD || inSegmentList(trailingBodies, segment)) {
            continue;
        }
        
        // Check for collisions with other segments and reverse direction if needed
        if (getNextSegmentBounds().intersects(segment.getGlobalBounds())) {
            dx = -dx;
            dy = (centipede.getRandomWalk()) ? randomWalkDy : getSign(playerY - getPosition().y);
            break;
        }
    }

    // Check for collisions with the horizontal window boundaries
    if (getPosition().x + dx * speed < 0 || getPosition().x + dx * speed + getGlobalBounds().width > windowWidth) {
        dx = getSign(0.5 * windowWidth - getPosition().x);
        dy = (centipede.getRandomWalk()) ? randomWalkDy : getSign(playerY - getPosition().y);
    }

    // Check for collisions with the vertical window boundaries
    if (getPosition().y + dy * speed < 0 || getPosition().y + dy * speed + getGlobalBounds().height > static_cast<int>(windowHeight / getGlobalBounds().height) * getGlobalBounds().height) {
        dy = (centipede.getRandomWalk()) ? getSign(0.5 * windowHeight - getPosition().y) : getSign(playerY - getPosition().y);
        if (centipede.getRandomWalk()) randomWalkDy = dy;
    }

    // Check if movement in current dx, dy is possible
    if (!segmentCanMove(getNextSegmentBounds((dy != 0) ? 0 : dx, dy))) {
        // Save dy if vertical movement is blocked and try horizontal movement
        if (dy != 0) {
            savedDy = dy;
            dy = 0;
        }

        // Check if horizontal movement is possible
        if (!segmentCanMove(getNextSegmentBounds(dx, 0))) {
            // Try other direction if horizontal movement is blocked
            dx = -dx;
        }
    }

    // If savedDy is non-zero, try overriding dy to savedDy once vertical movement is possible
    if (savedDy != 0) {
        if (segmentCanMove(getNextSegmentBounds(0, savedDy))) {
            dy = savedDy;
            savedDy = 0;
        }
    }
}

bool ECE_CentipedeSegment::segmentCanMove(FloatRect bounds) {
    std::list<ECE_CentipedeSegment> trailingBodies = getTrailingBodies(*this);
    for (auto& segment : centipede.getSegments()) {
        // Only account for living segments that are not in the trailing bodies
        if (getId() != segment.getId() && segment.getStatus() == CharacterStatus::ALIVE && bounds.intersects(segment.getGlobalBounds()) && !inSegmentList(trailingBodies, segment)) {
            return false;
        }
    }

    // Check for collisions with mushrooms
    for (auto& mushroom : mushrooms) {
        if (mushroom.getHealth() > 0 && bounds.intersects(mushroom.getGlobalBounds())) {
            return false;
        }
    }
    return true;
}

void ECE_CentipedeSegment::headMove() {
    int currentY = getPosition().y;
    int nextY = currentY + dy * speed;
    int size = getGlobalBounds().height;
    
    if (dy != 0) {
        // Moving vertically
        if (dy > 0) {
            // Moving down
            if (nextY % size == 0 || (currentY / size < nextY / size) && currentY % size != 0) {
                // Stop moving vertically if the next position is aligned to the grid
                int roundedY = (nextY / size) * size;
                setPosition(getPosition().x, roundedY);
                dy = 0;
            } else {
                // Continue moving vertically
                setPosition(getPosition().x, nextY);
            }
        } else if (dy < 0) {
            // Moving up
            if (nextY % size == 0 || (currentY / size > nextY / size) && currentY % size != 0) {
                // Stop moving vertically if the next position is aligned to the grid
                int roundedY = (currentY / size) * size;
                setPosition(getPosition().x, roundedY);
                dy = 0;
            } else {
                // Continue moving vertically
                setPosition(getPosition().x, nextY);
            }
        }
    } else {
        // Moving horizontally
        setPosition(getPosition().x + dx * speed, getPosition().y);
    }

    // Update texture orientation based on the current direction
    updateTextureOrientation();
}

void ECE_CentipedeSegment::bodyMove(int dx, int dy, float x, float y) {
    // Prioritize vertical movement
    moves.push(std::make_tuple(x, y, dx, dy));
    auto [nextX, nextY, nextDx, nextDy] = moves.front();
    moves.pop();
    setPosition(nextX, nextY);
    this->dx = nextDx;
    this->dy = nextDy;

    // Update texture orientation based on the current direction
    updateTextureOrientation();
}

FloatRect ECE_CentipedeSegment::getNextSegmentBounds(int useDx, int useDy) {
    FloatRect bounds = getGlobalBounds();
    int dx = (useDx == 999) ? this->dx : useDx;
    int dy = (useDy == 999) ? this->dy : useDy;
    bounds.left += dx * speed;
    bounds.top += dy * speed;
    return bounds;
}

void ECE_CentipedeSegment::updateTextureOrientation() {
    FloatRect bounds = getLocalBounds();
    float originX = 0;
    float originY = 0;

    // Determine rotation and scale for texture orientation
    float rotation = 0.0f;
    Vector2f scale(1.0f, 1.0f);

    if (dy != 0) {
        // Vertical movement: rotate texture
        rotation = (dy > 0) ? 90.0f : -90.0f; // Down: 90 degrees, Up: -90 degrees
        originX = (dy > 0) ? 0.0f : bounds.width;
        originY = (dy > 0) ? bounds.height : 0.0f;
    } else if (dx != 0) {
        // Horizontal movement: flip texture horizontally if moving left
        scale.x = (dx < 0) ? -1.0f : 1.0f;
        originX = (dx < 0) ? bounds.width : 0.0f;
    }

    // Apply rotation and scale
    setRotation(rotation);
    setScale(scale);

    // Reset origin so that the texture stays centered
    setOrigin(originX, originY);
}

ECE_Centipede::ECE_Centipede(int segmentCount, int initialSpeed) {
    length = segmentCount;
    this->initialSpeed = initialSpeed;
    speed = initialSpeed;
    randomWalk = false;
    
    // Initialize the first segment as the head
    for (int i = 0; i < length; i++) {
        ECE_CentipedeSegment segment(i == 0, initialSpeed);
        segment.setPosition((windowWidth / 2), 0);
        segments.push_back(segment);
    }
}

void ECE_Centipede::setRandomWalk(bool randomWalk) {
    this->randomWalk = randomWalk;
    if (randomWalk) segments.front().setRandomWalkDy(1);
    else segments.front().setRandomWalkDy(0);
}

void ECE_Centipede::setSpeed(int speed) {
    this->speed = speed;
    for (auto& segment : centipede.getSegments()) {
        segment.setSpeed(speed);
        segment.setMaxDelayTicks(segment.getGlobalBounds().width / speed);
        while (segment.getMoves().size() > segment.getMaxDelayTicks()) {
            segment.getMoves().pop();
        }
    }
}

void ECE_Centipede::move() {
    // Iterator to traverse the list of segments
    auto it = segments.begin();
    
    // Initialize saved values
    int savedDx = 0;
    int savedDy = 0;
    float savedX = 0;
    float savedY = 0;

    // Move each segment in the centipede
    while (it != segments.end()) {
        ECE_CentipedeSegment& currentSegment = *it;

        // Check if the current segment is a head
        if (currentSegment.getType() == SegmentType::HEAD && currentSegment.getStatus() == CharacterStatus::ALIVE) {
            // Determine the next move for the head
            currentSegment.checkCollisions();
            currentSegment.headMove();

            // Save the head's direction and position
            auto [x, y] = currentSegment.getPosition();
            auto [dx, dy] = currentSegment.getDirection();
            savedDx = dx;
            savedDy = dy;
            savedX = x;
            savedY = y;
        } else if (currentSegment.getStatus() == CharacterStatus::ALIVE) {
            // Move the body segment using the saved direction and position of previous segment
            currentSegment.bodyMove(savedDx, savedDy, savedX, savedY);

            // Update saved values to the current segment's new direction and position
            auto [dx, dy] = currentSegment.getDirection();
            auto [x, y] = currentSegment.getPosition();
            savedDx = dx;
            savedDy = dy;
            savedX = x;
            savedY = y;
        }

        ++it;
    }
}

Vector2f ECE_CentipedeSegment::findClosestOpenSpot() {
    auto openSpots = findOpenSpots();

    Vector2f currentPosition = getPosition();
    
    if (openSpots.empty()) {
        return currentPosition;
    }
    
    // Find the closest open spot
    Vector2f closestSpot = openSpots.front();
    float minDistance = std::numeric_limits<float>::max();
    
    for (const auto& spot : openSpots) {
        float distance = std::sqrt(std::pow(spot.x - currentPosition.x, 2) + std::pow(spot.y - currentPosition.y, 2));
        if (distance < minDistance) {
            minDistance = distance;
            closestSpot = spot;
        }
    }
    
    return closestSpot;
}

std::vector<Vector2f> ECE_CentipedeSegment::findOpenSpots() {
    std::vector<Vector2f> openSpots;
    
    // Iterate through the grid to find open spots
    for (float x = 0; x < static_cast<int>(windowWidth / getGlobalBounds().width) * getGlobalBounds().width; x += getGlobalBounds().width) {
        for (float y = 0; y < static_cast<int>(windowHeight / getGlobalBounds().width) * getGlobalBounds().height; y += getGlobalBounds().height) {
            FloatRect spotBounds(x, y, getGlobalBounds().width, getGlobalBounds().height);
            
            bool isOpen = true;

            // Check against mushrooms
            for (Mushroom& mushroom : mushrooms) {
                if (mushroom.getHealth() > 0 && spotBounds.intersects(mushroom.getGlobalBounds())) {
                    isOpen = false;
                    break;
                }
            }

            // Check against other centipede segments
            if (isOpen) {
                for (ECE_CentipedeSegment& segment : centipede.getSegments()) {
                    if (getId() != segment.getId() && segment.getStatus() == CharacterStatus::ALIVE && spotBounds.intersects(segment.getGlobalBounds()) && !inSegmentList(getTrailingBodies(*this), segment)) {
                        isOpen = false;
                        break;
                    }
                }
            }
            
            if (isOpen) {
                openSpots.emplace_back(x, y);
            }
        }
    }
    
    return openSpots;
}

void ECE_Centipede::reset(bool resetSpeed) {
    segments.clear();
    if (resetSpeed) {
        speed = initialSpeed;
    }
    for (int i = 0; i < length; i++) {
        ECE_CentipedeSegment segment(i == 0, (resetSpeed) ? initialSpeed : speed);
        segment.setPosition((windowWidth / 2), 0);
        segments.push_back(segment);
    }
}

void ECE_Centipede::draw() {
    for (auto it = segments.rbegin(); it != segments.rend(); ++it) {
        if (it->getStatus() == CharacterStatus::ALIVE) {
            it->animate();
            window.draw(*it);
        }
    }
}
