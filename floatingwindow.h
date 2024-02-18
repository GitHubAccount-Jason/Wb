#ifndef FLOATINGWINDOW_H
#define FLOATINGWINDOW_H

#include <QWidget>
#include "floatingsetting.h"
namespace Ui {
class FloatingWindow;
}

class FloatingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingWindow(QWidget *parent = nullptr);
    ~FloatingWindow();
    FloatingSetting* fs;
    void setTransparent(){
        hide();
        fs->setTransparent();
    }
    void setUntransparent(){
        show();
        fs->setUntransparent();
    }
protected:
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseMoveEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
private:
    Ui::FloatingWindow *ui;
};

#endif // FLOATINGWINDOW_H
