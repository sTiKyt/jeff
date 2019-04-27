#include "a_menubar.h"

AMenuBar::AMenuBar(ALine *l, QWidget *p) : QMenuBar(p) {
  // Sub-menu bar in four stages:
  //    1) creating items;
  //    2) assigning of key sequences;
  //    3) setting icons
  //    4) adding items;
  //    5) connecting of signals and slots.
  // "File" menu group...
  mf = this->addMenu("File");
  QAction *cm = new QAction("Container manager", mf);
  // QAction *sFile = new QAction("Save to file", menuFile);
  cm->setShortcut(Qt::CTRL + Qt::Key_M);
  // sFile->setShortcuts(QKeySequence::Save);
  cm->setIcon(QIcon(":/arts/icons/16/database-manager.svg"));
  mf->addAction(cm);
  // menuFile->addAction(sFile);
  mf->addSeparator();
  mf->addAction(QIcon(":/arts/icons/16/application-exit.svg"), "&Exit",
                &QApplication::quit, Qt::ALT + Qt::Key_F4);
  connect(cm, &QAction::triggered, this, &AMenuBar::openContainerManager);
  // connect(sFile, &QAction::triggered, this, &AkiwakeMenuBar::saveToFile);
  // "Edit" menu group...
  me = this->addMenu("Edit");
  QAction *cmh = new QAction("Clear message history", me);
  QAction *del = new QAction("Delete", me);
  QAction *cut = new QAction("Cut", me);
  QAction *copy = new QAction("Copy", me);
  QAction *paste = new QAction("Paste", me);
  QAction *sel = new QAction("Select all", me);
  cmh->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_D);
  del->setShortcut(Qt::Key_Backspace);
  cut->setShortcuts(QKeySequence::Cut);
  copy->setShortcuts(QKeySequence::Copy);
  paste->setShortcuts(QKeySequence::Paste);
  sel->setShortcuts(QKeySequence::SelectAll);
  cmh->setIcon(QIcon(":/arts/icons/16/clear-history.svg"));
  del->setIcon(QIcon(":/arts/icons/16/clear.svg"));
  cut->setIcon(QIcon(":/arts/icons/16/cut.svg"));
  copy->setIcon(QIcon(":/arts/icons/16/copy.svg"));
  paste->setIcon(QIcon(":/arts/icons/16/paste.svg"));
  sel->setIcon(QIcon(":/arts/icons/16/select-all.svg"));
  me->addAction(cmh);
  me->addSeparator();
  me->addAction(del);
  me->addAction(cut);
  me->addAction(copy);
  me->addAction(paste);
  me->addSeparator();
  me->addAction(sel);
  connect(cmh, &QAction::triggered, this, &AMenuBar::clearMsgHist);
  connect(del, &QAction::triggered, l->tl, &ALineEdit::backspace);
  connect(cut, &QAction::triggered, l->tl, &ALineEdit::cut);
  connect(copy, &QAction::triggered, l->tl, &ALineEdit::copy);
  connect(paste, &QAction::triggered, l->tl, &ALineEdit::paste);
  connect(sel, &QAction::triggered, l->tl, &ALineEdit::selectAll);
  // "Tools" menu group...
  mt = addMenu("Tools");
  QAction *hb = new QAction("Hide menu bar", mt);
  {
    fsa = new QAction("Full screen", mt);
    fsa->setCheckable(true);
  }
  hb->setShortcut(Qt::CTRL + Qt::Key_H);
  fsa->setShortcut(Qt::Key_F11);
  hb->setIcon(QIcon(":/arts/icons/16/show-menu.svg"));
  fsa->setIcon(QIcon(":/arts/icons/16/view-fullscreen.svg"));
  mt->addAction(hb);
  mt->addAction(fsa);
  connect(hb, &QAction::triggered, this, &AMenuBar::hideThis);
  connect(fsa, &QAction::triggered, this, &AMenuBar::fScreen);
  // "Help" menu group...
  mh = addMenu("Help");
  QAction *about = new QAction("About", mh);
  about->setIcon(QIcon(":/arts/icons/16/help-about.svg"));
  mh->addAction(about);
  mh->addAction(QIcon(":/arts/icons/16/qt.svg"), "About Qt",
                &QApplication::aboutQt);
  connect(about, &QAction::triggered, this, &AMenuBar::openAbout);
}

AMenuBar::~AMenuBar() {
  // Prevents memory leaks.
  delete mf;
  delete me;
  delete mt;
  delete mh;
}

void AMenuBar::openContainerManager() { emit contManTriggered(); }

void AMenuBar::openAbout() { emit aboutTriggered(); }

void AMenuBar::saveToFile() { emit saveToFileTriggered(); }

void AMenuBar::clearMsgHist() { emit clearScreenTriggered(); }

void AMenuBar::fScreen() { emit fullscreenModeChanged(); }

void AMenuBar::hideThis() { setVisible(!isVisible()); }
