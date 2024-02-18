#ifndef WHITEBOARD_H
#define WHITEBOARD_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include "wbdefs.h"
#include "floatingwindow.h"


using Line = QLineF;
using LineList = QList<Line>;
class Whiteboard : public QWidget {
public:
  int mode;

  class WbObject {
  public:
    bool isVisible;
    int type;

    QPen pen;
    QBrush brush;
    void *extra;
    WbObject() : type(WBOBJTYPE_EXTRA), extra(nullptr) {}
    WbObject(const WbObject &wo) {
      isVisible = wo.isVisible;
      pen = wo.pen;
      brush = wo.brush;
      type = wo.type;
      switch (wo.type) {
      case WBOBJTYPE_LINE:
        extra = new Line(*(Line *)wo.extra);
        break;
      case WBOBJTYPE_LINELIST:
        extra = new LineList(*(LineList *)wo.extra);
      default:
        break;
      };
    }
    WbObject(const Line &l, const QPen &pen, const QBrush &brush)
        : isVisible(true), type(WBOBJTYPE_LINE), pen(pen), brush(brush),
          extra(new Line(l)) {}
    WbObject(const LineList &l, const QPen &pen, const QBrush &brush)
        : isVisible(true), type(WBOBJTYPE_LINELIST), pen(pen), brush(brush),
          extra(new LineList(l)) {}
    WbObject &operator=(const WbObject &wo) {
      isVisible = wo.isVisible;
      pen = wo.pen;
      brush = wo.brush;
      type = wo.type;
      switch (wo.type) {
      case WBOBJTYPE_LINE:
        extra = new Line(*(Line *)wo.extra);
        break;
      case WBOBJTYPE_LINELIST:
        extra = new LineList(*(LineList *)wo.extra);
      default:
        break;
      }
      return *this;
    }
    Line &line() { return *(Line *)extra; }
    const Line &line() const { return *(Line *)extra; }
    LineList &linelist() { return *(LineList *)extra; }
    const LineList &linelist() const { return *(LineList *)extra; }
    ~WbObject() {
      if (extra != nullptr) {
        switch (type) {
        case WBOBJTYPE_LINE:
          delete (Line *)extra;
          break;
        case WBOBJTYPE_LINELIST:
          delete (LineList *)extra;
        default:
          break;
        }
      }
    }
  };
  class WbScene{
      QPixmap pm;
      QPoint p;
      int tp;
  public:
      [[deprecated]]
      WbScene(const WbObject& obj, const QRect& rect):pm(rect.size()), p(rect.topLeft()),tp(obj.type){
          pm.fill(Qt::transparent);
          QPainter painter(&pm);
          painter.setCompositionMode(QPainter::CompositionMode_Source);
          painter.setRenderHint(QPainter::Antialiasing);
          switch (obj.type) {
          case WBOBJTYPE_LINELIST:
              painter.setPen(obj.pen);
              painter.setBrush(obj.brush);
              for (const auto &l:obj.linelist()){
                    painter.drawLine(l);
              }
              break;
          default:
              break;
          }
          painter.end();

      }
      WbScene(const LineList& l,const QPen& pen, const QBrush&brush, const QRect&rect):pm((pen.width()*2+rect.width()),(pen.width()*2+rect.height())), p(QPoint(rect.left()-pen.width(), rect.top()-pen.width())),tp(WBOBJTYPE_LINELIST){
          // qDebug()<<pm.size();
          pm.fill(Qt::transparent);
          QPainter painter;
          painter.begin(&pm);
          painter.setCompositionMode(QPainter::CompositionMode_Source);
          painter.setRenderHint(QPainter::Antialiasing);
          painter.setPen(pen);
          painter.setBrush(brush);
          for (const auto &l:l){
              painter.drawLine(Line(QPointF(l.p1().x()-rect.left()+pen.width(), l.p1().y()-rect.top()+pen.width()), QPointF(l.p2().x()-rect.left()+pen.width(), l.p2().y()-rect.top()+pen.width())));
          }
          painter.end();
      }
      void paint(QPainter& p){
          p.drawPixmap(this->p, pm);
      }
  };
  QPen pen;
  QBrush brush;
    FloatingWindow* fl;
  Whiteboard();
  ~Whiteboard();
  bool undo();
  bool redo();
  void changeMode(int mode);
  void setTransparent(){
      fl->setTransparent();
  }
  void setUntransparent(){
      fl->setUntransparent();
      setFocus();
  }
protected:
  virtual void paintEvent(QPaintEvent *ev) override;
  virtual void mousePressEvent(QMouseEvent *ev) override;
  virtual void mouseMoveEvent(QMouseEvent *ev) override;
  virtual void mouseReleaseEvent(QMouseEvent *ev) override;

private:
  QList<WbObject> tmpScene;
  QList<WbScene> opSc;
  QList<QPointF> mouseMoveTrace;
  qsizetype nextScenePos;

  int l=INT_MAX, r=INT_MIN, u=INT_MAX, d=INT_MIN;
  // If I press on the button and move the mouse, than ignore this move or release
  bool isPressTriggered=false;
};

#endif // WHITEBOARD_H
