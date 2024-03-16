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
#include <QPointF>
using Line = QLineF;
using LineList = QList<Line>;
struct Circle{QPoint o;double r;};
#endif // WBDEFS_H
