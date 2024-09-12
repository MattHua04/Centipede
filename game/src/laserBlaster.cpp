#include "laserBlaster.h"

Texture laserTexture, starShipTexture;
ECE_LaserBlaster player(0, 0, 0);

void laserBlasterInit() {
    // Create the laser blast texture
    Image laserBlast;
    int width = 5;
    int height = 15;
    laserBlast.create(width, height, Color::Red);
    laserTexture.loadFromImage(laserBlast);

    // Load the starship texture
    if (!starShipTexture.loadFromFile("assets/StarShip.png")) {
        printf("Failed to load texture from %s\n", "assets/StarShip.png");
    }

    // Initialize the player
    player = ECE_LaserBlaster(3, 10, 0.25f);
}

ECE_LaserBlast::ECE_LaserBlast(float blastSpeed) {
    setTexture(laserTexture);
    speed = blastSpeed;
    id = globalCounter++;
}

bool ECE_LaserBlast::move() {
    setPosition(getPosition().x, getPosition().y - speed);
    bool remove = handleCollision();
    return remove;
}

bool ECE_LaserBlast::handleCollision() {
    // Handle laser blast collision with the window bounds
    if (getPosition().y < 0 || getPosition().y > windowHeight) {
        return true;
    }

    // Check collision with mushrooms
    for (auto& mushroom : mushrooms) {
        if (mushroom.getHealth() > 0 && getGlobalBounds().intersects(mushroom.getGlobalBounds())) {
            mushroom.handleCollision();
            return true;
        }
    }

    // Check collision with centipede segments
    auto& segments = centipede.getSegments();
    for (auto it = segments.begin(); it != segments.end(); ++it) {
        ECE_CentipedeSegment& segment = *it;
        if (segment.getStatus() == CharacterStatus::ALIVE && getGlobalBounds().intersects(segment.getGlobalBounds())) {
            segment.setStatus(CharacterStatus::DEAD);
            
            // Increment player score by 100 for head segment, 10 for body segment
            if (segment.getType() == SegmentType::HEAD) {
                player.incrementScore(100);
            } else {
                player.incrementScore(10);
            }

            // Save the iterator before tyring to change the next segment to a head
            auto currentIt = it;
            ++it;

            // Check if the next segment exists and is alive before changing it to a head
            if (it != segments.end() && it->getStatus() == CharacterStatus::ALIVE) {
                ECE_CentipedeSegment& nextSegment = *it;
                nextSegment.setType(SegmentType::HEAD);
                nextSegment.setTexture(headTextures[nextSegment.getTextureIndex()]);
            }

            // Spawn a mushroom at the location of the destroyed segment
            addMushroom(segment.getPosition().x, segment.getPosition().y);

            return true;
        }
    }

    // Check collision with spider and increment player score by 500 on hit
    if (spider.getStatus() == CharacterStatus::ALIVE && spider.getGlobalBounds().intersects(getGlobalBounds())) {
        spider.handleCollision();
        player.incrementScore(500);
        return true;
    }

    return false;
}

ECE_LaserBlaster::ECE_LaserBlaster(float speed, float blastSpeed, float reloadTime) {
    this->speed = speed;
    this->blastSpeed = blastSpeed;
    lives = 3;
    score = 0;
    highScore = 0;
    shotDelay = reloadTime;
    shotClock.restart();
    setTexture(starShipTexture);
    setPosition(windowWidth / 2, windowHeight - 2 * getGlobalBounds().height);
}

void ECE_LaserBlaster::shoot() {
    // Check if the reload time has passed
    if (shotClock.getElapsedTime().asSeconds() < shotDelay) {
        return;
    }

    // Reset the reload cooldown and fire a new blast
    shotClock.restart();
    ECE_LaserBlast blast(blastSpeed);
    blast.setPosition(getPosition().x + 0.5 * getGlobalBounds().width - 0.5 * laserTexture.getSize().x, getPosition().y);
    blasts.push_back(blast);
}

void ECE_LaserBlaster::update(Direction direction) {
    Vector2f position = getPosition();
    FloatRect bounds = getGlobalBounds();

    // Lambda function to check for collision with mushrooms
    auto mushroomCollision = [&](Vector2f newPos) {
        FloatRect newBounds(newPos.x, newPos.y, bounds.width, bounds.height);
        for (auto& mushroom : mushrooms) {
            if (mushroom.getHealth() > 0 && mushroom.getGlobalBounds().intersects(newBounds)) {
                return true;
            }
        }
        return false;
    };

    // Lambda function to check for collision with centipede segments
    auto centipedeCollision = [&]() {
        for (auto& segment : centipede.getSegments()) {
            if (segment.getStatus() == CharacterStatus::ALIVE && getGlobalBounds().intersects(segment.getGlobalBounds())) {
                return true;
            }
        }
        return false;
    };

    // Lambda function to check for collision with the spider
    auto spiderCollision = [&]() {
        if (spider.getStatus() == CharacterStatus::ALIVE && spider.getGlobalBounds().intersects(getGlobalBounds())) {
            return true;
        }
        return false;
    };

    // Process movement based on input direction
    switch (direction) {
        case Direction::UP:
            if (position.y - speed >= 0.5f * windowHeight && !mushroomCollision({position.x, position.y - speed})) {
                setPosition(position.x, position.y - speed);
            }
            break;
        case Direction::DOWN:
            if (position.y + bounds.height + speed <= windowHeight && 
                !mushroomCollision({position.x, position.y + speed})) {
                setPosition(position.x, position.y + speed);
            }
            break;
        case Direction::LEFT:
            if (position.x - speed >= 0 && !mushroomCollision({position.x - speed, position.y})) {
                setPosition(position.x - speed, position.y);
            }
            break;
        case Direction::RIGHT:
            if (position.x + bounds.width + speed <= windowWidth &&
                !mushroomCollision({position.x + speed, position.y})) {
                setPosition(position.x + speed, position.y);
            }
            break;
        case Direction::NONE:
        default:
            break;
    }

    // Check for collision with centipede or spider
    if (centipedeCollision() || spiderCollision()) {
        player.decrementLives();
        centipede.reset(false);
        resetPosition();
    }

    // Update the laser blasts and handle collisions
    for (auto it = blasts.begin(); it != blasts.end();) {
        bool remove = it->move();
        if (remove) {
            it = blasts.erase(it);
        } else {
            ++it;
        }
    }
}

void ECE_LaserBlaster::resetPosition() {
    setPosition(windowWidth / 2, windowHeight - 2 * getGlobalBounds().height);
}

void ECE_LaserBlaster::reset() {
    blasts.clear();
    resetScore();
    resetLives();
    resetPosition();
}

void ECE_LaserBlaster::draw() {
    window.draw(*this);
    for (auto& blast : blasts) {
        window.draw(blast);
    }
}