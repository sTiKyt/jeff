#include "button.hpp"

/*! @brief The constructor. */
Button::Button(const QString &text, QWidget *parent) : QPushButton(parent) {
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  setFocusPolicy(Qt::NoFocus);
  setText(text);
  setStyleSheet(QString(
    "Button { text-align: center; border-width: 1px; border-style: solid; border-radius: 6px;"
    "border-color: %1; background-color: %1; color: %2; padding: 2px; } Button:pressed, Button:open"
    " { border-color: %3; border-width: 2px; } Button:hover { border-color: %3; }"
  ).arg(styling.css_bg_color).arg(styling.css_fg_color).arg(styling.css_hl_color));
}

/*! @brief Sets a popup menu on button click. */
void Button::setMenu(Menu *menu) {
  disconnect(this, &Button::clicked, nullptr, nullptr);
  connect(this, &Button::clicked, this, [this, menu] {
    if (not parent()) return;
    QPoint pos = qobject_cast<QWidget *>(parent())->mapToGlobal(this->pos());
    pos += QPoint(0, height());
    menu->exec(pos);
  });
} 
