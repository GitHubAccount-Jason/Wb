#include "whiteboard.h"
#include "floatingwindow.h"

// nextScenePos == scene.size() => cur scene is tmpScene
// else cur scene is scene[nextScenePos]
Whiteboard::Whiteboard()
    : mode(WBMODE_PEN), pen(QColorConstants::Blue, 10, Qt::SolidLine,
                            Qt::RoundCap, Qt::RoundJoin),
      brush(QColorConstants::Red) {
  nextScenePos = 0;
  fl = new FloatingWindow(this);
  fl->hide();
  tmpScene.pen = &pen;
  tmpScene.brush = nullptr;
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
void Whiteboard::mousePressEvent(QMouseEvent *ev) { /*{{{*/
  traceLen = 0;
  traceSpeed = 0;
  isPressTriggered = true;
  if ((ev->button() == Qt::MouseButton::RightButton)) {
    return;
  }
  tmpScene.path.clear();
  tmpScene.isDrawPatchPoint=false;
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
  tmpScene.path.moveTo(ev->position());
  } /*}}}*/

void Whiteboard::mouseMoveEvent(QMouseEvent *ev) { /*{{{*/
  traceLen += disPoints(mouseMoveTrace.back(), ev->position());
  l = min(l, ev->pos().x());
  r = max(r, ev->pos().x());
  u = min(u, ev->pos().y());
  d = max(d, ev->pos().y());
#define AVAILABLE (4)
  if (mode == WBMODE_PEN) {
    if (mouseMoveTrace.size() != 0 &&
        (qAbs(ev->position().x() - mouseMoveTrace.last().x()) < AVAILABLE &&
         qAbs(ev->pos().y() - mouseMoveTrace.last().y()) < AVAILABLE)) {
      return;
    }
    if (mouseMoveTrace.length()>=2){
        if (mouseMoveTrace.length()>=3){
        tmpScene.path.quadTo(mouseMoveTrace[mouseMoveTrace.length()-2], mid(mouseMoveTrace[mouseMoveTrace.length()-1], mouseMoveTrace[mouseMoveTrace.length()-2]));
        }
        tmpScene.isDrawPatchPoint=true;
    tmpScene.lastline = QLineF(mouseMoveTrace.last(), ev->position());
    tmpScene.beginPoint = mid(mouseMoveTrace[mouseMoveTrace.length()-1], mouseMoveTrace[mouseMoveTrace.length()-2]);
    }

  } else if (mode == WBMODE_ERASER) {
  }
  mouseMoveTrace.push_back(ev->position());

  traceSpeed = traceLen / (mouseMoveTrace.size() - 1);
  update();
} /*}}}*/

void Whiteboard::mouseReleaseEvent(QMouseEvent *ev) { /*{{{*/
    qDebug()<<ev->pos();

  if ((ev->buttons() & Qt::MouseButton::RightButton) ||
      isPressTriggered == false) {
    return;
  }
  traceLen += disPoints(mouseMoveTrace.back(), ev->position());
  traceSpeed = traceLen / (mouseMoveTrace.size() - 1);
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
    }
    if (!isline) {
      double e = 0;
      QPoint o = QPoint((l + r) / 2, (u + d) / 2);
      double _r = (r - l) / 2;
      for (auto &point : mouseMoveTrace) {
        e += disPointCircle(point, Circle{o, _r});
      }
      e /= mouseMoveTrace.size();
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
      QPainterPath path;
    if (isline == true) {
      path.moveTo(mapToRect(mouseMoveTrace.front(), QRect{l, u, r - l, d - u},
                            pen.widthF()));
        path.lineTo(mouseMoveTrace.back());
    } else if (iscircle == true) {
        path.addEllipse(QRect{circleO.x() - (int)circleR,
                             circleO.y() - (int)circleR, r * 2, r * 2});
    } else {
      path.moveTo(mouseMoveTrace.front());
      if (mouseMoveTrace.length()>=2){
          path.lineTo(mid(mouseMoveTrace[0], mouseMoveTrace[1]));
      }
      for (int i = 0; i + 2 < mouseMoveTrace.length()-1; ++i) {
         path.quadTo(mouseMoveTrace[i+1],mid(mouseMoveTrace[i+1], mouseMoveTrace[i + 2]));
      }
    }
    QPointF offset(-l+pen.widthF(), -u+pen.widthF());
      path.translate(offset);

      QPointF beginPoint = mid(mouseMoveTrace[mouseMoveTrace.length()-2], mouseMoveTrace[mouseMoveTrace.length()-3])+offset;
      QLineF lastLine = QLineF(mouseMoveTrace[mouseMoveTrace.length()-2], mouseMoveTrace.back());
      lastLine.translate(offset);
      // qDebug()<<mouseMoveTrace.back()<<"|"<<ev->pos();

      opSc.emplaceBack(path, lastLine,beginPoint, pen,QBrush(),QRectF(l, u, r-l, d-u));
  } else if (mode == WBMODE_ERASER) {
  }

  tmpScene.path.clear();
  tmpScene.isDrawPatchPoint=false;

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
