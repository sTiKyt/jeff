#ifndef A_LINE_H
#define A_LINE_H

#include "widgets/a_board.h"
#include "widgets/a_lineedit.h"
#include "widgets/a_pushbutton.h"
#include <QFontDatabase>
#include <QLayout>
#include <QLineEdit>
#include <QWidget>

class ALine : public ABoard {
  Q_OBJECT
public:
  ALine(QWidget *parent = nullptr);
  ALineEdit *textLine;
  APushButton *sendButton;

private:
  Q_DISABLE_COPY(ALine)
};

#endif // A_LINE_H