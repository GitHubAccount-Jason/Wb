#include "whiteboard.h"
#include "floatingwindow.h"
#include "wbdefs.cpp"

// nextScenePos == scene.size() => cur scene is tmpScene
// else cur scene is scene[nextScenePos]
Whiteboard::Whiteboard()
    : mode(WBMODE_PEN), pen(QColorConstants::Blue, 10, Qt::SolidLine,
                            Qt::RoundCap, Qt::RoundJoin),
      brush(QColorConstants::Red),
      pm(QSize(QGuiApplication::screens().at(0)->availableGeometry().size())) {
  pm.fill(Qt::transparent);
  isflush = true;

  nextScenePos = 0;
  fl = new FloatingWindow(this);
  qDebug() << "Whiteboard constructed\n";
  curTmp = new WbTmpFreePen(this);
  fl->move(0, QGuiApplication::screens().at(0)->availableGeometry().height() -
                  fl->height());
}
Whiteboard::~Whiteboard() { delete curTmp; }

void Whiteboard::paintEvent(QPaintEvent *ev) { /*{{{*/
  (void)ev;
  // for (int i = 0; i != nextScenePos; ++i){
  //     controls[i]->onPaint(pm);
  // }
  if (isflush) {
    pm.fill(Qt::transparent);
    if (nextScenePos != 0) {
      for (auto i : scenes[nextScenePos - 1]) {
        i->onPaint(pm);
      }
      curTmp->onPaint(pm);
    }
    isflush = false;
  } else {
    curTmp->onPaint(pm);
  }
  QPainter p(this);
  p.drawPixmap(QPoint(0, 0), pm);
  p.end();

} /*}}}*/
void Whiteboard::addControl(const std::shared_ptr<WbControl> &c) {
  if (scenes.length() != 0) {
    scenes.erase(scenes.begin() + nextScenePos, scenes.end());
  }
  if (scenes.size() == 0) {
    scenes.emplaceBack();
    scenes.back().emplaceBack(c);
  } else {
    scenes.emplaceBack(scenes.back());
    scenes.back().emplaceBack(c);
    // c->onPaint(pm);
  }
  nextScenePos = scenes.length();
}
void Whiteboard::mousePressEvent(QMouseEvent *ev) { /*{{{*/
  if (evinfo.init(ev) == false) {
    return;
  }

    curTmp->onPress(ev->position());

} /*}}}*/

void Whiteboard::mouseMoveEvent(QMouseEvent *ev) { /*{{{*/
  if (evinfo.refresh(ev) == false) {
    return;
  }
  if (evinfo.mouseMoveTrace.length() <= 3) {
    return;
  }
#define AVAILABLE (4)
    curTmp->onMove(ev->position());
  update();
} /*}}}*/

void Whiteboard::mouseReleaseEvent(QMouseEvent *ev) { /*{{{*/
  auto &mouseMoveTrace = evinfo.mouseMoveTrace;
  auto &l = evinfo.l, &r = evinfo.r, &u = evinfo.u, &d = evinfo.d;

  // isPressTriggered = false;
  if (mouseMoveTrace.size() == 1) {
    mouseMoveTrace.clear();
    return;
  }
    curTmp->onRelease(ev->position());
  update();
} /*}}}*/

bool Whiteboard::undo() { /*{{{*/
  if (nextScenePos == 0) {
    return false;
  }
  --nextScenePos;
  flush();
  update();
  return true;
} /*}}}*/
bool Whiteboard::redo() { /*{{{*/
  if (nextScenePos == scenes.length()) {
    return false;
  }
  ++nextScenePos;
  flush();
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
  fl->setParent(this, fl->windowFlags());
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
