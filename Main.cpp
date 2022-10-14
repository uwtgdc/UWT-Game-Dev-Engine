/*
* Game engine foundation
* @author: Justin Goding
* @version date: 2/4/2022
* Some framework code from Professor Dave Churchill at Memorial University
* */

#include <SFML/Graphics.hpp>

#include "Game_Engine.h"

int main()
{
    Game_Engine g("assets.txt");
    g.run();
}