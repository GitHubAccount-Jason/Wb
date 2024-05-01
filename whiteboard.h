#ifndef WHITEBOARD_H
#define WHITEBOARD_H

#include "wbdefs.h"
#include <concepts>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineF>
#include <QMouseEvent>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
class FloatingWindow;
class MainWidget;
class Whiteboard : public QWidget {
public:
  int mode;
  WbConfig gConfig;

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

  struct EvInfo {
    int l;
    int r;
    int u;
    int d;
    QList<QPointF> mouseMoveTrace;
    double traceLen;
    double traceSpeed;

  private:
    bool isPressTriggered_;

  public:
    void setPressTriggered() { isPressTriggered_ = true; }
    bool isPressTriggered() const { return isPressTriggered_; }
    EvInfo() { clear(); }
    bool refresh(const QMouseEvent *ev) {
      if (isPressTriggered_ == false) {
        return false;
      }

      traceLen += disPoints(mouseMoveTrace.back(), ev->position());
      traceSpeed = traceLen / (mouseMoveTrace.size() - 1);
      l = min(l, ev->pos().x());
      r = max(r, ev->pos().x());
      u = min(u, ev->pos().y());
      d = max(d, ev->pos().y());
      mouseMoveTrace.push_back(ev->position());
      return true;
    }
    bool init(QMouseEvent *pressEv) {
      clear();
      if (pressEv->button() != Qt::LeftButton) {
        return false;
      }
      l = min(l, pressEv->pos().x());
      r = max(r, pressEv->pos().x());
      u = min(u, pressEv->pos().y());
      d = max(d, pressEv->pos().y());
      mouseMoveTrace.push_back(pressEv->position());
      isPressTriggered_ = true;
      return true;
    }
    void clear() {
      l = INT_MAX;
      r = INT_MIN;
      u = INT_MAX;
      d = INT_MIN;
      mouseMoveTrace.clear();
      traceLen = 0;
      traceSpeed = 0;
      isPressTriggered_ = false;
    }
  };
  EvInfo evinfo;
  void addControl(const std::shared_ptr<WbControl>& c);
  MainWidget* mainwidget() {return (MainWidget*)parent();}
  const MainWidget* mainwidget() const {return (const MainWidget*)parent();}
  using WbScene = QList<std::shared_ptr<WbControl>>;
  QList<WbScene> scenes;
  void flush(){isflush = true;}
  WbTmp* curTmp;
  template<class T>
    requires std::derived_from<T, WbTmp>
  void changeTmp(){
      delete curTmp;
      curTmp = new T(this);

  }
  void store(){

  if (scenes.length() != 0) {
    scenes.erase(scenes.begin() + nextScenePos, scenes.end());
  }
  if (scenes.size() == 0) {
    scenes.emplaceBack();
  } else {
    scenes.emplaceBack(scenes.back());
  }
  nextScenePos = scenes.length();
  }
  protected:
  virtual void paintEvent(QPaintEvent *ev) override;
  virtual void mousePressEvent(QMouseEvent *ev) override;
  virtual void mouseMoveEvent(QMouseEvent *ev) override;
  virtual void mouseReleaseEvent(QMouseEvent *ev) override;

private:
  qsizetype nextScenePos;

  QPixmap pm;


  bool isflush;



};

#endif // WHITEBOARD_H
