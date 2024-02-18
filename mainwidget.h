#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "whiteboard.h"
#include <QPalette>
#include <QMainWindow>
#include <QWidget>
#include <windows.h>
#include "settingspanel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QMainWindow {
  Q_OBJECT

public:
  enum { ST_NORMAL, ST_TRANSPARENT } state = ST_TRANSPARENT;
  bool isTransparent = true;
  MainWidget(QWidget *parent = nullptr);
  ~MainWidget();

  Whiteboard *board;
  SettingsPanel *settingsPanel;
  void setTransparent();
  void setUntransparent();

protected:
  virtual void paintEvent(QPaintEvent *ev) override;
  virtual void resizeEvent(QResizeEvent *ev) override;

private:
  Ui::MainWidget *ui;
};
#endif // MAINWIDGET_H
