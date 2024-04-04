#include "whiteboard.h"
#include "floatingwindow.h"
#include "wbdefs.cpp"

// nextScenePos == scene.size() => cur scene is tmpScene
// else cur scene is scene[nextScenePos]
Whiteboard::Whiteboard()
    : mode(WBMODE_PEN), pen(QColorConstants::Blue, 10, Qt::SolidLine,
                            Qt::RoundCap, Qt::RoundJoin),
    brush(QColorConstants::Red) ,pm(QSize(QGuiApplication::screens().at(0)->availableGeometry().size())){
    pm.fill(Qt::transparent);

  nextScenePos = 0;
  fl = new FloatingWindow(this);
  qDebug() << "Whiteboard constructed\n";
  curTmp = new WbTmpFreePen(this, pen,Qt::NoBrush);
  fl->move(0, QGuiApplication::screens().at(0)->availableGeometry().height()-fl->height());
}
Whiteboard::~Whiteboard() {
    for (auto i:controls){
        delete i;
    }
    delete curTmp;
}

void Whiteboard::paintEvent(QPaintEvent *ev) { /*{{{*/
  (void)ev;
    pm.fill(Qt::transparent);
    for (int i = 0; i != nextScenePos; ++i){
        controls[i]->onPaint(pm);
    }
    curTmp->onPaint(pm);
    QPainter p(this);
    p.drawPixmap(QPoint(0, 0), pm);
    p.end();

} /*}}}*/
void Whiteboard::addControl(WbControl* c){
  if (controls.length()!=0){
  controls.erase(controls.begin()+nextScenePos, controls.end());
  }
  controls.emplaceBack(c);
  nextScenePos = controls.length();
}
void Whiteboard::mousePressEvent(QMouseEvent *ev) { /*{{{*/
  if (evinfo.init(ev)==false){
      return;
  }

  if (mode == WBMODE_PEN) {
  // controls.push_back(new WbControlFreePen(this, pen,Qt::NoBrush));
  // controls.back()->onPress(ev->position());
      curTmp->onPress(ev->position());
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
      curTmp->onMove(ev->position());

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
    if (mouseMoveTrace.size() != 1) {
      update();
    }
      QPainterPath path;
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
      curTmp->onRelease(ev->position());
  // controls.back()->onRelease(ev->position());
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
    fl->setParent(this);
  fl->setUntransparent();
  setFocus();
}
void Whiteboard::setTransparent() {
    QPoint qp = fl->pos();
    fl->setParent(nullptr, fl->windowFlags());
    fl->setTransparent();
    fl->show();
    fl->move(qp);
}
