#include "expression-editor.h"

/*! 
 * @brief The constructor. 
 * @details Layout scheme:
 * <--------------->
 * {   Select db   }
 * { Select  table }
 * [Expression list]
 * [ New ] [ Close ]
 * <--------------->
 */
ExpressionEditor::ExpressionEditor(Basis *_basis, QWidget *parent, ModalHandler *m_handler)
    : QWidget(parent), basis(_basis), _m_handler(m_handler) {
  _m_handler->setPrisoner(this);
  expressions.setHeaderLabel(tr("Expressions"));
  new_expression.setText(tr("New expression"));
  close_editor.setText(tr("Close"));
  new_expression.setIcon(
    QIcon::fromTheme("list-add", QIcon(":/arts/icons/16/list-add.svg")));
  close_editor.setIcon(
    QIcon::fromTheme("dialog-ok-apply", QIcon(":/arts/icons/16/dialog-ok-apply.svg")));
  selector_layout.setSpacing(0);
  selector_layout.setMargin(0);
  selector_layout.addWidget(&databases, 0, 0, 1, 0);
  selector_layout.addWidget(&tables, 1, 0, 1, 0);
  selector_layout.addWidget(&expressions, 2, 0, 1, 0);
  selector_layout.addWidget(&new_expression, 3, 0);
  selector_layout.addWidget(&close_editor, 3, 1);
  connect(basis, &Basis::sources_changed, this, &ExpressionEditor::fill_databases);
  connect(&close_editor, &Button::clicked, this, [this]() { _m_handler->closePrisoner(); });
  auto *lt = new QGridLayout();
  lt->setSpacing(0);
  lt->setMargin(0);
  selector.setLayout(&selector_layout);
  area.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  area.setFocusPolicy(Qt::NoFocus);
  area.setFrameStyle(QFrame::NoFrame);
  area.setFrameShadow(QFrame::Plain);
  area.setWidgetResizable(true);
  area.setWidget(&selector);
  lt->addWidget(&area);
  setLayout(lt);
  setFixedWidth(480);
  fill_databases();
}

/*! @brief TBD */
void ExpressionEditor::fill_databases() {
  auto selected = databases.currentData();
  auto sources = basis->sources();
  for (auto source : sources) {
    if (databases.findData(source.path) == -1)
      databases.addItem(
        styling.metrics->elidedText(source.path, Qt::ElideLeft, width() - 30), source.path);
  }
  if (selected.isValid()) {
    int id = -1;
    if ((id = databases.findData(selected)) != -1)
      databases.setCurrentIndex(id);
  }
  fill_tables(sources);
}

/*! @brief TBD */
void ExpressionEditor::fill_tables(const Sources &sources) {
  auto selected = tables.currentData();
  for (auto source : sources) {
    if (tables.findData(source.table_name) == -1)
      tables.addItem(
        styling.metrics->elidedText(source.table_title, Qt::ElideLeft, width() - 30), source.table_name);
  }
  if (selected.isValid()) {
    int id = -1;
    if ((id = tables.findData(selected)) != -1)
      tables.setCurrentIndex(id);
  }
  fill_expressions(sources);
}

/*! @brief TBD */
void ExpressionEditor::fill_expressions(const Sources &sources) {
  
}
