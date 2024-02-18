#ifndef SETTINGSPANEL_H
#define SETTINGSPANEL_H

#include <QBrush>
#include <QColorDialog>
#include <QPushButton>
#include <QComboBox>
#include <QDialog>
#include <QPen>
#include "wbdefs.h"

namespace Ui {
class SettingsPanel;
}

class SettingsPanel : public QDialog {
  Q_OBJECT

public:
  explicit SettingsPanel(QWidget *parent, QPen *pen, QBrush *brush, int* mode);
  ~SettingsPanel();
  virtual void showEvent(QShowEvent* ev) override;

private:
  Ui::SettingsPanel *ui;
  QPen *penPen;
  QBrush *penBrush;
  int* penMode;
};

#endif // SETTINGSPANEL_H
