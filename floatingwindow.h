#ifndef FLOATINGWINDOW_H
#define FLOATINGWINDOW_H

#include <QWidget>
#include "floatingsetting.h"
#include "mainwidget.h"
namespace Ui {
class FloatingWindow;
}

class FloatingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingWindow(Whiteboard *parent = nullptr);
    ~FloatingWindow();
    FloatingSetting* fs;
    void setTransparent(){
        setParent(nullptr);
  setWindowFlag(Qt::SubWindow);
  setWindowFlag(Qt::WindowStaysOnTopHint);
        show();
        fs->setTransparent();
    }
    void setUntransparent(){
        setParent(wb);
        show();
        fs->setUntransparent();
    }
protected:
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseMoveEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
private:
    Ui::FloatingWindow *ui;
    Whiteboard* wb;
};

#endif // FLOATINGWINDOW_H
