/*
Author: Matthew Hua
Class: ECE4122
Last Date Modified: 09-09-2024
Description:
Lab 1: Centipede Game
*/

#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <locale>
#include <array>
#include <SFML/Graphics.hpp>
#include "mushroom.h"
#include "laserBlaster.h"
#include "centipede.h"
#include "spider.h"
#include "globals.h"

using namespace sf;

RenderWindow window;
Texture startupLogo;
Sprite startupSprite;
Texture backgroundTexture;
Sprite backgroundSprite;

Texture backgroundTextures[3]; ///< Background texture variants.
Texture starShipTextures[3]; ///< Starship texture variants.
Texture laserTextures[3]; ///< Laser texture variants.
Texture normalMushroomTextures[3]; ///< Normal mushroom texture variants.
Texture damagedMushroomTextures[3]; ///< Damaged mushroom texture variants.
std::vector<std::array<Texture, 3>> headTexturesVariants; ///< Head texture variants.
std::vector<std::array<Texture, 3>> bodyTexturesVariants; ///< Body texture variants.
std::vector<std::array<Texture, 3>> spiderTexturesVariants; ///< Spider texture variants.

int globalCounter = 0; ///< Global counter for creating unique IDs.
int colorSwapIndex = 0; ///< Index for the current color variant.
int windowWidth = 1080;
int windowHeight = 680;

/**
 * @brief Captures keyboard inputs and returns the corresponding direction.
 *
 * @return Direction The direction corresponding to the pressed arrow key, or Direction::NONE if no arrow key is pressed.
 */
Direction getInputs() {
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        return Direction::LEFT;
    }
    else if (Keyboard::isKeyPressed(Keyboard::Right)) {
        return Direction::RIGHT;
    }
    else if (Keyboard::isKeyPressed(Keyboard::Up)) {
        return Direction::UP;
    }
    else if (Keyboard::isKeyPressed(Keyboard::Down)) {
        return Direction::DOWN;
    }
    return Direction::NONE;
}

/**
 * @brief Formats a number into a string with commas.
 * 
 * @param number The number to format.
 */
std::string formatWithCommas(int number) {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << std::setprecision(0) << number;
    return ss.str();
}

/**
 * @brief Updates the life sprites based on the player's current lives.
 *
 * @param livesSprites A reference to a vector of Sprite objects representing the life sprites.
 * @param totalWidth A reference to a float that will store the total width of all life sprites.
 */
void updateLifeSprites(std::vector<Sprite>& livesSprites, float& totalWidth) {
    livesSprites.clear();
    int totalLives = player.getLives();
    totalWidth = 0;

    for (int i = 0; i < totalLives; ++i) {
        Sprite lifeSprite(starShipTextures[colorSwapIndex]);
        totalWidth += lifeSprite.getGlobalBounds().width;
    }

    float startX = windowWidth - totalWidth;

    for (int i = 0; i < totalLives; ++i) {
        Sprite lifeSprite(starShipTextures[colorSwapIndex]);
        lifeSprite.setPosition(startX + i * lifeSprite.getGlobalBounds().width - 10, 0);
        livesSprites.push_back(lifeSprite);
    }
}

/**
 * @brief Draws the player's remaining lives on the screen.
 * 
 * @param livesSprites A vector containing the sprites that represent the player's lives.
 */
void drawLives(const std::vector<Sprite>& livesSprites) {
    for (const auto& lifeSprite : livesSprites) {
        window.draw(lifeSprite);
    }
}

/**
 * @brief Rotates the RGB values of all pixels in a texture.
 *
 * @param texture The texture whose color channels will be swapped.
 */
void swapRGB(Texture& texture) {
    Image image = texture.copyToImage();
    Vector2u size = image.getSize();

    for (unsigned int y = 0; y < size.y; ++y) {
        for (unsigned int x = 0; x < size.x; ++x) {
            Color color = image.getPixel(x, y);
            Color newColor(color.b, color.r, color.g, color.a);
            image.setPixel(x, y, newColor);
        }
    }

    texture.update(image);
}

/**
 * @brief Creates texture variants for different game elements by swapping their RGB values.
 */
void createTextureVariants() {
    headTexturesVariants.resize(headTextures.size());
    bodyTexturesVariants.resize(bodyTextures.size());
    spiderTexturesVariants.resize(spiderTextures.size());

    for (int i = 0; i < headTextures.size(); ++i) {
        headTexturesVariants[i] = {headTextures[i]};
        for (int j = 1; j < 3; ++j) {
            Texture variant = (j > 0) ? headTexturesVariants[i][j - 1] : headTexturesVariants[i][0];
            swapRGB(variant);
            headTexturesVariants[i][j] = variant;
        }
    }

    for (int i = 0; i < bodyTextures.size(); ++i) {
        bodyTexturesVariants[i] = {bodyTextures[i]};
        for (int j = 1; j < 3; ++j) {
            Texture variant = (j > 0) ? bodyTexturesVariants[i][j - 1] : bodyTexturesVariants[i][0];
            swapRGB(variant);
            bodyTexturesVariants[i][j] = variant;
        }
    }

    for (int i = 0; i < spiderTextures.size(); ++i) {
        spiderTexturesVariants[i] = {spiderTextures[i]};
        for (int j = 1; j < 3; ++j) {
            Texture variant = (j > 0) ? spiderTexturesVariants[i][j - 1] : spiderTexturesVariants[i][0];
            swapRGB(variant);
            spiderTexturesVariants[i][j] = variant;
        }
    }

    for (int i = 0; i < 3; ++i) {
        Texture backgroundTextureVariant = (i > 0) ? backgroundTextures[i - 1] : backgroundTexture;
        if (i > 0) swapRGB(backgroundTextureVariant);
        backgroundTextures[i] = backgroundTextureVariant;

        Texture starShipVariant = (i > 0) ? starShipTextures[i - 1] : starShipTexture;
        if (i > 0) swapRGB(starShipVariant);
        starShipTextures[i] = starShipVariant;

        Texture laserVariant = (i > 0) ? laserTextures[i - 1] : laserTexture;
        if (i > 0) swapRGB(laserVariant);
        laserTextures[i] = laserVariant;

        Texture normalMushroomVariant = (i > 0) ? normalMushroomTextures[i - 1] : normalMushroomTexture;
        if (i > 0) swapRGB(normalMushroomVariant);
        normalMushroomTextures[i] = normalMushroomVariant;

        Texture damagedMushroomVariant = (i > 0) ? damagedMushroomTextures[i - 1] : damagedMushroomTexture;
        if (i > 0) swapRGB(damagedMushroomVariant);
        damagedMushroomTextures[i] = damagedMushroomVariant;
    }
}

/**
 * @brief Rotates the colors of all textures to the next set of variants.
 * 
 * @param index Optional parameter to specify the color variant index. If not provided,
 *              the function will use the next color variant in the sequence.
 */
void rotateAllTextureColors(int index=-1) {
    colorSwapIndex = (index != -1) ? index : (colorSwapIndex + 1) % 3;

    for (int i = 0; i < headTextures.size(); ++i) {
        headTextures[i] = headTexturesVariants[i][colorSwapIndex];
    }
    for (int i = 0; i < bodyTextures.size(); ++i) {
        bodyTextures[i] = bodyTexturesVariants[i][colorSwapIndex];
    }
    for (int i = 0; i < spiderTextures.size(); ++i) {
        spiderTextures[i] = spiderTexturesVariants[i][colorSwapIndex];
    }
    starShipTexture = starShipTextures[colorSwapIndex];
    laserTexture = laserTextures[colorSwapIndex];
    normalMushroomTexture = normalMushroomTextures[colorSwapIndex];
    damagedMushroomTexture = damagedMushroomTextures[colorSwapIndex];
    backgroundTexture = backgroundTextures[colorSwapIndex];
}

/**
 * @brief Resets all texture colors to their initial state.
 */
void resetAllTextureColors() {
    if (colorSwapIndex == 0) return;
    colorSwapIndex = 0;
    rotateAllTextureColors(colorSwapIndex);
}

int main() {
    // Create the window
    window.create(VideoMode(windowWidth, windowHeight), "Centipede", Style::Default);

    // Load the github logo
    if (!startupLogo.loadFromFile("assets/StartupLogo.png")) {
        printf("Failed to load texture from %s\n", "assets/StartupLogo.png");
        return -1;
    }
    // Scale and position the startup logo
    startupSprite.setTexture(startupLogo);
    float widthScale = static_cast<float>(windowWidth) / startupLogo.getSize().x;
    float heightScale = static_cast<float>(windowHeight) / startupLogo.getSize().y;
    FloatRect startupBounds = startupSprite.getLocalBounds();
    startupSprite.setOrigin(0.5f * startupBounds.width, 0.5f * startupBounds.height);
    startupSprite.setScale(std::min(widthScale, heightScale), std::min(widthScale, heightScale));
    startupSprite.setPosition(0.5f * windowWidth, 0.5f * windowHeight);

    // Display the startup logo
    window.clear();
    window.draw(startupSprite);
    window.display();
    Event event;
    window.pollEvent(event);

    // Load the background texture
    if (!backgroundTexture.loadFromFile("assets/DirtBackground.png")) {
        printf("Failed to load texture from %s\n", "assets/DirtBackground.png");
        return -1;
    }

    // Scale the background texture
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(
        static_cast<float>(windowWidth) / backgroundTexture.getSize().x,
        static_cast<float>(windowHeight) / backgroundTexture.getSize().y
    );

    // Game timing variables
    Clock clock;
    const float targetFrameTime = 1.0f / 60.0f;
    float deltaTime;

    // The current screen being displayed
    Screen currentScreen = Screen::HOME;

    // Initialize the game elements
    int centipedeLength = 12;
    int initialCentipedeSpeed = 2;
    centipedeInit(centipedeLength, initialCentipedeSpeed);
    mushroomInit();
    laserBlasterInit();
    int initialSpiderSpeed = 2;
    spiderInit(initialSpiderSpeed);

    // Generate texture color variants for all game textures
    createTextureVariants();

    // Initialize the text elements
    std::stringstream str;
    Text scoreText;
    Text highScoreText;
    Text livesLabelText;
    Text titleText;
    Text messageText;

    // Load the font
    Font font;
    if (!font.loadFromFile("fonts/KOMIKAP_.ttf")) {
        printf("Failed to load font from %s\n", "fonts/KOMIKAP_.ttf");
        return -1;
    }

    // Initialize all text elements
    highScoreText.setFont(font);
    scoreText.setFont(font);
    livesLabelText.setFont(font);
    titleText.setFont(font);
    messageText.setFont(font);
    std::vector<Sprite> livesSprites;
    float totalLivesWidth;
    updateLifeSprites(livesSprites, totalLivesWidth);

    highScoreText.setString("High Score: 0");
    scoreText.setString("Score: 0");
    livesLabelText.setString("Lives: ");
    titleText.setString("ECE Centipede");
    messageText.setString("Enter to Start");

    highScoreText.setCharacterSize(30);
    scoreText.setCharacterSize(30);
    livesLabelText.setCharacterSize(30);
    titleText.setCharacterSize(50);
    messageText.setCharacterSize(30);

    highScoreText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);
    livesLabelText.setFillColor(Color::White);
    titleText.setFillColor(Color::White);
    messageText.setFillColor(Color::Green);

    FloatRect highScoreBounds = highScoreText.getLocalBounds();
    highScoreText.setOrigin(0, 0.5f * highScoreBounds.height);
    highScoreText.setPosition(10, 10);

    FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setOrigin(0.5f * scoreBounds.width, 0.5f * scoreBounds.height);
    scoreText.setPosition(0.5f * windowWidth, 10);

    FloatRect livesLabelBounds = livesLabelText.getLocalBounds();
    livesLabelText.setOrigin(0, 0.5f * livesLabelBounds.height);
    livesLabelText.setPosition(windowWidth - totalLivesWidth - livesLabelBounds.width - 10, 10);

    FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(0.5f * titleBounds.width, 0.5f * titleBounds.height);
    titleText.setPosition(0.5f * windowWidth, 0.33f * windowHeight);

    FloatRect messageBounds = messageText.getLocalBounds();
    messageText.setOrigin(0.5f * messageBounds.width, 0.5f * messageBounds.height);
    messageText.setPosition(0.5f * windowWidth, 0.66f * windowHeight);

    // Main game loop
    while (window.isOpen()) {
        // Handle close window events
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed || (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)) {
                window.close();
            }
        }

        // Maintain frame rate
        Time frameTime = clock.restart();
        deltaTime = frameTime.asSeconds();
        Time sleepTime = Time::Zero;
        if (deltaTime < targetFrameTime) {
            sleepTime = seconds(targetFrameTime) - frameTime;
            sleep(sleepTime);
        }

        window.clear();

        // Draw the appropriate screen
        switch (currentScreen) {
            case Screen::HOME:
                // Background game simulation
                if (mushrooms.size() == 0) {
                    generateMushrooms();
                }
                if (!centipede.getRandomWalk()) centipede.setRandomWalk(true);
                centipede.move();

                resetAllTextureColors();
                window.draw(backgroundSprite);
                centipede.draw();
                drawMushrooms();
                window.draw(titleText);
                window.draw(messageText);
                highScoreText.setOrigin(0.5f * highScoreBounds.width, 0.5f * highScoreBounds.height);
                highScoreText.setPosition(0.5f * windowWidth, 0.5f * windowHeight);
                window.draw(highScoreText);
                if (Keyboard::isKeyPressed(Keyboard::Return)) {
                    // Start the game
                    currentScreen = Screen::GAME;
                    highScoreText.setOrigin(0, 0.5f * highScoreBounds.height);
                    highScoreText.setPosition(10, 10);
                    generateMushrooms();
                    centipede.setRandomWalk(false);
                    centipede.reset();
                    player.reset();
                    spider.reset();
                }
                break;
            case Screen::GAME:
                // Update text elements
                window.draw(backgroundSprite);
                str << "Score: " << formatWithCommas(player.getScore());
                scoreText.setString(str.str());
                str.str("");
                player.updateHighScore();
                str << "High Score: " << formatWithCommas(player.getHighScore());
                highScoreText.setString(str.str());
                str.str("");
                updateLifeSprites(livesSprites, totalLivesWidth);

                // Check if player wants to shoot
                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                    player.shoot();
                }

                // Update all game elements
                player.update(getInputs());
                centipede.move();
                spider.update();

                // Draw all game elements
                drawMushrooms();
                centipede.draw();
                spider.draw();
                player.draw();
                window.draw(scoreText);
                window.draw(highScoreText);
                window.draw(livesLabelText);
                drawLives(livesSprites);

                // Check if the centipede is dead
                bool centipedeAlive = false;
                for (ECE_CentipedeSegment& segment : centipede.getSegments()) {
                    if (segment.getStatus() == CharacterStatus::ALIVE) {
                        centipedeAlive = true;
                        break;
                    }
                }
                // Spawn a new centipede if the current one is dead and rotate the texture colors
                if (!centipedeAlive) {
                    centipede.reset(false);
                    centipede.setSpeed(centipede.getSpeed() + 1);
                    spider.setSpeed(spider.getSpeed() + 1);
                    rotateAllTextureColors();
                }

                // Check if the player is dead
                if (player.getLives() == 0) {
                    centipede.reset(true);
                    mushrooms.clear();
                    player.updateHighScore();
                    str << "High Score: " << formatWithCommas(player.getHighScore());
                    highScoreText.setString(str.str());
                    str.str("");
                    currentScreen = Screen::HOME;
                }
                break;
        }

        window.display();
    }

    return 0;
}
