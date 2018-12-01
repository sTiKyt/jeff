#include "akiwake_line.h"

AkiwakeLine::AkiwakeLine(QWidget *parent) : AkiwakeBoard(parent) {
  // Creates a user input widget.
  QLayout *centralLayout = new QHBoxLayout();
  centralLayout->setContentsMargins(0, 0, 0, 0);
  this->textLine = new AkiwakeLineEdit();
  this->sendButton = new AkiwakePushButton("Send");
  this->sendButton->setFocusPolicy(Qt::NoFocus);
  connect(this->textLine, &QLineEdit::returnPressed, this->sendButton,
          &AkiwakePushButton::click);
  centralLayout->setSpacing(0);
  centralLayout->addWidget(this->textLine);
  centralLayout->addWidget(this->sendButton);
  this->centralWidget->setLayout(centralLayout);
}

AkiwakeLine::~AkiwakeLine() {
  // Prevents memory leaks.
  delete this->sendButton;
  delete this->textLine;
}
