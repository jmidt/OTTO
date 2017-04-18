#pragma once

#include <cairomm/cairomm.h>
#include <thread>

#include "../module.h"

namespace ui {

typedef Cairo::RefPtr<Cairo::Context> ContextPtr;

/**
 * Used for keypresses
 */
enum Key {
  K_NONE = 0,

  K_RED_UP,
  K_RED_DOWN,
  K_BLUE_UP,
  K_BLUE_DOWN,
  K_WHITE_UP,
  K_WHITE_DOWN,
  K_GREEN_UP,
  K_GREEN_DOWN,

  // Tapedeck
  K_PLAY,
  K_REC,
  K_TRACK_1,
  K_TRACK_2,
  K_TRACK_3,
  K_TRACK_4,

  // Globals:
  K_QUIT,

  // Numbers
  K_1,
  K_2,
  K_3,
  K_4,
  K_5,
  K_6,
  K_7,
  K_8,
  K_9,
  K_0,
};


/**
 * Anything that can be drawn on screen.
 * Holds a pointer to its parent
 */
class Widget {
public:

  Widget *parent;

  Widget() {};
  Widget(Widget *_parent) :
    parent (_parent) {}

  /**
   * Draw this widget to the context.
   * Called from the parent's draw method.
   * @param cr the Cairo context to draw to.
   */
  virtual void draw(const ContextPtr& cr) = 0;

};


class Screen : public Widget {
public:

  Screen() : Widget(NULL) {}
  /**
   * Run by MainUI when a key is pressed
   * @param key the pressed key
   * @return true if the key was used.
   */
  virtual bool keypress(Key key) {};
};

/**
 * A specific view/window.
 * Each module will probably have at least one
 */
template<class M>
class ModuleScreen : public Screen {
protected:
  M *module;

public:

  ModuleScreen() : Screen() {}
  ModuleScreen(M *module)
    : Screen(),
    module (module) {}
};


/**
 * The default screen, shown on boot
 */
class DefaultScreen : public Screen {
public:
  void draw(const ContextPtr& cr) override;
  bool keypress(Key key) override;
};

}