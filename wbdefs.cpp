#ifndef WBDEFS_CPP
#define WBDEFS_CPP
#include "wbdefs.h"
#include "whiteboard.h"

class WbControlFreePen : public WbControl {
private:
  QList<QPointF> trace;

  bool isDrawPatchPoint = false;
  QPainterPath path;
  QLineF last;
  QPointF begin;

  QPointF p;
  QPixmap pm;

public:
  WbControlFreePen(Whiteboard *wb, const QPen &pen, const QBrush &brush)
      : WbControl(wb, pen, brush) {
    // fns.emplaceBack(WbFunction())
  }
  WbControlFreePen(Whiteboard *wb, const QPen &pen, const QBrush &brush, const QPainterPath&path, const QLineF&last, const QPointF& begin, const QPointF& p)
      : WbControl(wb, pen, brush), path(path), last(last), begin(begin), p(p) {
    auto l = this->wb->evinfo.l;
    auto r = this->wb->evinfo.r;
    auto u = this->wb->evinfo.u;
    auto d = this->wb->evinfo.d;
    pm = (QPixmap(QSize(r - l + 2 * pen_.widthF(), d - u + 2 * pen_.widthF())));
    pm.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&pm);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(pen_);
    painter.setBrush(brush_);
    painter.drawPath(path);
    if (last.length() >= 5) {
      drawPatchPoint(&painter,last,begin, pen_);
    }
    painter.end();
    // fns.emplaceBack(WbFunction())
  }
  ~WbControlFreePen() {}


  void onPaint(QPixmap &w) override {
      QPainter painter(&w);
      // painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
      painter.drawPixmap(this->p, pm);
      painter.end();
  }
};
class WbTmpFreePen:public WbTmp{
private:
  QList<QPointF> trace;

    bool activated=false;
  bool isDrawPatchPoint = false;
  QPainterPath path;
  QLineF last;
  QPointF begin;

public:
  WbTmpFreePen(Whiteboard *wb, const QPen &pen, const QBrush &brush)
      :WbTmp(wb, pen, brush) {
    // fns.emplaceBack(WbFunction())
  }
  ~WbTmpFreePen() {}
  void onPress(const QPointF &p) override {
      trace.clear();
      isDrawPatchPoint = false;
      path.clear();
      last = QLineF(0, 0, 0, 0);
      begin = QPointF(0, 0);
      activated = true;

    trace.push_back(p);
  }

  void onMove(const QPointF &p) override {
    if (trace.length() >= 2) {
      isDrawPatchPoint = true;
    }
    if (trace.length() == 2) {
      path.moveTo(mid(trace.front(), trace.back()));
    }
    trace.push_back(p);
    if (trace.length() >= 4) {
      path.quadTo(trace[trace.length() - 3],
                  mid(trace[trace.length() - 3], trace[trace.length() - 2]));
    QPointF beginPoint =
        mid(trace[trace.length() - 2], trace[trace.length() - 3]) ;
    QLineF lastLine = QLineF(trace[trace.length() - 2], trace.back());
    last = lastLine;
    begin = beginPoint;
    }
  }

  void onRelease(const QPointF &p) override {
    if (trace.length() <= 2) {
      // WARNING : MAY BE PROBLEMS
      return;
    }
    auto l = this->wb->evinfo.l;
    auto r = this->wb->evinfo.r;
    auto u = this->wb->evinfo.u;
    auto d = this->wb->evinfo.d;
    QPointF offset(-l + pen_.widthF(), -u + pen_.widthF());
    QPointF beginPoint =
        mid(trace[trace.length() - 2], trace[trace.length() - 3]) + offset;
    QLineF lastLine = QLineF(trace[trace.length() - 2], trace.back());
    lastLine.translate(offset);
    path.translate(offset);
    wb->addControl(new WbControlFreePen(wb, pen_,Qt::NoBrush,path, lastLine, beginPoint,  QPointF(l - pen_.widthF(), u - pen_.widthF())));
    this->last = lastLine;
    this->begin = beginPoint;

    activated = false;
  }
  void onPaint(QPixmap &w) override {
      if (!activated){
          return;
      }
      QPainter painter(&w);
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
      painter.setPen(pen_);
      painter.setBrush(brush_);
      painter.drawPath(path);
      if (isDrawPatchPoint && last.length() >= 5) {
        drawPatchPoint(&painter, last, begin, pen_);
      }
      painter.end();
    }



};

#endif
