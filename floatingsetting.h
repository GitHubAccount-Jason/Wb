#ifndef FLOATINGSETTING_H
#define FLOATINGSETTING_H

#include <QWidget>
#include <QMouseEvent>
#include <floatingsettingpen.h>

namespace Ui {
class FloatingSetting;
}

class FloatingSetting : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingSetting(Whiteboard *parent = nullptr);
    ~FloatingSetting();
    void setTransparent(){
        hide();
    }
    void setUntransparent(){
        // show();
    }
    FloatingSettingPen* p;
    void moveEvent(QMoveEvent*);
    void hideEvent(QHideEvent*);

private slots:
    void on_pbPen_pressed();

private:
    Ui::FloatingSetting *ui;
};

#endif // FLOATINGSETTING_H
