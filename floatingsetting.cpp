#include "floatingsetting.h"
#include "ui_floatingsetting.h"

FloatingSetting::FloatingSetting(Whiteboard *parent)
    : QWidget(parent)
    , ui(new Ui::FloatingSetting)
{
    ui->setupUi(this);
    p=new FloatingSettingPen(parent);
    p->setAttribute(Qt::WA_TranslucentBackground, false);
    p->setWindowFlag(Qt::FramelessWindowHint);
    p->hide();
}

FloatingSetting::~FloatingSetting()
{
    delete ui;
}

void FloatingSetting::on_pbPen_pressed()
{
    p->move(QPoint(pos()).x(), (pos()).y()-100);
    p->show();
}

void FloatingSetting::moveEvent(QMoveEvent *ev){
    if (!p->isHidden()){
            p->move(QPoint(pos()).x(), (pos()).y()-100);
    }
}

void FloatingSetting::hideEvent(QHideEvent *event){
    if (!p->isHidden()){
        p->hide();
    }
}
