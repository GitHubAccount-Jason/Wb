#ifndef FLOATINGSETTINGPEN_H
#define FLOATINGSETTINGPEN_H

#include <QWidget>
#include "whiteboard.h"

namespace Ui {
class floatingsettingpen;
}

class FloatingSettingPen : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingSettingPen(QWidget* parent, Whiteboard *wb);
    ~FloatingSettingPen();

private slots:

    void on_colorRed_pressed();

    void on_colorBlue_pressed();

    void on_colorGreen_pressed();


public:

    void setTransparent(){

        QPoint p = pos();
        setParent(nullptr);
  setWindowFlag(Qt::SubWindow);
  setWindowFlag(Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_TranslucentBackground, true);
        move(p);
    }
    void setUntransparent(){

        QPoint p = pos();
        setParent(wb);
  setWindowFlag(Qt::SubWindow, 0);
  setWindowFlag(Qt::WindowStaysOnTopHint, 0);
  setAttribute(Qt::WA_TranslucentBackground, true);
        move(p);
    }
private:
    Ui::floatingsettingpen *ui;
    Whiteboard *wb;

};

#endif // FLOATINGSETTINGPEN_H
