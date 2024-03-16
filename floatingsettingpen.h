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
    explicit FloatingSettingPen(Whiteboard *parent = nullptr);
    ~FloatingSettingPen();

private slots:

    void on_colorRed_pressed();

    void on_colorBlue_pressed();

    void on_colorGreen_pressed();

private:
    Ui::floatingsettingpen *ui;
    Whiteboard *wb;

};

#endif // FLOATINGSETTINGPEN_H
