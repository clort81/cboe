//
//  winutil.h
//  BoE
//
//  Created by Celtic Minstrel on 14-03-28.
//
//

#ifndef BoE_boe_windows_h
#define BoE_boe_windows_h

#include <boost/filesystem/path.hpp>
#include <SFML/Window.hpp>

namespace fs = boost::filesystem; // TODO: Centralize this alias

char keyToChar(sf::Keyboard::Key key, bool isShift);

void makeFrontWindow(sf::Window& win);
void setWindowFloating(sf::Window& win, bool floating);

void init_fileio();

fs::path nav_get_party();
fs::path nav_put_party();
fs::path nav_get_scenario();
fs::path nav_put_scenario();

class ModalSession {
	void* session;
public:
	void pumpEvents();
	ModalSession(sf::Window& win);
	~ModalSession();
};

// System key
#ifdef __APPLE__
bool sf::Event::KeyEvent::*const systemKey = &sf::Event::KeyEvent::system;
#else
bool sf::Event::KeyEvent::*const systemKey = &sf::Event::KeyEvent::control;
#endif

#endif