#include "whiteboard.h"
// nextScenePos == scene.size() => cur scene is tmpScene
// else cur scene is scene[nextScenePos]
Whiteboard::Whiteboard()
    : mode(WBMODE_PEN),
      pen(QColorConstants::Blue, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin),
      brush(QColorConstants::Gray) {
    nextScenePos = 0;
    fl = new FloatingWindow(this);
  qDebug() << "Whiteboard constructed";
}
Whiteboard::~Whiteboard(){

}

void Whiteboard::paintEvent(QPaintEvent *ev) { /*{{{*/

  QPainter painter(this);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  painter.setRenderHint(QPainter::Antialiasing);
  for (qsizetype i =0;i!=nextScenePos;++i) {
    opSc[i].paint(painter);
  }
  for (auto obj = tmpScene.constBegin(); obj != tmpScene.constEnd(); ++obj) {
    if (obj->isVisible) {
      switch (obj->type) {
      case WBOBJTYPE_LINE:
        painter.setPen(obj->pen);
        painter.setBrush(obj->brush);
        painter.drawLine(obj->line());
        break;
      case WBOBJTYPE_LINELIST:
        painter.setPen(obj->pen);
        painter.setBrush(obj->brush);
        for (const auto &l : *(LineList *)obj->extra) {
          painter.drawLine(l);
        }
        break;
      default:
        break;
      }
    }
  }
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

  if ((ev->button() == Qt::MouseButton::RightButton)) {
    return;
  }
  isPressTriggered=true;
  if (!opSc.empty()){
    opSc.erase(opSc.begin()+nextScenePos, opSc.end());
  }
  l = min(l, ev->pos().x());
  r = max(r, ev->pos().x());
  u = min(u, ev->pos().y());
  d = max(d, ev->pos().y());
  if (mode == WBMODE_PEN) {
    tmpScene.push_back(WbObject(LineList(), pen, brush));
    this->mouseMoveTrace.push_back(ev->position());
  } else if (mode == WBMODE_ERASER) {
    this->mouseMoveTrace.push_back(ev->position());
  }
} /*}}}*/

void Whiteboard::mouseMoveEvent(QMouseEvent *ev) { /*{{{*/
  if ((ev->buttons() & Qt::MouseButton::RightButton)||isPressTriggered==false) {
    return;
  }
  l = min(l, ev->pos().x());
  r = max(r, ev->pos().x());
  u = min(u, ev->pos().y());
  d = max(d, ev->pos().y());
#define mid(a, b) (QPointF((a.x() + b.x()) / 2, (a.y() + b.y()) / 2))
#define AVAILABLE (2)
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
      tmpScene.back().linelist().push_back(
          QLineF(mouseMoveTrace.back(), __mid));
      ll.push_back(__mid);
      ll.push_back(ev->position());
    }
    ll.push_back(__mid);
    bezier2(ll[0], ll[1], ll[2], 0.08, tmpScene.back().linelist());
    ll.pop_front();
    ll.pop_front();
    ll.push_back(ev->position());
    this->mouseMoveTrace.push_back(ev->position());
    update();
  } else if (mode == WBMODE_ERASER) {
    for (int i = 0; i != tmpScene.size(); ++i) {
      if (tmpScene[i].type == WBOBJTYPE_LINELIST) {
        for (const auto &p : tmpScene[i].linelist()) {
          const Line mouseline(mouseMoveTrace.back(), ev->position());
          if (p.intersects(mouseline, nullptr) ==
              QLineF::IntersectType::BoundedIntersection) {
            tmpScene.removeAt(i);
            --i;
            update();
            break;
          }
        }
      }
    }
    mouseMoveTrace.push_back(ev->position());
  }
} /*}}}*/

void Whiteboard::mouseReleaseEvent(QMouseEvent *ev) { /*{{{*/
  if ((ev->button() == Qt::MouseButton::RightButton)) {
    return;
  }
  if (isPressTriggered==false){
      return;
  }
  isPressTriggered=false;
  if (mouseMoveTrace.size() == 1) {
    mouseMoveTrace.clear();
    return;
  }
  if (mode == WBMODE_PEN) {
    if (mouseMoveTrace.size() != 1) {
      update();
    }
    // scan near line
    for (auto i = tmpScene.back().linelist().begin();
         i < tmpScene.back().linelist().end() - 1; ++i) {
      if (isTwoLineNear(*i, *(i + 1))) {
        auto l = chgTwoNearLineIntoOne(*i, *(i + 1));
        i = tmpScene.back().linelist().erase(i);
        *i = l;
        --i;
        continue;
      }
    }
  } else if (mode == WBMODE_ERASER) {
  }
  opSc.emplaceBack(tmpScene.back().linelist(), pen, brush,
                   QRect{l, u, r - l, d - u});
  tmpScene.clear();

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
