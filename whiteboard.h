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

  class WbObject {
  public:
    int type;

    QPen pen;
    QBrush brush;
    void *extra;
    WbObject() : type(WBOBJTYPE_EXTRA), extra(nullptr) {}
    WbObject(const WbObject &wo) {
      pen = wo.pen;
      brush = wo.brush;
      type = wo.type;
      switch (wo.type) {
      case WBOBJTYPE_LINE:
        extra = new Line(*(Line *)wo.extra);
        break;
      case WBOBJTYPE_LINELIST:
        extra = new LineList(*(LineList *)wo.extra);
      default:
        break;
      };
    }
    WbObject(const Line &l, const QPen &pen, const QBrush &brush)
        : type(WBOBJTYPE_LINE), pen(pen), brush(brush), extra(new Line(l)) {}
    WbObject(const LineList &l, const QPen &pen, const QBrush &brush)
        : type(WBOBJTYPE_LINELIST), pen(pen), brush(brush),
          extra(new LineList(l)) {}
    WbObject(const Circle &c, const QPen &pen, const QBrush &brush)
        : type(WBOBJTYPE_LINELIST), pen(pen), brush(brush),
          extra(new Circle(c)) {}
    WbObject &operator=(const WbObject &wo) {
      pen = wo.pen;
      brush = wo.brush;
      type = wo.type;
      switch (wo.type) {
      case WBOBJTYPE_LINE:
        extra = new Line(*(Line *)wo.extra);
        break;
      case WBOBJTYPE_LINELIST:
        extra = new LineList(*(LineList *)wo.extra);
        break;
      case WBOBJTYPE_CIRCLE:
        extra = new Circle(*(Circle *)wo.extra);
        break;
      default:
        break;
      }
      return *this;
    }
    Line &line() { return *(Line *)extra; }
    const Line &line() const { return *(Line *)extra; }
    LineList &linelist() { return *(LineList *)extra; }
    const LineList &linelist() const { return *(LineList *)extra; }
    Circle &circle() { return *(Circle *)extra; }
    const Circle &circle() const { return *(Circle *)extra; }
    ~WbObject() {
      if (extra != nullptr) {
        switch (type) {
        case WBOBJTYPE_LINE:
          delete (Line *)extra;
          break;
        case WBOBJTYPE_LINELIST:
          delete (LineList *)extra;
          break;
        case WBOBJTYPE_CIRCLE:
          delete (Circle *)extra;
          break;
        default:
          break;
        }
      }
    }
  };
  class WbScene {
    QPixmap pm;
    QPoint p;
    bool isValid = true;
    std::variant<LineList, Circle> extra;
    int tp;

  public:
    WbScene() : tp(WBOBJTYPE_NONE) {}
    WbScene(const LineList &l, QPen pen, const QBrush &brush, const QRect &rect)
        : pm((pen.width() * 2 + rect.width()),
             (pen.width() * 2 + rect.height())),
          p(QPoint(rect.left() - pen.width(), rect.top() - pen.width())),
          tp(WBOBJTYPE_LINELIST) {
      extra = l;
      pm.fill(Qt::transparent);
      QPainter painter;
      painter.begin(&pm);
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
      painter.setPen(pen);
      painter.setBrush(brush);
      if (l.size() == 1) {
        painter.drawLine(
            Line(QPointF(l.front().p1().x() - rect.left() + pen.width(),
                         l.front().p1().y() - rect.top() + pen.width()),
                 QPointF(l.front().p2().x() - rect.left() + pen.width(),
                         l.front().p2().y() - rect.top() + pen.width())));
      } else {
        for (int i = 0; i < l.size() - 1; ++i) {
          // pen.setWidthF(widthLevel(disPoints(l[i].p1(), l[i].p2())) *
          //              pen.widthF());
          painter.setPen(pen);
          painter.drawLine(
              Line(QPointF(l[i].p1().x() - rect.left() + pen.width(),
                           l[i].p1().y() - rect.top() + pen.width()),
                   QPointF(l[i].p2().x() - rect.left() + pen.width(),
                           l[i].p2().y() - rect.top() + pen.width())));
        }
        if (l.size() > 1) {
          if (l.back().length() < 3) {
            painter.drawLine(mapToRect(l.back(), rect, pen.widthF()));
          } else {
            auto w = pen.widthF();
            pen.setWidthF(w / 2);
            painter.setPen(pen);
            auto &line = l.back();
            double A = line.y2() - line.y1(), B = line.x1() - line.x2();
            QPointF p1(line.x1() - w * A / (2 * (sqrt(A * A + B * B))) -
                           rect.left() + w,
                       line.y1() + w * B / (2 * (sqrt(A * A + B * B))) -
                           rect.top() + w);
            QPointF p2(line.x2() + w * A / (2 * (sqrt(A * A + B * B))) -
                           rect.left() + w,
                       line.y2() - w * B / (2 * (sqrt(A * A + B * B))) -
                           rect.top() + w);
            QPointF p3(5 * (line.p2().x() - line.p1().x()) + line.p1().x(),
                       5 * (line.p2().y() - line.p1().y()) + line.p1().y());
            QPointF _p1(line.x1() - rect.left() + w,
                        line.y1() - rect.top() + w);

            QPolygonF tri{
                p1, p2,
                QPointF(p3.x() - rect.left() + w, p3.y() - rect.top() + w)};
            painter.drawPolygon(tri);
            painter.drawPoint(line.p1());

            if (l.size() > 1) {
              pen.setWidth(w);
              painter.setPen(pen);
              QPointF m = QPointF((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2);
              painter.drawLine(
                  QLineF(QPointF(l[l.size() - 2].x2() - rect.left() + w,
                                 l[l.size() - 2].y2() - rect.top() + w),
                         m));
            }

            // painter.drawLine(_p1, p1);
            // painter.drawLine(_p1, p2);
          }
        }
        painter.end();
      }
    }
    WbScene(const Circle &l, const QPen &pen, const QBrush &brush,
            const QRect &rect)
        : pm((pen.width() * 2 + rect.width()),
             (pen.width() * 2 + rect.height())),
          p(QPoint(rect.left() - pen.width(), rect.top() - pen.width())),
          tp(WBOBJTYPE_CIRCLE) {
      (void)brush;
      extra = l;
      pm.fill(Qt::transparent);
      QPainter painter;
      // painter.drawPolygon(QPolygon())
      painter.begin(&pm);
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
      painter.setPen(pen);
      painter.setBrush(QColor(0, 0, 0, 0));
      painter.drawEllipse(pen.width(), pen.width(), l.r + pen.width(),
                          l.r + pen.width());
      painter.end();
    }
    void paint(QPainter &p) { p.drawPixmap(this->p, pm); }
    // LineList* asLineList(){if (tp==WBOBJTYPE_LINELIST)return
    // &std::get<LineList>(extra);else return nullptr;} const LineList*
    // asLineList()const{if (tp==WBOBJTYPE_LINELIST)return
    // &std::get<LineList>(extra);else return nullptr;}
    void reInit() {}
    bool isInRect(const QPoint &point) {
      return point.x() > p.x() && point.x() < p.x() + pm.width() &&
             point.y() > p.y() && point.y() < p.y() + pm.width();
    }
    // 判断鼠标轨迹是否经过对象
    bool isInteract(const QPoint &pa, const QPoint &pb) {
      switch (tp) {
      case WBOBJTYPE_LINELIST:
        for (int i = 0; i != std::get<LineList>(extra).size(); ++i) {
          if (std::get<LineList>(extra)[i].intersects(QLineF(pa, pb)) ==
              QLineF::BoundedIntersection) {
            // std::get<LineList>(extra).erase(std::get<LineList>(extra).begin()
            // + i);
            // --i;
            return true;
          }
        }
        // if (std::get<LineList>(extra).size() == 0) {
        // isValid = false;
        // }
        return false;
        break;
      case WBOBJTYPE_CIRCLE:
        return disLineCircle(QLineF(pa, pb), std::get<Circle>(extra)) <= 0;
      default:
        return false;
        break;
      }
    }
  };
  class WbTmpScene {
  public:
    int tp = WBOBJTYPE_LINELIST;
    std::variant<LineList> d = LineList();
    QPen pen;
    QBrush brush;
    void paint(QWidget &w) {
      LineList &l = std::get<LineList>(d);
      QPainter painter(&w);
      switch (tp) {
      case WBOBJTYPE_LINELIST:
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(pen);
        painter.setBrush(brush);
        if (l.size() == 1) {
          painter.drawLine(l.front());
        } else {
          for (int i = 0; i < l.size() - 1; ++i) {
            // pen.setWidthF(widthLevel(disPoints(l[i].p1(), l[i].p2())) *
            //              pen.widthF());
            painter.setPen(pen);
            painter.drawLine(l[i]);
          }
          if (l.size() > 1) {
            if (l.back().length() < 3) {
              painter.drawLine(l.back());
            } else {
              auto w = pen.widthF();
              pen.setWidthF(w / 2);
              painter.setPen(pen);
              auto &line = l.back();
              double A = line.y2() - line.y1(), B = line.x1() - line.x2();
              QPointF p1(line.x1() - w * A / (2 * (sqrt(A * A + B * B))),
                         line.y1() + w * B / (2 * (sqrt(A * A + B * B))));
              QPointF p2(line.x2() + w * A / (2 * (sqrt(A * A + B * B))),
                         line.y2() - w * B / (2 * (sqrt(A * A + B * B))));
              QPointF p3(5 * (line.p2().x() - line.p1().x()) + line.p1().x(),
                         5 * (line.p2().y() - line.p1().y()) + line.p1().y());
              QPointF _p1(line.x1(), line.y1());

              QPolygonF tri{p1, p2, QPointF(p3.x(), p3.y())};
              painter.drawPolygon(tri);
              painter.drawPoint(line.p1());

              if (l.size() > 1) {
                pen.setWidth(w);
                painter.setPen(pen);
                QPointF m =
                    QPointF((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2);
                painter.drawLine(QLineF(
                    QPointF(l[l.size() - 2].x2(), l[l.size() - 2].y2()), m));
              }

              // painter.drawLine(_p1, p1);
              // painter.drawLine(_p1, p2);
            }
          }
          break;
        default:
          break;
        }
        painter.end();
      }
    }
    LineList &lineList() { return std::get<LineList>(d); }
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
