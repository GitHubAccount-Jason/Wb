#ifndef WBDEFS_CPP
#define WBDEFS_CPP
#include "wbdefs.h"
#include "whiteboard.h"

class WbControlFreePen : public WbControl {
private:
  QList<QPointF> trace;

  QPainterPath path;
  QLineF last;
  QPointF begin;

  // >= 2000: false
  // < 2000: true
  bool usePm = false;
  QPointF p;
  QPixmap pm;
  QPainterPath interactpath;

public:
  WbControlFreePen(Whiteboard *wb, const QPen &pen, const QBrush &brush)
      : WbControl(wb, pen, brush) {
    // fns.emplaceBack(WbFunction())
  }
  WbControlFreePen(Whiteboard *wb, const QPen &pen, const QBrush &brush,
                   const QPainterPath &path, const QLineF &last,
                   const QPointF &begin, const QPointF &p)
      : WbControl(wb, pen, brush), path(path), last(last), begin(begin), p(p) {
    auto l = this->wb->evinfo.l;
    auto r = this->wb->evinfo.r;
    auto u = this->wb->evinfo.u;
    auto d = this->wb->evinfo.d;
    if ((r - l) * (d - u) >= 2000) {
      usePm = false;

    } else {
      usePm = true;
      QPointF offset(-l + wb->pen.widthF(), -u + wb->pen.widthF());
      this->begin = this->begin + offset;
      this->last.translate(offset);
      this->path.translate(offset);
      pm = (QPixmap(
          QSize(r - l + 2 * pen_.widthF(), d - u + 2 * pen_.widthF())));
      pm.fill(Qt::transparent);
      QPainter painter;
      painter.begin(&pm);
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
      painter.setPen(pen_);
      painter.setBrush(brush_);
      painter.drawPath(path);
      if (last.length() >= 5) {
        drawPatchPoint(&painter, last, begin, pen_);
      }
      painter.end();
    }
    interactpath = path;
    interactpath.quadTo(begin, last.center());
    registfn(WbFunction::make<bool(const QLineF &, double)>(
        "isInteract", [this](const QLineF &p, double w) -> bool {
          if (this->usePm) {
            QLineF real = p.translated(-this->p);
              if (!pm.rect().translated(this->p.toPoint()).intersects(QRectF(real.p1(), real.p2()).toRect())){
                  return false;
              }

            if (!isLineIntersectRect(real, pm.rect())) {
              return false;
            }

            for (int i = 0; i <= this->path.length(); ++i) {
              if (disPointSeg(
                      this->path.pointAtPercent((qreal)i / this->path.length()),
                      real) <= w) {
                    qDebug()<<"del";
                return true;
              }
            }
            return false;

          } else {
              if (!this->path.intersects(QRectF(p.p1(), p.p2()).toRect())){
                  return false;
              }
            for (int i = 0; i <= this->path.length(); ++i) {
              if (disPointSeg(
                      this->path.pointAtPercent((qreal)i / this->path.length()),
                      p) <= w) {
                return true;
              }
            }
            return false;
          }
        }));

    // fns.emplaceBack(WbFunction())
  }
  ~WbControlFreePen() {}

  void onPaint(QPixmap &w) override {
    if (usePm) {
      QPainter painter(&w);
      // painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
      painter.drawPixmap(this->p, pm);
      painter.end();
    } else {
      QPainter painter;
      painter.begin(&w);
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
      painter.setPen(pen_);
      painter.setBrush(brush_);
      painter.drawPath(path);
      if (last.length() >= 5) {
        drawPatchPoint(&painter, last, begin, pen_);
      }
      painter.end();
    }
  }
};
class WbTmpFreePen : public WbTmp {
private:
  QList<QPointF> trace;

  bool activated = false;
  bool isDrawPatchPoint = false;
  QPainterPath path;
  QLineF last;
  QPointF begin;
  QPixmap pm;

public:
  WbTmpFreePen(Whiteboard *wb) : WbTmp(wb) {
    // fns.emplaceBack(WbFunction())
  }
  ~WbTmpFreePen() {}
  void onPress(const QPointF &p) override {
    pm = QPixmap(wb->geometry().size());
    pm.fill(Qt::transparent);
    trace.clear();
    isDrawPatchPoint = false;
    path.clear();
    last = QLineF(0, 0, 0, 0);
    begin = QPointF(0, 0);
    activated = true;

    trace.push_back(p);
  }

  void onMove(const QPointF &p) override {
    if (!activated) {
      return;
    }
    trace.push_back(p);
    if (trace.length() == 2) {
      path.moveTo(mid(trace.front(), trace.back()));
      return;
    }
    if (trace.length() <= 3) {
      return;
    }
    QPainterPath _p;
    _p.moveTo(mid(trace[trace.length() - 4], trace[trace.length() - 3]));
    _p.quadTo(trace[trace.length() - 3],
              mid(trace[trace.length() - 3], trace[trace.length() - 2]));
    QPainter _painter(&pm);
    _painter.setCompositionMode(QPainter::CompositionMode_Source);
    _painter.setRenderHint(QPainter::Antialiasing);
    _painter.setPen(wb->pen);
    _painter.setBrush(Qt::NoBrush);
    _painter.drawPath(_p);
    if (trace.length() >= 4) {
      path.quadTo(trace[trace.length() - 3],
                  mid(trace[trace.length() - 3], trace[trace.length() - 2]));
      QPointF beginPoint =
          mid(trace[trace.length() - 2], trace[trace.length() - 3]);
      QLineF lastLine = QLineF(trace[trace.length() - 2],
                               mid(trace[trace.length() - 2], trace.back()));
      last = lastLine;
      begin = beginPoint;
      drawPatchPoint(&_painter, lastLine, begin, wb->pen);
      _painter.end();
    }
  }

  void onRelease(const QPointF &p) override {
    if (!activated) {
      return;
    }
    if (trace.length() <= 2) {
      // WARNING : MAY BE PROBLEMS
      return;
    }
    auto l = this->wb->evinfo.l;
    auto r = this->wb->evinfo.r;
    auto u = this->wb->evinfo.u;
    auto d = this->wb->evinfo.d;
    QPointF beginPoint =
        mid(trace[trace.length() - 2], trace[trace.length() - 3]);
    QLineF lastLine = QLineF(trace[trace.length() - 2], trace.back());
    std::shared_ptr<WbControlFreePen> __sp =
        std::shared_ptr<WbControlFreePen>(new WbControlFreePen(
            wb, wb->pen, Qt::NoBrush, path, lastLine, beginPoint,
            QPointF(l - wb->pen.widthF(), u - wb->pen.widthF())));
    wb->addControl(__sp);
    this->last = lastLine;
    this->begin = beginPoint;

    activated = false;
  }
  void onPaint(QPixmap &w) override {
    if (!activated) {
      return;
    }
    QPainter painter(&w);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(QPoint(0, 0), pm);
    painter.end();
  }
};
class WbTmpEraser : public WbTmp {
private:
  QList<QPointF> trace;

  bool activated = false;
  bool isDrawPatchPoint = false;
  bool _2move=false;
  QPointF last;

public:
  WbTmpEraser(Whiteboard *wb) : WbTmp(wb) {
    // fns.emplaceBack(WbFunction())
  }
  ~WbTmpEraser() {}
  void onPress(const QPointF &p) override {
    qDebug() << "press\n";
    trace.clear();
    isDrawPatchPoint = false;
    last = QPointF(0, 0);
    activated = true;
    _2move=false;
  }

  void onMove(const QPointF &p) override {
    if (!activated) {
      return;
    }
    if (_2move){
    QLineF line(last, p);
    for (int i = 0; i != wb->scenes.back().size(); ++i) {
      if ((wb->scenes.back()[i])
              ->checkfn<bool(const QLineF &, double)>("isInteract")) {
        auto p = (wb->scenes.back()[i])
                     ->getfn<bool(const QLineF &, double)>("isInteract");
        if (p.value()(line, wb->pen.widthF())) {
          wb->store();
          wb->scenes.back().erase(wb->scenes.back().begin() + i);
          --i;
          wb->flush();
          wb->update();
        }
      }
    }
    }
    last = p;
    _2move=true;
  }

  void onRelease(const QPointF &p) override {
    if (!activated) {
      return;
    }
    activated = false;
  }
  void onPaint(QPixmap &w) override { return; }
};

#endif
