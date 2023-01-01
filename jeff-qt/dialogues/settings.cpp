#include "settings.h"

/*! @brief The constructor. */
Settings::Settings(Basis *_basis, QWidget *parent, ModalHandler *m_handler)
    : QWidget(parent), basis(_basis), _m_handler(m_handler) {
  _m_handler->setPrisoner(this);
  setObjectName(object_name);
  mainLt.setMargin(0);
  // Delay.
  delay.setText(tr("Enable delay"));
  connect(&delay, &QCheckBox::toggled, this, &Settings::delayChecked);
  auto *delayExplanation = new ExplanationLabel(tr("Enables Jeff's response delay."), this);
  auto *minDelayText = new QLabel(tr("From"), this);
  minDelayText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  minDelay.setMaximum(5000);
  minDelay.setSingleStep(10);
  connect(&minDelay, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &Settings::minDelayValueChanged);
  auto *maxDelayText = new QLabel(tr("to"), this);
  maxDelayText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  maxDelay.setMaximum(5000);
  maxDelay.setSingleStep(10);
  connect(&maxDelay, QOverload<int>::of(&QSpinBox::valueChanged), this,
          &Settings::maxDelayValueChanged);
  auto *msText = new QLabel(tr("ms"), this);
  msText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  auto *delayBoxLayout = new QHBoxLayout();
  delayBoxLayout->addWidget(minDelayText);
  delayBoxLayout->addWidget(&minDelay);
  delayBoxLayout->addWidget(maxDelayText);
  delayBoxLayout->addWidget(&maxDelay);
  delayBoxLayout->addWidget(msText);
  delayBox.setLayout(delayBoxLayout);
  // History keeping.
  keepHistory.setText(tr("Keep history"));
  auto *keepHistoryExplanation = new ExplanationLabel(tr(
    "Enables keeping the message history after exiting the application."), this
  );
  // Greetings.
  greetings.setText(tr("Enable greetings message on start"));
  connect(&greetings, &QCheckBox::toggled, this, &Settings::greetingsChecked);
  auto *greetingsExplanation = new ExplanationLabel(
    tr("Allows you to automatically send a message to Jeff at startup so that he prepares the "
    "information you need in advance for you."), this
  );
  // Save button & layout setting.
  save_and_close.setText(tr("OK"));
  save_and_close.setIcon(
    QIcon::fromTheme("dialog-ok-apply", QIcon(":/arts/icons/16/dialog-ok-apply.svg")));
  connect(&save_and_close, &Button::clicked, this, &Settings::saveAndClose);
  mainLt.addWidget(&delay);
  mainLt.addWidget(delayExplanation);
  mainLt.addWidget(&delayBox);
  mainLt.addWidget(&keepHistory);
  mainLt.addWidget(keepHistoryExplanation);
  mainLt.addWidget(&greetings);
  mainLt.addWidget(greetingsExplanation);
  mainLt.addWidget(&greetingsMsg);
  mainLt.addWidget(&save_and_close);
  setLayout(&mainLt);
  loadStates();
}

/*! @brief Loads settings from file. */
void Settings::loadStates() {
  delay.setChecked(basis->read(basis->isDelayEnabledSt).toBool());
  delayChecked();
  keepHistory.setChecked(basis->read(basis->isHistoryKeepingEnabledSt).toBool());
  minDelay.setValue(basis->read(basis->minDelaySt).toInt());
  minDelayValueChanged(minDelay.value());
  maxDelay.setValue(basis->read(basis->maxDelaySt).toInt());
  maxDelayValueChanged(maxDelay.value());
  greetings.setChecked(basis->read(basis->isGreetingsEnabledSt).toBool());
  greetingsChecked();
  greetingsMsg.setText(basis->read(basis->greetingsMsg).toString());
}

/*! @brief Saves Jeff's settings. */
void Settings::saveAndClose() {
  basis->write(basis->isDelayEnabledSt, delay.isChecked());
  basis->write(basis->isHistoryKeepingEnabledSt, keepHistory.isChecked());
  basis->write(basis->minDelaySt, minDelay.value());
  basis->write(basis->maxDelaySt, maxDelay.value());
  basis->write(basis->isGreetingsEnabledSt, greetings.isChecked());
  basis->write(basis->greetingsMsg, greetingsMsg.text());
  _m_handler->closePrisoner();
}

/*! @brief Depending on whether the delay is enabled, turns on/off the widget
 *  with a choice of delay boundaries.  */
void Settings::delayChecked() { delayBox.setEnabled(delay.isChecked()); }
/*! @brief When the startup greeting is enabled, it enables the ability to edit it. */
void Settings::greetingsChecked() { greetingsMsg.setEnabled(greetings.isChecked()); }
/*! @brief Changes maxDelay value, if minDelay value is bigger than that. */
void Settings::minDelayValueChanged(int value) { 
  if (minDelay.value() > value) maxDelay.setValue(value); }
/*! @brief Changes minDelay value, if maxDelay value is smaller than that. */
void Settings::maxDelayValueChanged(int value) {
  if (maxDelay.value() < value) minDelay.setValue(value); }