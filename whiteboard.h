#ifndef WHITEBOARD_H
#define WHITEBOARD_H

#include "wbdefs.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineF>
#include <QMouseEvent>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#define mid(a, b) (QPointF((a.x() + b.x()) / 2, (a.y() + b.y()) / 2))

template <class A> A min(const A &a, const A &b) { return std::min(a, b); }
template <class A> A min(const A &a, const A &b, const A &L...) {
  return min(min(a, b), L);
}
template <class A> A max(const A &a, const A &b) { return std::max(a, b); }
template <class A> A max(const A &a, const A &b, const A &L...) {
  return max(max(a, b), L);
}
inline void drawPatchPoint(QPainter *painter, const QLineF &path,const QPointF&beginPoint,  const QPen&pen) {
  qreal temp = (pen.widthF() - 3) / 100.;
  int k = 0;
    QPainterPath p;
  p.moveTo(beginPoint);
    p.quadTo(path.p1(), path.p2());
  for (double i = 0; i <path.length(); i += 1) {
    k++;

      painter->setPen(QPen(pen.color(), pen.widthF() - temp * (k/path.length()*100), pen.style(),
                         pen.capStyle(), pen.joinStyle()));
    painter->drawPoint(p.pointAtPercent(i/path.length()));
  }
}
inline double disPoints(const QPointF &p1, const QPointF &p2) {
  return sqrt(pow(p1.x() - p2.x(), 2) + pow(p1.y() - p2.y(), 2));
}
inline double disCircles(const Circle &c1, const Circle &c2) {
  return disPoints(c1.o, c2.o) - c1.r - c2.r;
}
inline double disPointCircle(const QPointF &p1, const Circle &c1) {
  return fabs(disPoints(p1, c1.o) - c1.r);
}
inline double disPointLine(const QPointF &p1, const QLineF &l1) {
  double A = l1.y2() - l1.y1();
  double B = l1.x1() - l1.x2();
  double C = l1.x2() * l1.y1() - l1.x1() * l1.y2();
  return fabs(A * p1.x() + B * p1.y() + C) / sqrt(A * A + B * B);
}
inline double disLineCircle(const QLineF &l, const Circle &c) {
  return disPointLine(c.o, l) - c.r;
}
inline QPointF mapToRect(const QPointF &p, const QRect &r, qreal w) {
  return QPointF(p.x() - r.left() + w, p.y() - r.top() + w);
}
inline QLineF mapToRect(const QLineF &l, const QRect &r, qreal w) {
  return QLineF(mapToRect(l.p1(), r, w), mapToRect(l.p2(), r, w));
}
inline QPointF mapToPoint(const QPointF &p, const QPointF &ori, qreal w) {
  return QPointF(p.x() - ori.x() + w, p.y() - ori.y() + w);
}
inline QLineF mapToPoint(const QLineF &l, const QPointF &ori, qreal w) {
  return QLineF(mapToPoint(l.p1(), ori, w), mapToPoint(l.p2(), ori, w));
}
class FloatingWindow;
inline double widthLevel(double dis) {
  if (dis > 300) {
    return 0.3;
  }
  if (dis > 250) {
    return 0.4;
  }
  if (dis > 200) {
    return 0.5;
  }
  if (dis > 170) {
    return 0.6;
  }
  if (dis > 150) {
    return 0.7;
  }
  if (dis > 120) {
    return 0.8;
  }
  if (dis > 100) {
    return 0.9;
  }
  if (dis > 80) {
    return 0.95;
  }
  return 1;
}

class Whiteboard : public QWidget {
public:
  int mode;

  class WbScene {
    QPixmap pm;
    QPoint p;
    QPainterPath extra;
    int tp;

  public:
    WbScene() : tp(WBOBJTYPE_NONE) {}
    //
    WbScene(const QPainterPath &l, const QLineF&lastline, const QPointF&beginPoint, QPen pen, const QBrush &brush, const QRectF&point
            )
        : pm((pen.width() * 2 + point.width()),
             (pen.width() * 2 + point.height())),
          p(QPoint(point.x() - pen.width(), point.y() - pen.width())),
          tp(WBOBJTYPE_LINELIST) {
      extra = l;
      pm.fill(Qt::transparent);
      QPainter painter;
      painter.begin(&pm);
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
      painter.setPen(pen);
      painter.setBrush(brush);
      painter.drawPath(extra);
      drawPatchPoint(&painter, lastline,beginPoint, pen);
      painter.end();
      // qDebug()<<p;
    }
    void paint(QPainter &p) { p.drawPixmap(this->p, pm); }
    // 判断鼠标轨迹是否经过对象
    bool isInteract(const QPoint &pa, const QPoint &pb) {
      return extra.intersects(QRectF(pa, pb));
    }
  };
  class WbTmpScene {
  public:
    QPen *pen;
    QBrush *brush;
    QPainterPath path;
    QLineF lastline;
    QPointF beginPoint;
    bool isDrawPatchPoint=false;
    void paint(QWidget &w) {
        // qDebug()<<__PRETTY_FUNCTION__<<"\n";
      QPainter painter(&w);
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
      if (pen!=nullptr){
      painter.setPen(*pen);
      }
      if (brush!=nullptr){
      painter.setBrush(*brush);
      }
      painter.drawPath(path);
      if (pen!=nullptr&&isDrawPatchPoint){
          // qDebug()<<last;
          drawPatchPoint(&painter, lastline,beginPoint, *pen);
      }
      painter.end();
    }
  };

  QPen pen;
  QBrush brush;
  FloatingWindow *fl;
  Whiteboard();
  ~Whiteboard();
  bool undo();
  bool redo();
  void changeMode(int mode);
  void setTransparent();
  void setUntransparent();

protected:
  virtual void paintEvent(QPaintEvent *ev) override;
  virtual void mousePressEvent(QMouseEvent *ev) override;
  virtual void mouseMoveEvent(QMouseEvent *ev) override;
  virtual void mouseReleaseEvent(QMouseEvent *ev) override;

private:
  WbTmpScene tmpScene;
  QList<WbScene> opSc;
  QList<QPointF> mouseMoveTrace;
  qsizetype nextScenePos;

  int l = INT_MAX, r = INT_MIN, u = INT_MAX, d = INT_MIN;
  // If I press on the button and move the mouse, than ignore this move or
  // release
  bool isPressTriggered = false;
  double traceLen = 0;
  double traceSpeed = 0;
};

#endif // WHITEBOARD_H
