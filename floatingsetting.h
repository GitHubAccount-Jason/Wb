#ifndef FLOATINGSETTING_H
#define FLOATINGSETTING_H

#include <QWidget>
#include <QMouseEvent>
#include <floatingsettingpen.h>
#include "whiteboard.h"

namespace Ui {
class FloatingSetting;
}

class FloatingSetting : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingSetting(QWidget* parent, Whiteboard *wb);
    ~FloatingSetting();
    void setTransparent();
    void setUntransparent() ;   FloatingSettingPen* p;
    void moveEvent(QMoveEvent*);
    void hideEvent(QHideEvent*);

private slots:
    void on_pbPen_pressed();

    void on_pbEraser_pressed();

private:
    Ui::FloatingSetting *ui;
    Whiteboard*wb;
};

#endif // FLOATINGSETTING_H
