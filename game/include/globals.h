#ifndef GLOBALS_H
#define GLOBALS_H

#include <SFML/Graphics.hpp>

using namespace sf;

extern RenderWindow window;
extern int globalCounter;
extern int windowWidth, windowHeight;

enum class Screen {
    HOME,
    GAME
};

enum SegmentType {
    HEAD,
    BODY
};

enum CharacterStatus {
    ALIVE,
    DEAD
};

enum CollisionType {
    REDIRECT,
    DESTROY
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

#endif