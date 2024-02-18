#include "mainwidget.h"
#include "./ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWidget) {
    QApplication::style()->unpolish(this);
  setWindowFlag(Qt::WindowStaysOnTopHint);
  setWindowFlag(Qt::FramelessWindowHint);
  setWindowFlag(Qt::SubWindow);
  setAttribute(Qt::WA_TranslucentBackground, true);
  board = new Whiteboard;
  board->setParent(this);
  board->mode = WBMODE_PEN;
  board->move(QPoint(0, 0));
  board->resize(size());
  board->show();
  settingsPanel = new SettingsPanel(this, &this->board->pen,
                                    &this->board->brush, (int *)&board->mode);
}

void MainWidget::resizeEvent(QResizeEvent *ev) { board->resize(this->size()); }

void MainWidget::paintEvent(QPaintEvent *ev) {
  if (state == ST_NORMAL) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 100));
    painter.end();
  } else if (state == ST_TRANSPARENT) {
  }
}

void MainWidget::setTransparent() {
  // Winapi, pretty good
  HWND hwnd = (HWND)winId();
  DWORD dwlong = GetWindowLongW(hwnd, GWL_EXSTYLE);
  SetWindowLongW(hwnd, GWL_EXSTYLE,
                 (dwlong | WS_EX_TRANSPARENT | WS_EX_LAYERED));
  state = ST_TRANSPARENT;
  board->changeMode(WBMODE_TRANSPARENT);
  // board->fl->hide();
  // board->fl->fs->hide();
  board->setTransparent();
  update();
}
void MainWidget::setUntransparent() {
  HWND hwnd = (HWND)winId();
  DWORD dwlong = GetWindowLongW(hwnd, GWL_EXSTYLE);
  SetWindowLongW(hwnd, GWL_EXSTYLE, (dwlong & ~WS_EX_TRANSPARENT));
  state = ST_NORMAL;
  board->changeMode(WBMODE_PEN);
  board->setUntransparent();
  setFocus();
  update();
}

MainWidget::~MainWidget() {
  delete ui;
  delete board;
  delete settingsPanel;
}
