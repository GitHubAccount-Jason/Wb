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
class FloatingWindow;
class Whiteboard : public QWidget {
public:
  int mode;
  WbConfig gConfig;

  class WbScene {
    QPixmap pm;
    QPoint p;
    QPainterPath extra;
    int tp;

  public:
    WbScene() : tp(WBOBJTYPE_NONE) {}
    //
    WbScene(const QPainterPath &l, const QLineF &lastline,
            const QPointF &beginPoint, QPen pen, const QBrush &brush,
            const QRectF &point)
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
      if (lastline.length() >= 5) {
        drawPatchPoint(&painter, lastline, beginPoint, pen);
      }
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
    bool isDrawPatchPoint = false;
    void paint(QWidget &w) {
      // qDebug()<<__PRETTY_FUNCTION__<<"\n";
      QPainter painter(&w);
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
      if (pen != nullptr) {
        painter.setPen(*pen);
      }
      if (brush != nullptr) {
        painter.setBrush(*brush);
      }
      painter.drawPath(path);
      if (pen != nullptr && isDrawPatchPoint && lastline.length() >= 5) {
        // qDebug()<<last;
        drawPatchPoint(&painter, lastline, beginPoint, *pen);
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

  struct EvInfo {
    int l;
    int r;
    int u;
    int d;
    QList<QPointF> mouseMoveTrace;
    double traceLen;
    double traceSpeed;

  private:
    bool isPressTriggered_;

  public:
    void setPressTriggered() { isPressTriggered_ = true; }
    bool isPressTriggered() const { return isPressTriggered_; }
    EvInfo() { clear(); }
    bool refresh(const QMouseEvent *ev) {
      if (isPressTriggered_ == false) {
        return false;
      }

      traceLen += disPoints(mouseMoveTrace.back(), ev->position());
      traceSpeed = traceLen / (mouseMoveTrace.size() - 1);
      l = min(l, ev->pos().x());
      r = max(r, ev->pos().x());
      u = min(u, ev->pos().y());
      d = max(d, ev->pos().y());
      mouseMoveTrace.push_back(ev->position());
      return true;
    }
    bool init(QMouseEvent *pressEv) {
      clear();
      if (pressEv->button() != Qt::LeftButton) {
        return false;
      }
      l = min(l, pressEv->pos().x());
      r = max(r, pressEv->pos().x());
      u = min(u, pressEv->pos().y());
      d = max(d, pressEv->pos().y());
      mouseMoveTrace.push_back(pressEv->position());
      isPressTriggered_ = true;
      return true;
    }
    void clear() {
      l = INT_MAX;
      r = INT_MIN;
      u = INT_MAX;
      d = INT_MIN;
      mouseMoveTrace.clear();
      traceLen = 0;
      traceSpeed = 0;
      isPressTriggered_ = false;
    }
  };
  EvInfo evinfo;
protected:
  virtual void paintEvent(QPaintEvent *ev) override;
  virtual void mousePressEvent(QMouseEvent *ev) override;
  virtual void mouseMoveEvent(QMouseEvent *ev) override;
  virtual void mouseReleaseEvent(QMouseEvent *ev) override;

private:
  qsizetype nextScenePos;

  QList<WbControl*> controls;
  QPixmap pm;

};

#endif // WHITEBOARD_H
