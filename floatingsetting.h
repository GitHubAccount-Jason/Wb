#ifndef FLOATINGSETTING_H
#define FLOATINGSETTING_H

#include <QWidget>

namespace Ui {
class FloatingSetting;
}

class FloatingSetting : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingSetting(QWidget *parent = nullptr);
    ~FloatingSetting();
    void setTransparent(){
        hide();
    }
    void setUntransparent(){
        // show();
    }

private:
    Ui::FloatingSetting *ui;
};

#endif // FLOATINGSETTING_H
