#include "brief.hpp"

/*! @brief The constructor. */
ExtensionsViewerBrief::ExtensionsViewerBrief(ExtensionsManager *_em, QWidget *parent) : QWidget(parent), em(_em) {
  connect(&back_btn, &Button::clicked, this, [this] { extension_meta = nullptr; emit close_brief(); });
  connect(&remove_btn, &Button::clicked, this, &ExtensionsViewerBrief::remove_extension);
  remove_btn.setText(tr("Remove this extension from Jeff"));
  back_btn.setIcon(QIcon::fromTheme("go-previous", QIcon(":/arts/icons/16/go-previous.svg")));
  remove_btn.setIcon(QIcon::fromTheme("edit-delete", QIcon(":/arts/icons/16/edit-delete.svg")));
  desc_lbl.setTextFormat(Qt::RichText);
  desc_lbl.setWordWrap(true);
  authors_lbl.setTextFormat(Qt::RichText);
  license_lbl.setTextFormat(Qt::RichText);
  links_lbl.setTextFormat(Qt::RichText);
  status_lbl.setTextFormat(Qt::RichText);
  status_lbl.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  auto *spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
  setLayout(
    VLineLt::another()
      ->addlt(HLineLt::another()->addw(&back_btn)->addw(&name_lbl)->addi(spacer))
      ->addw(&desc_lbl)->addw(&authors_lbl)->addw(&license_lbl)->addw(&links_lbl)
      ->addlt(HLineLt::another()->addw(&status_lbl)->addw(&on_off_btn))
      ->addw(&remove_btn)
  );
}

/*! @brief Sets up extension details. */
void ExtensionsViewerBrief::setup(ExtensionMeta *_extension_meta) {
  extension_meta = _extension_meta;
  name_lbl.setText("<h1>" + extension_meta->name + "</h1>");
  desc_lbl.setText("<b>" + tr("Description:") + "</b> " + extension_meta->desc);
  if (extension_meta->authors.keys().isEmpty()) {
    if (authors_lbl.isVisible()) authors_lbl.setVisible(false);
  } else {
    if (not authors_lbl.isVisible()) authors_lbl.setVisible(true);
    QString authors = "<b>" + tr("Authors:") + "</b>";
    for (auto author : extension_meta->authors.keys()) {
      authors += "<br>· <i>" + author + "</i>";
      if (not extension_meta->authors[author].keys().isEmpty())
        for (auto contact_type : extension_meta->authors[author].keys()) {
          auto contact_content = extension_meta->authors[author][contact_type];
          contact_type[0] = contact_type[0].toUpper();
          authors += "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- " + contact_type + ": " + contact_content;
        }
    }
    authors_lbl.setText(authors);
  }
  license_lbl.setText("<b>" + tr("License:") + "</b> " + extension_meta->license);
  if (extension_meta->links.isEmpty()) {
    if (links_lbl.isVisible()) links_lbl.setVisible(false);
  } else {
    if (not links_lbl.isVisible()) links_lbl.setVisible(true);
    QString links = "<b>" + tr("Links:") + "</b>";
    for (auto link : extension_meta->links) links += "<br><a href=\"" + link + "\">" + link + "</a>";
    links_lbl.setText(links);
  }
  status_lbl.setText("<i>" + tr("Waiting status...") + "</i>");
  on_off_btn.setText(tr("Control"));
  on_off_btn.setEnabled(false);
  update_status();
}

/*! @brief Displays every second whether an extension is running or not. */
void ExtensionsViewerBrief::update_status() {
  if (not extension_meta) return;
  if (isVisible()) {
    if (not on_off_btn.isEnabled()) {
      on_off_btn.setEnabled(true);
    }
    if (em->is_running(extension_meta)) {
      disconnect(&on_off_btn, &Button::clicked, nullptr, nullptr);
      on_off_btn.setText(tr("Disable the extension"));
      on_off_btn.setIcon(QIcon::fromTheme("media-playback-stop", QIcon(":/arts/icons/16/media-playback-stop.svg")));
      connect(&on_off_btn, &Button::clicked, this, [this] {
        em->stop_extension(extension_meta);
        extension_meta->enabled = false;
      });
      status_lbl.setText("<p style=\"color:#3b961a\">" + tr("Running") + "</p>");
    }
    else {
      disconnect(&on_off_btn, &Button::clicked, nullptr, nullptr);
      on_off_btn.setText(tr("Enable the extension"));
      on_off_btn.setIcon(QIcon::fromTheme("media-playback-start", QIcon(":/arts/icons/16/media-playback-start.svg")));
      connect(&on_off_btn, &Button::clicked, this, [this] {
        em->start_extension(extension_meta);
        extension_meta->enabled = true;
      });
      status_lbl.setText("<p style=\"color:#d22a2a\">" + tr("Stopped") + "</p>");
    }
  }
  QTimer::singleShot(1000, this, &ExtensionsViewerBrief::update_status);
}

/*! @brief Removes an extension from Jeff. */
void ExtensionsViewerBrief::remove_extension() {
  em->remove_extension(extension_meta);
  emit close_brief();
}
