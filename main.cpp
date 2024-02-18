#include "QHotkey"
#include "mainwidget.h"
#include "floatingwindow.h"

#include <QApplication>
#include <QShortcut>
#include <QStyleFactory>
#include <windows.h>

int main(int argc, char *argv[]) {
  qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
  QApplication a(argc, argv);
  a.setStyle(QStyleFactory::create("fusion"));
  MainWidget w(nullptr);
  w.showMaximized();
  w.setTransparent();
  QHotkey hk_st(QKeySequence("Ctrl+Shift+S"), true, &a);
  QObject::connect(&hk_st, &QHotkey::activated, &a, [&w] {
    qDebug() << "Ctrl+Shift+S receive";
    if (w.state == MainWidget::ST_TRANSPARENT) {
        w.setUntransparent();
    } else {
      w.setTransparent();
    }
  });
  QHotkey hk_exit(QKeySequence("Ctrl+Shift+X"), true, &a);
  QObject::connect(&hk_exit, &QHotkey::activated, &a, [&w, &a] {
    qDebug() << "Ctrl+Shift+X receive";
    a.exit();
  });
  QShortcut hk_settingspanel(QKeySequence("Ctrl+S"), &w);
  QObject::connect(&hk_settingspanel, &QShortcut::activated, &a, [&w] {
    qDebug() << "Ctrl+S receive";
    w.settingsPanel->exec();
  });
  QShortcut hk_undo(QKeySequence("Ctrl+Z"), &w);
  QObject::connect(&hk_undo, &QShortcut::activated, &a, [&w] {
    qDebug() << "Ctrl+Z receive";
    w.board->undo();
  });
  QShortcut hk_redo(QKeySequence("Ctrl+R"), &w);
  QObject::connect(&hk_redo, &QShortcut::activated, &a, [&w] {
    qDebug() << "Ctrl+R receive";
    w.board->redo();
  });
  QShortcut hk_update(QKeySequence("Ctrl+Shift+A"), &w);
  QObject::connect(&hk_update, &QShortcut::activated, &a, [&w] {
    qDebug() << "Ctrl+Shift+A receive";
    w.board->update();
  });
  return a.exec();
}
