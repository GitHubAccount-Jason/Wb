#include "floatingwindow.h"
#include "ui_floatingwindow.h"
#include <QMouseEvent>
FloatingWindow::FloatingWindow(Whiteboard *parent)
    : QWidget(parent), wb(parent), ui(new Ui::FloatingWindow),
      fs(new FloatingSetting(parent, parent)) {

  ui->setupUi(this);
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground, true);
  setWindowFlag(Qt::SubWindow);
  ui->pushButton->setAttribute(Qt::WA_TransparentForMouseEvents);

  fs->hide();
}

FloatingWindow::~FloatingWindow() { delete ui; }
static QPoint lastPoint;
void FloatingWindow::mousePressEvent(QMouseEvent *ev) {
  if (ev->button() == Qt::LeftButton) {
    this->raise(); // 将此按钮移动到顶层显示
    lastPoint = ev->pos();
  }
}
void FloatingWindow::mouseMoveEvent(QMouseEvent *ev) {
  if (ev->buttons() == Qt::LeftButton) {
    if (parent() == wb) {
      int ax, ay;
      QRect g = this->geometry();
      QRect next = g.translated(ev->pos() - lastPoint);
      QRect av = wb->geometry();
      if (next.x() < 0) {
        ax = 0;
      } else if (next.topRight().x() > av.width()) {
        ax = av.width()-g.width();
      } else {
        ax = next.x();
      }
      if (next.y() < 0) {
        ay = 0;
      } else if (next.bottomLeft().y() > av.height()) {
        ay = av.height()-g.height();
      } else {
          ay = next.y();
      }
      this->move(ax, ay);
      if (!fs->isHidden()) {
        fs->move(ax + 128, ay);
      }
    } else if (parent() == nullptr) {
      int ax, ay;
      QRect g = this->geometry();
      QRect next = g.translated(ev->pos() - lastPoint);
      QRect av = QApplication::screens().at(0)->availableGeometry();
      if (next.x() < 0) {
        ax = 0;
      } else if (next.topRight().x() > av.width()) {
        ax = av.width()-g.width();
      } else {
        ax = next.x();
      }
      if (next.y() < 0) {
        ay = 0;
      } else if (next.bottomLeft().y() > av.height()) {
        ay = av.height()-g.height();
      } else {
          ay = next.y();
      }
      this->move(ax, ay);
      if (!fs->isHidden()) {
        fs->move(ax + 128, ay);
      }
    }
  }
}
void FloatingWindow::mouseReleaseEvent(QMouseEvent *ev) {
  if (ev->pos() == lastPoint) {
    fs->move(QPoint(pos().x() + 129, pos().y()));
    if (fs->isHidden()) {
      fs->show();
    } else
      fs->hide();
  }
  if (ev->button() & Qt::RightButton) {
    switch (wb->mainwidget()->state) {
    case MainWidget::ST_TRANSPARENT:
      wb->mainwidget()->setUntransparent();
        break;
    case MainWidget::ST_NORMAL:
      wb->mainwidget()->setTransparent();
        break;
    }
  }
}
