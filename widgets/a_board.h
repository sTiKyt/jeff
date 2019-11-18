#ifndef A_BOARD_H
#define A_BOARD_H

#include <QFrame>
#include <QGridLayout>
#include <QWidget>

/*!
 * Class: ABoard.
 * Shell widget. Looks like a white rectangle with rounded edges.
 */
class ABoard : public QFrame {
  Q_OBJECT
 public:
  // Functions:
  ABoard(QWidget *parent = nullptr);
  const QString wss =
      "#a_board {border-width: 4px; border-style: solid; border-color: "
      "#ffec4d; border-radius: 6px; background-color: #ffec4d;} * { color: "
      "white; }";
  const QString ess =
      "#a_board {border-width: 4px; border-style: solid; border-color: "
      "#e8553a; border-radius: 6px; background-color: #e8553a;} * { color: "
      "white; }";

 private:
  Q_DISABLE_COPY(ABoard)

  // Objects:
  const QString objn = "a_board";
  const QString ss =
      "#a_board {border-width: 4px; border-style: solid; border-color: white; "
      "border-radius: 6px; background-color: white;} * { color: black; }";
};

#endif
