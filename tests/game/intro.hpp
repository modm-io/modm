
#ifndef INTRO_HPP
#define INTRO_HPP

#include <xpcc/driver/lcd/graphic_display.hpp>
#include <xpcc/workflow.hpp>

#include "intro/monster.hpp"
#include "intro/monster_ship.hpp"
#include "intro/human.hpp"
#include "intro/missle.hpp"

#include "window.hpp"

class Intro : public xpcc::Protothread, public Window
{
public:
	Intro(xpcc::GraphicDisplay *display);
	
	bool
	update(Action action);
	
private:
	Monster monster;
	MonsterShip ship;
	Human human;
	Missle missle;
	
	bool missleLaunched;
};

#endif // INTRO_HPP
