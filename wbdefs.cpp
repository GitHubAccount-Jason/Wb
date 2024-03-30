#ifndef WBDEFS_CPP
#define WBDEFS_CPP
#include "wbdefs.h"
#include "whiteboard.h"

class WbControlFreePen : public WbControl {
private:
  enum { NONE, TEMP, DONE } state;
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
    state = NONE;
    // fns.emplaceBack(WbFunction())
  }
  ~WbControlFreePen(){}
  void onPress(const QPointF &p) override {
    state = TEMP;
    path.moveTo(p);

    trace.push_back(p);
  }

  void onMove(const QPointF &p) override {
      if (trace.length()>=2){
      isDrawPatchPoint = true;
      }
      trace.push_back(p);
      if (trace.length()>=3){
          path.quadTo(trace[trace.length()-2], mid(trace[trace.length()-2], trace[trace.length()-1]));
      }

  }

  void onRelease(const QPointF &p) override {
      if (trace.length()==1){
          // WARNING : MAY BE PROBLEMS
          return;
      }
      auto l = this->wb->evinfo.l;
      auto r = this->wb->evinfo.r;
      auto u = this->wb->evinfo.u;
      auto d = this->wb->evinfo.d;
    QPointF offset(-l+pen_.widthF(), -u+pen_.widthF());
      QPointF beginPoint = mid(trace[trace.length()-2], trace[trace.length()-3])+offset;
      QLineF lastLine = QLineF(trace[trace.length()-2], trace.back());
      lastLine.translate(offset);
      path.clear();
      path.moveTo(trace.front());
      if (trace.length()>=2){
          path.lineTo(mid(trace[0], trace[1]));
      }
      for (int i = 0; i + 2 < trace.length()-1; ++i) {
         path.quadTo(trace[i+1],mid(trace[i+1], trace[i + 2]));
      }
      path.translate(offset);
      this->p = QPointF(l-pen_.widthF(), u-pen_.widthF());
      // qDebug()<<"p="<<this->p;
      this->last = lastLine;
      this->begin = beginPoint;
      pm = (QPixmap(QSize(r-l+2*pen_.widthF(), d-u+2*pen_.widthF())));
      pm.fill(Qt::transparent);
      QPainter painter;
      painter.begin(&pm);
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
      painter.setPen(pen_);
      painter.setBrush(brush_);
      painter.drawPath(path);
      if (lastLine.length() >= 5) {
        drawPatchPoint(&painter, lastLine, beginPoint, pen_);
      }
      painter.end();
      state=DONE;
    }
  void onPaint(QPixmap&w) override {
    switch (state) {
    case NONE:
      return;
    case TEMP:
    {
        QPainter painter(&w);
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(pen_);
        painter.setBrush(brush_);
      painter.drawPath(path);
      if (isDrawPatchPoint && last.length() >= 5) {
        // qDebug()<<last;
        drawPatchPoint(&painter,last,begin, pen_);
      }
      painter.end();
      break;
    }
  case DONE:
    {
        QPainter painter(&w);
      // painter.setCompositionMode(QPainter::CompositionMode_Source);
      painter.setRenderHint(QPainter::Antialiasing);
      painter.drawPixmap(this->p, pm);
      painter.end();
    }
        break;
  }
}
}
;
#endif
