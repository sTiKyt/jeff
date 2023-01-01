#include "display.h"
#include <iostream>

/*! @brief The constructor. */
Display::Display(HProcessor *_hp, short _max_message_amount, QWidget *parent)
    : QScrollArea(parent), hp(_hp), max_message_amount(_max_message_amount) {
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setFocusPolicy(Qt::NoFocus);
  setFrameStyle(QFrame::NoFrame);
  setFrameShadow(QFrame::Plain);
  setWidgetResizable(true);
  setObjectName(object_name);
  verticalScrollBar()->setStyleSheet(
    styling.css_scroll_bar.arg(styling.light_theme ? styling.css_light_sb : styling.css_dark_sb)
  );
  verticalScrollBar()->setFixedWidth(5);
  connect(verticalScrollBar(), &QScrollBar::rangeChanged, this, &Display::scroll_down);
  connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &Display::scroller);
  spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
  setStyleSheet(box_style_sheet);
  start();
}

/*! @brief Customizes the message before showing it. */
void Display::prepare_message(Message *message) {
  message->setParent(this);
  connect(message, &Message::closed, this, [this, message] { remove_message(message); });
  auto max_width = int(width() * 0.8);
  message->setWidth(max_width);
}

/*! @brief Really adds a message to the display. @sa ..._by_md and ..._with_widget. */
void Display::add_message(Message *message) {
  prepare_message(message);
  messages_mutex.lock();
  message_counter++;
  all_messages.append(message);
  vertical_box_layout->addWidget(message);
  /*! If the scroll is approximately below the middle, and the number of
   *  displayed messages is greater than the maximum...  */
  if ((verticalScrollBar()->value() >
       ((verticalScrollBar()->minimum() + verticalScrollBar()->maximum()) / 2)) and
      (message_counter > max_message_amount))
    /*! ...we delete all unnecessary messages */
    while (message_counter > max_message_amount) {
      auto *message = all_messages.takeAt(0);
      for (auto key : status_messages.keys()) 
        if (status_messages[key] == message) status_messages.remove(key);
      message->hide();
      vertical_box_layout->removeWidget(message);
      delete message;
      message_counter--;
    }
  messages_mutex.unlock();
}

/*! @brief Constructs @a Message and adds it to the display. */
void Display::add_message_by_md(MessageData message_data) {
  add_message(new Message(message_data));
}

/*! @brief Updates the text of a message by its ID, or adds a new message with that ID. */
void Display::update_status(QPair<QString, MessageData> id_and_message_data) {
  if (not status_messages.contains(id_and_message_data.first)) {
    auto *message = new Message(id_and_message_data.second);
    add_message(message);
    messages_mutex.lock();
    status_messages[id_and_message_data.first] = message;
    messages_mutex.unlock();
  } else {
    messages_mutex.lock();
    status_messages[id_and_message_data.first]->update_text(id_and_message_data.second.content);
    messages_mutex.unlock();
  }
}

/*! @brief Constructs @a Message and adds it to the display. */
void Display::add_message_with_widget(MessageData message_data, ModalHandler *handler) {
  auto *message = new Message(message_data);
  handler->getPrisoner()->setParent(message);
  message->widget(handler);
  add_message(message);
}

/*! @brief Sets the widget to its initial state.
 *  @details The method is also used to reset an existing state.  */
void Display::start() {
  messages_mutex.lock();
  message_counter = 0;
  all_messages.clear();
  status_messages.clear();
  if (vertical_box_layout) delete vertical_box_layout;
  auto *w = takeWidget();
  if (w) delete w;
  vertical_box_layout = new QVBoxLayout(this);
  vertical_box_layout->setSpacing(0);
  vertical_box_layout->setMargin(0);
  spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
  vertical_box_layout->addItem(spacer);
  QWidget *box = new QWidget(this);
  box->setContentsMargins(0, 0, 5, 0);
  box->setObjectName(box_object_name);
  box->setLayout(vertical_box_layout);
  setWidget(box);
  messages_mutex.unlock();
}

/*! @details Of the given message history, it only shows the last 50. This is because the widget
 *  is designed to only show 50 (or whatever) messages when autoscroll is enabled.  */
void Display::start_by(Messages history) {
  start();
  scroll_enabled = true;
  int m /*!< @brief Max messages to be shown now. */ = history.length() > max_message_amount ?
    max_message_amount : history.length();
  int l = history.length();
  messages_mutex.lock();
  for (int i = 0; i < l - m; i++) {
    auto *message = new Message(history[i]);
    prepare_message(message);
    message_counter++;
    all_messages.append(message);
  }
  messages_mutex.unlock();
  for (int i = l - m; i < l; i++) add_message_by_md(history[i]);
}

/*! @brief When the scrolling range of the display changes, it scrolls to the end
 *  if automatic scrolling is enabled.  */
void Display::scroll_down(int min, int max) {
  Q_UNUSED(min)
  if (scroll_enabled) verticalScrollBar()->setValue(max);
}

/*! @brief Enables or disables automatic scrolling and shows the message history when scrolling up. */
void Display::scroller(int value) {
  if (not scroll_enabled and (value == verticalScrollBar()->maximum())) {
    scroll_enabled = true;
    if (not spacer) {
      spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
      vertical_box_layout->insertItem(0, spacer);
    }
  }
  else if (scroll_enabled and (viewportSizeHint().height() > height()) and (value != verticalScrollBar()->maximum())) {
    scroll_enabled = false;
    if (spacer) {
      vertical_box_layout->removeItem(spacer);
      delete spacer;
      spacer = nullptr;
    }
  }
  auto length = hp->length();
  if ((value == verticalScrollBar()->minimum()) and (message_counter < length)) {
    /*! Add half of the maximum. */
    short portion = max_message_amount / 2;
    if (message_counter + portion > length) portion = length - message_counter;
    auto *messages = hp->messages();
    while (portion--) {
      auto *message = new Message(messages->at(messages->length() - message_counter - 1));
      all_messages.insert(0, message);
      int pos = 0;
      if (spacer) pos = 1;
      vertical_box_layout->insertWidget(pos, message);
      message->show();
      message_counter++;
    }
    verticalScrollBar()->setValue(1);
  }
}

/*! @brief Removes the message from Display. */
void Display::remove_message(Message *message) {
  messages_mutex.lock();
  message->close();
  vertical_box_layout->removeWidget(message);
  all_messages.removeOne(message);
  disconnect(message);
  message_counter--;
  messages_mutex.unlock();
}

/*! @brief When resizing, calls the @a fit_messages function again. */
void Display::resizeEvent(QResizeEvent *event) {
  fit_messages();
  QScrollArea::resizeEvent(event);
}

/*! @brief Controls the size of messages on the screen, limiting them to 80% of the display width. */
void Display::fit_messages() {
  if (not messages_mutex.try_lock()) return;
  auto max_width = int(width() * 0.8);
  for (auto *message : all_messages) message->setWidth(max_width);
  messages_mutex.unlock();
}