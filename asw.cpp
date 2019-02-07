#include "asw.h"

ASW::ASW(QWidget *parent) : QMainWindow(parent) {
  // Removes interface padding, making it smaller...
  this->layout()->setMargin(0);
  this->layout()->setSpacing(0);
  // Creates central widget and connected widgets...
  QWidget *central = new QWidget();
  central->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  central->setObjectName("asw_bg");
  central->setStyleSheet("#asw_bg {"
                         "background-image: url(:/arts/background.jpg);"
                         "background-position: center center;"
                         "background-repeat:  no-repeat;"
                         "background-attachment: fixed;"
                         "}");
  QLayout *centralLayout = new QVBoxLayout();
  centralLayout->setSpacing(0);
  this->display = new AkiwakeDisplay();
  this->line = new AkiwakeLine();
  this->mBar = new AkiwakeMenuBar(this->line);
  // Appends widgets and output...
  central->setLayout(centralLayout);
  central->layout()->addWidget(this->display);
  central->layout()->addWidget(this->line);
  this->setMenuBar(this->mBar);
  this->setCentralWidget(central);
  QSqlDatabase::addDatabase("QSQLITE");
  this->connector();
  this->applyingSettings();
  // Creates a welcome message...
  thinking TH;
  TH.putExpression("Hello!");
  TH.startProcessing();
  this->addMessage(AkiwakeMessage::ASW, TH.get());
}

void ASW::applyingSettings() {
  auto *ST = new SettingsStore();
  this->setWindowTitle(ST->application);
  // It's not allowed to reduce the size of the
  // window is less than these values...
  this->setMinimumSize(300, 150);
  this->resize(800, 496);
  // Restores window settings...
  if (ST->read("asw/size").toSize() != QSize(-1, -1))
    this->resize(ST->read("asw/size").toSize());
  if (ST->read("asw/isfullscreen") == true) {
    this->showFullScreen();
    this->mBar->fullScreen->setChecked(true);
  }
  if (ST->read("asw/menubarishidden") == true)
    this->mBar->setVisible(false);
  delete ST;
}

void ASW::connector() {
  // Connects signals with slots.
  connect(this->line->sendButton, &AkiwakePushButton::clicked, this,
          &ASW::userSendsMessage);
  connect(this->mBar, &AkiwakeMenuBar::fullscreenModeChanged, this,
          &ASW::fullscreenHandler);
  connect(this->mBar, &AkiwakeMenuBar::clearScreenPressed, this,
          &ASW::clearScreen);
}

ASW::~ASW() {
  // Saves the window settings.
  auto *ST = new SettingsStore();
  ST->write("asw/size", this->size());
  ST->write("asw/menubarishidden", this->mBar->isHidden());
  ST->write("asw/isfullscreen", this->isFullScreen());
  delete ST;
}

void ASW::resizeEvent(QResizeEvent *event) {
  // Calibrates all messages.
  for (auto message : this->messages)
    message->textLayoutDesigner(this->width());
  event->accept();
}

void ASW::addMessage(AkiwakeMessage::AuthorType Author, QString Text = "") {
  // Text preparation...
  if (Author == AkiwakeMessage::User) {
    Text = this->line->textLine->text();
    this->line->textLine->clear();
    this->display->scrollEnabled = true;
  }
  if (Text.trimmed() == "")
    return;
  AkiwakeMessage *msg =
      new AkiwakeMessage(Text, Author, this->themeFolder, this->display);
  // Calibrates the message and add it to the screen...
  msg->textLayoutDesigner(this->width());
  this->current = msg;
  this->messages.append(msg);
  this->display->layout->addWidget(this->current);
  // Responses to user expression...
  if (Author == AkiwakeMessage::User) {
    thinking TH;
    TH.putExpression(Text);
    TH.startProcessing();
    this->addMessage(AkiwakeMessage::ASW, TH.get());
  }
}

void ASW::userSendsMessage() {
  // Redirecting...
  this->addMessage(AkiwakeMessage::User);
}

void ASW::keyPressEvent(QKeyEvent *event) {
  if ((event->modifiers() == Qt::ControlModifier) &&
      (event->key() == Qt::Key_H)) {
    if (this->mBar->isVisible())
      this->mBar->setVisible(false);
    else
      this->mBar->setVisible(true);
  }
}

void ASW::fullscreenHandler() {
  if (this->mBar->fullScreen->isChecked() == true)
    this->showFullScreen();
  else
    this->showNormal();
}

void ASW::clearScreen() {
  foreach (AkiwakeMessage *msg, this->messages) {
    this->display->layout->removeWidget(msg);
    delete msg;
  }
  this->messages.clear();
  this->current = nullptr;
}
