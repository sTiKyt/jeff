#ifndef MENUBAR_H
#define MENUBAR_H

#include "core-kit/basis.h"
#include "widgets/line.h"
#include "widgets/menu.h"
#include <QAction>
#include <QApplication>
#include <QJsonArray>
#include <QMenu>
#include <QMenuBar>
#include <QWidget>

/*! @class MenuBar
 *  @brief Provides the ability to interact with Jeff using the menubar.  */
class MenuBar : public QMenuBar {
  Q_OBJECT
  Q_DISABLE_COPY(MenuBar)
public:
  // Objects:
  QAction full_screen_action;
  QAction enable_monologue_mode;

  // Functions described in `menubar.cpp`:
  MenuBar(Basis *_basis, Line *line, QWidget *parent = nullptr);

signals:
  /*! @brief Informs about clicking on the "Clear history" action. */
  void clear_history_triggered();
  /*! @brief Informs about clicking on the "About" action. */
  void about_triggered();
  /*! @brief Informs about clicking on the "Settings" action. */
  void settings_triggered();
  /*! @brief Informs about clicking on the "Source manager" action. */
  void sources_triggered();
  /*! @brief Informs about clicking on the "Expression editor" action. */
  void phrase_editor_triggered();
  /*! @brief Informs about clicking on the "Export message history..." action. */
  void export_triggered();
  /*! @brief Informs about clicking on the "Import message history..." action. */
  void import_triggered();
  /*! @brief Informs about clicking on the "&Exit" action. */
  void exit_triggered();
  /*! @brief Informs about clicking on the "Help" action [constructing] */
  // void help_triggered();

private:
  // Objects:
  Basis *basis = nullptr;
  Menu file_menu, edit_menu, tools_menu, help_menu;
  QAction 
    /*! `File`  menu. */
    source_manager_action, script_manager_action, phrase_editor_action, export_history_action,
    import_history_action, exit_action,
    /*! `Edit`  menu. */
    clear_history_action, delete_text_action, cut_text_action, copy_text_action, paste_text_action,
    select_all_text_action,
    /*! `Tools` menu. */
    hide_menubar_action, settings_action,
    /*! `Help`  menu. */
    novice_guide, database_guide, aiml_guide, scripts_guide, server_script_guide, about_action;
};

#endif