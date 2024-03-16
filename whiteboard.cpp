#include "whiteboard.h"
#include "floatingwindow.h"

// nextScenePos == scene.size() => cur scene is tmpScene
// else cur scene is scene[nextScenePos]
Whiteboard::Whiteboard()
    : mode(WBMODE_PEN),
      pen(QColorConstants::Blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin),
      brush(QColorConstants::Blue) {
  nextScenePos = 0;
  fl = new FloatingWindow(this);
  fl->hide();
  tmpScene.pen=pen;
  tmpScene.brush=brush;
  qDebug() << "Whiteboard constructed";
}
Whiteboard::~Whiteboard() {}

void Whiteboard::paintEvent(QPaintEvent *ev) { /*{{{*/
  (void)ev;
  QPainter painter(this);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  painter.setRenderHint(QPainter::Antialiasing);
  for (qsizetype i = 0; i != nextScenePos; ++i) {
    opSc[i].paint(painter);
  }
  tmpScene.paint(*this);

   painter.end();
} /*}}}*/
void bezier2(const QPointF &p0, const QPointF &p1, const QPointF &p2, /*{{{*/
             qreal prec, QList<QLineF> &push_list) {

  QVarLengthArray<QPointF> retp(1 / prec + 2);
  size_t i = 0;
  for (; i * prec < 1; ++i) {
#define t (i * prec)
    retp[i] =
        (1.0 - t) * (1.0 - t) * p0 + 2.0 * t * (1.0 - t) * p1 + t * t * p2;
#undef t
  }
  retp[i] = p2;
  i = 1;
  for (; i * prec < 1; ++i) {
    push_list.emplaceBack(retp[i - 1], retp[i]);
  }
  push_list.emplaceBack(retp[i - 1], retp[i]);
} /*}}}*/
template <class A> A min(const A &a, const A &b) { return std::min(a, b); }
template <class A> A min(const A &a, const A &b, const A &L...) {
  return min(min(a, b), L);
}
template <class A> A max(const A &a, const A &b) { return std::max(a, b); }
template <class A> A max(const A &a, const A &b, const A &L...) {
  return max(max(a, b), L);
}
bool isTwoLineNear(const QLineF &a, const QLineF &b) {
  qreal le = min<qreal>(a.y1(), a.y2(), b.y1(), b.y2());
  qreal ri = max<qreal>(a.y1(), a.y2(), b.y1(), b.y2());
  qreal up = min<qreal>(a.x1(), a.x2(), b.x1(), b.x2());
  qreal lo = max<qreal>(a.x1(), a.x2(), b.x1(), b.x2());
  return std::sqrt(std::pow(ri - le, 2) + std::pow(up - lo, 2)) <= 1.414;
}
QLineF chgTwoNearLineIntoOne(const QLineF &a, const QLineF &b) {
  return QLineF(a.p1(), b.p2());
}

void Whiteboard::mousePressEvent(QMouseEvent *ev) { /*{{{*/
  traceLen = 0;
    traceSpeed=0;
  if ((ev->button() == Qt::MouseButton::RightButton)) {
    return;
  }
  isPressTriggered = true;
  if (!opSc.empty()) {
    opSc.erase(opSc.begin() + nextScenePos, opSc.end());
  }
  l = min(l, ev->pos().x());
  r = max(r, ev->pos().x());
  u = min(u, ev->pos().y());
  d = max(d, ev->pos().y());
  if (mode == WBMODE_PEN) {
    // tmpScene.push_back(WbObject(LineList(), pen, brush));
    this->mouseMoveTrace.push_back(ev->position());
  } else if (mode == WBMODE_ERASER) {
    this->mouseMoveTrace.push_back(ev->position());
  }
} /*}}}*/

void Whiteboard::mouseMoveEvent(QMouseEvent *ev) { /*{{{*/
  traceLen += disPoints(mouseMoveTrace.back(), ev->position());

  if ((ev->buttons() & Qt::MouseButton::RightButton) ||
      isPressTriggered == false) {
    return;
  }
  l = min(l, ev->pos().x());
  r = max(r, ev->pos().x());
  u = min(u, ev->pos().y());
  d = max(d, ev->pos().y());
#define mid(a, b) (QPointF((a.x() + b.x()) / 2, (a.y() + b.y()) / 2))
#define AVAILABLE (4)
  if (mode == WBMODE_PEN) {
    if (mouseMoveTrace.size() != 0 &&
        (qAbs(ev->position().x() - mouseMoveTrace.last().x()) < AVAILABLE &&
         qAbs(ev->pos().y() - mouseMoveTrace.last().y()) < AVAILABLE)) {
      return;
    }
    static QList<QPointF> ll;
    QPointF __mid = mid(mouseMoveTrace.back(), ev->position());
    if (mouseMoveTrace.length() == 1) {
      // New line
      ll.clear();
      tmpScene.lineList().push_back(
          QLineF(mouseMoveTrace.back(), __mid));
      ll.push_back(__mid);
      ll.push_back(ev->position());
    }
    ll.push_back(__mid);
    bezier2(ll[0], ll[1], ll[2], 0.08, tmpScene.lineList());
    ll.pop_front();
    ll.pop_front();
    ll.push_back(ev->position());
    this->mouseMoveTrace.push_back(ev->position());
    update();
  } else if (mode == WBMODE_ERASER)
  {
    }
    mouseMoveTrace.push_back(ev->position());
        traceSpeed=traceLen/(mouseMoveTrace.size()-1);
} /*}}}*/

void Whiteboard::mouseReleaseEvent(QMouseEvent *ev) { /*{{{*/
  traceLen += disPoints(mouseMoveTrace.back(), ev->position());
    traceSpeed=traceLen/(mouseMoveTrace.size()-1);
  l = min(l, ev->pos().x());
  r = max(r, ev->pos().x());
  u = min(u, ev->pos().y());
  d = max(d, ev->pos().y());
  if ((ev->button() == Qt::MouseButton::RightButton)) {
    return;
  }
  if (isPressTriggered == false) {
    return;
  }
  isPressTriggered = false;
  if (mouseMoveTrace.size() == 1) {
    mouseMoveTrace.clear();
    return;
  }

  if (mode == WBMODE_PEN) {
    bool isline = true, iscircle = true;
    QPoint circleO;
    double circleR;
    if (mouseMoveTrace.size() != 1) {
      update();
    }
    // scan near line
    for (auto i = tmpScene.lineList().begin();
         i < tmpScene.lineList().end() - 1; ++i) {
      if (isTwoLineNear(*i, *(i + 1))) {
        auto l = chgTwoNearLineIntoOne(*i, *(i + 1));
        i = tmpScene.lineList().erase(i);
        *i = l;
        --i;
        continue;
      }
    }
    {
#define x1 mouseMoveTrace.front().x()
#define y1 mouseMoveTrace.front().y()
#define x2 mouseMoveTrace.back().x()
#define y2 mouseMoveTrace.back().y()
      double e = 0;
      for (auto &point : mouseMoveTrace) {
        e += disPointLine(
            point, QLineF(mouseMoveTrace.front(), mouseMoveTrace.back()));
      }
      if (1 - (e / traceSpeed) < -2) {
        isline = false;
      }
      qDebug()<<1 - (e / traceSpeed)<<"\n";
    }
    if (!isline) {
      double e = 0;
      QPoint o = QPoint((l + r) / 2, (u + d) / 2);
      double _r = (r - l) / 2;
      for (auto &point : mouseMoveTrace) {
        e += disPointCircle(point, Circle{o, _r});
      }
      e/=mouseMoveTrace.size();
      if (1 - (e / traceSpeed) < 0.5) {
        iscircle = false;
      }
      if (iscircle) {
        circleO = o;
        circleR = _r;
      }
    }
#undef x1
#undef y1
#undef x2
#undef y2
    if (isline == true) {
      opSc.emplaceBack(
          LineList{Line(mouseMoveTrace.front(), mouseMoveTrace.back())}, pen,
          brush, QRect{l, u, r - l, d - u});
    } else if (iscircle == true) {
      opSc.emplaceBack(Circle{circleO, circleR}, pen, brush,
                       QRect{circleO.x() - (int)circleR,
                             circleO.y() - (int)circleR, r * 2, r * 2});
    } else {
      opSc.emplaceBack(tmpScene.lineList(), pen, brush,
                       QRect{l, u, r - l, d - u});
    }
  } else if (mode == WBMODE_ERASER) {
  }

  tmpScene.lineList().clear();

  nextScenePos = opSc.size();
  mouseMoveTrace.clear();
  update();
  l = INT_MAX;
  r = INT_MIN;
  u = INT_MAX;
  d = INT_MIN;

} /*}}}*/

bool Whiteboard::undo() { /*{{{*/
  if (nextScenePos == 0) {
    return false;
  }
  --nextScenePos;
  update();
  return true;
} /*}}}*/
bool Whiteboard::redo() { /*{{{*/
  if (nextScenePos == opSc.size()) {
    return false;
  }
  ++nextScenePos;
  update();
  return true;
} /*}}}*/

void Whiteboard::changeMode(int mode) { /*{{{*/
  if (mode == this->mode) {
    return;
  }
  switch (mode) {
  case WBMODE_TRANSPARENT:
    this->mode = WBMODE_TRANSPARENT;
    break;
  case WBMODE_PEN:
    this->mode = WBMODE_PEN;
    break;
  default:
    break;
  }
} /*}}}*/
void Whiteboard::setUntransparent() {
  fl->setUntransparent();
  setFocus();
}
void Whiteboard::setTransparent() { fl->setTransparent(); }
