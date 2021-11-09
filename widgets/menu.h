#ifndef MENU_H
#define MENU_H

#include <QMenu>

/*!
 * @class Menu
 * @brief Menu with a special style of design.
 */
class Menu : public QMenu {
  Q_OBJECT
public:
  // Functions described in `menu.cpp`:
  Menu(QWidget *parent);
};

#endif