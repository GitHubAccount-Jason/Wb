#include "whiteboard.h"
#include "floatingwindow.h"
#include "wbdefs.cpp"

// nextScenePos == scene.size() => cur scene is tmpScene
// else cur scene is scene[nextScenePos]
Whiteboard::Whiteboard()
    : mode(WBMODE_PEN), pen(QColorConstants::Blue, 10, Qt::SolidLine,
                            Qt::RoundCap, Qt::RoundJoin),
    brush(QColorConstants::Red) ,pm(QSize(QGuiApplication::screens().at(0)->geometry().size())){
    pm.fill(Qt::transparent);

  nextScenePos = 0;
  fl = new FloatingWindow(this);
  fl->hide();
  qDebug() << "Whiteboard constructed\n";
}
Whiteboard::~Whiteboard() {
    for (auto i:controls){
        delete i;
    }
}

void Whiteboard::paintEvent(QPaintEvent *ev) { /*{{{*/
  (void)ev;
    pm.fill(Qt::transparent);
    for (int i = 0; i != nextScenePos; ++i){
        controls[i]->onPaint(pm);
    }
    QPainter p(this);
    p.drawPixmap(QPoint(0, 0), pm);
    p.end();

} /*}}}*/
void Whiteboard::mousePressEvent(QMouseEvent *ev) { /*{{{*/
  if (evinfo.init(ev)==false){
      return;
  }
  if (controls.length()!=0){
  controls.erase(controls.begin()+nextScenePos, controls.end());
  }

  if (mode == WBMODE_PEN) {
  controls.push_back(new WbControlFreePen(this, pen,Qt::NoBrush));
  controls.back()->onPress(ev->position());
  nextScenePos = controls.length();
  } else if (mode == WBMODE_ERASER) {
  }


  } /*}}}*/

void Whiteboard::mouseMoveEvent(QMouseEvent *ev) { /*{{{*/
  if (evinfo.refresh(ev)==false){
      return ;
  }
  if (evinfo.mouseMoveTrace.length()<=3){
      return ;
  }
#define AVAILABLE (4)
  if (mode == WBMODE_PEN) {
  controls.back()->onMove(ev->position());

  } else if (mode == WBMODE_ERASER) {
  }
  update();
} /*}}}*/

void Whiteboard::mouseReleaseEvent(QMouseEvent *ev) { /*{{{*/
  auto& mouseMoveTrace = evinfo.mouseMoveTrace;
  auto &l = evinfo.l, &r = evinfo.r, &u = evinfo.u, &d = evinfo.d;

  // isPressTriggered = false;
  if (mouseMoveTrace.size() == 1) {
    mouseMoveTrace.clear();
    return;
  }
  if (mode == WBMODE_PEN) {
    bool isline = true, iscircle = true;
    QPoint circleO;
    double circleR;
      if (0){
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
      if (1 - (e / evinfo.traceSpeed) < -2) {
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
      if (1 - (e / evinfo.traceSpeed) < 0.5) {
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
      }
      QPainterPath path;
    if (isline == true&&0) {
      path.moveTo(mapToRect(mouseMoveTrace.front(), QRect{l, u, r - l, d - u},
                            pen.widthF()));
        path.lineTo(mouseMoveTrace.back());
    } else if (iscircle == true&&0) {
        path.addEllipse(QRect{circleO.x() - (int)circleR,
                             circleO.y() - (int)circleR, r * 2, r * 2});
    } else {
    QPointF offset(-l+pen.widthF(), -u+pen.widthF());
      QPointF beginPoint = mid(mouseMoveTrace[mouseMoveTrace.length()-2], mouseMoveTrace[mouseMoveTrace.length()-3])+offset;
      QLineF lastLine = QLineF(mouseMoveTrace[mouseMoveTrace.length()-2], mouseMoveTrace.back());
      lastLine.translate(offset);
      path.moveTo(mouseMoveTrace.front());
      if (mouseMoveTrace.length()>=2){
          path.lineTo(mid(mouseMoveTrace[0], mouseMoveTrace[1]));
      }
      for (int i = 0; i + 2 < mouseMoveTrace.length()-1; ++i) {
         path.quadTo(mouseMoveTrace[i+1],mid(mouseMoveTrace[i+1], mouseMoveTrace[i + 2]));
      }
      path.translate(offset);
    }
  controls.back()->onRelease(ev->position());
  } else if (mode == WBMODE_ERASER) {
  }
  update();
} /*}}}*/

bool Whiteboard::undo() { /*{{{*/
    if (nextScenePos == 0){
        return false;
    }
    --nextScenePos;
    update();
    return true;
} /*}}}*/
bool Whiteboard::redo() { /*{{{*/
    if (nextScenePos == controls.length()){
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
