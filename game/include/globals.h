/*
Author: Matthew Hua
Class: ECE4122
Last Date Modified: 09-09-2024
Description:
Lab 1: Centipede Game
*/

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