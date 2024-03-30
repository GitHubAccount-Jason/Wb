#ifndef WBDEFS_H
#define WBDEFS_H
#define WBMODE_TRANSPARENT (-1)
#define WBMODE_PEN (0)
#define WBMODE_ERASER (1)

#define WBOBJTYPE_EXTRA (-1)
#define WBOBJTYPE_LINE (1)
#define WBOBJTYPE_LINELIST (2)
#define WBOBJTYPE_CIRCLE (3)
#define WBOBJTYPE_NONE (0)
#include <QLineF>
#include <QList>
#include <QLine>
#include <QPoint>
#include <any>
#include <QPainterPath>
#include <QPainter>
#include <QPaintDevice>
#include <QPointF>
#include <QMouseEvent>
#include <QPen>
#include <QBrush>
#include <QWidget>
using Line = QLineF;
using LineList = QList<Line>;
using Circle=struct{QPointF o; qreal r;};
class Whiteboard;

#define mid(a, b) (QPointF((a.x() + b.x()) / 2, (a.y() + b.y()) / 2))

template <class A> A min(const A &a, const A &b) { return std::min(a, b); }
template <class A> A min(const A &a, const A &b, const A &L...) {
  return min(min(a, b), L);
}
template <class A> A max(const A &a, const A &b) { return std::max(a, b); }
template <class A> A max(const A &a, const A &b, const A &L...) {
  return max(max(a, b), L);
}
inline void drawPatchPoint(QPainter *painter, const QLineF &path,
                           const QPointF &beginPoint, const QPen &pen) {
  qreal temp = (pen.widthF() - 3) / 100.;
  int k = 0;
  QPainterPath p;
  p.moveTo(beginPoint);
  p.quadTo(path.p1(), path.p2());
  for (double i = 0; i < path.length(); i += 1) {
    k++;

    painter->setPen(QPen(pen.color(),
                         pen.widthF() - temp * (k / path.length() * 100),
                         pen.style(), pen.capStyle(), pen.joinStyle()));
    painter->drawPoint(p.pointAtPercent(i / path.length()));
  }
}
inline double disPoints(const QPointF &p1, const QPointF &p2) {
  return sqrt(pow(p1.x() - p2.x(), 2) + pow(p1.y() - p2.y(), 2));
}
inline double disCircles(const Circle &c1, const Circle &c2) {
  return disPoints(c1.o, c2.o) - c1.r - c2.r;
}
inline double disPointCircle(const QPointF &p1, const Circle &c1) {
  return fabs(disPoints(p1, c1.o) - c1.r);
}
inline double disPointLine(const QPointF &p1, const QLineF &l1) {
  double A = l1.y2() - l1.y1();
  double B = l1.x1() - l1.x2();
  double C = l1.x2() * l1.y1() - l1.x1() * l1.y2();
  return fabs(A * p1.x() + B * p1.y() + C) / sqrt(A * A + B * B);
}
inline double disLineCircle(const QLineF &l, const Circle &c) {
  return disPointLine(c.o, l) - c.r;
}
inline QPointF mapToRect(const QPointF &p, const QRect &r, qreal w) {
  return QPointF(p.x() - r.left() + w, p.y() - r.top() + w);
}
inline QLineF mapToRect(const QLineF &l, const QRect &r, qreal w) {
  return QLineF(mapToRect(l.p1(), r, w), mapToRect(l.p2(), r, w));
}
inline QPointF mapToPoint(const QPointF &p, const QPointF &ori, qreal w) {
  return QPointF(p.x() - ori.x() + w, p.y() - ori.y() + w);
}
inline QLineF mapToPoint(const QLineF &l, const QPointF &ori, qreal w) {
  return QLineF(mapToPoint(l.p1(), ori, w), mapToPoint(l.p2(), ori, w));
}
class WbConfig{
public:
    bool fitLine = true;
    bool fitCircle = false;
};
static  int classid = 1;
template<class A>
struct TypeIndexer{
    static int index;
};
template<class A>
int TypeIndexer<A>::index = classid++;

class WbFunction{
private:
    QString name;
    std::any function;
    int typeIndex_=0;
public:
    WbFunction(){}
    template<class A>
    WbFunction(const QString&name, const std::function<A>& function):name(name), function(function), typeIndex_(TypeIndexer<A>::index){}
    int typeIndex()const{return typeIndex_;}
    template<class A>
    bool check(const QString& name)const{
        return name==name&&TypeIndexer<A>::index==typeIndex_;
    }
    template<class A>
    std::optional<std::function<A>> get(const QString &name)const{
        if (TypeIndexer<A>::index == typeIndex_ && name == this->name){
            return std::any_cast<std::function<A>>(function);
        }
    }
};

class WbControl{
public:
    WbControl(Whiteboard*wb, const QPen&pen, const QBrush&brush):wb(wb), pen_(pen), brush_(brush){}
    virtual void onPress(const QPointF& p)=0;
    virtual void onMove(const QPointF&p)=0;
    virtual void onRelease(const QPointF&p)=0;
    virtual void onPaint(QPixmap&w)=0;
    virtual ~WbControl(){}
    template<class A>
    bool checkFn(const QString&name){
        for (const auto& i:fns){
            if (i.check<A>(name)){return true;}
        }
        return false;
    }
    template<class A>
    std::optional<std::function<A>> getFn(const QString&s){
        for (const auto& i:fns){
            if (i.get<A>(s).has_value()){return i.get<A>(s).value();}
        }
    }
protected:
    Whiteboard *wb;
    QList<WbFunction> fns;
    QPen pen_;
    QBrush brush_;

};
class WbControlFreePen;
#endif // WBDEFS_H
