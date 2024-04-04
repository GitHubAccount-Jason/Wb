#include "floatingsetting.h"
#include "ui_floatingsetting.h"

FloatingSetting::FloatingSetting(QWidget* parent, Whiteboard *wb)
    : QWidget(parent),wb(wb)
    , ui(new Ui::FloatingSetting)
{

    ui->setupUi(this);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowFlag(Qt::SubWindow);
    setWindowFlag(Qt::FramelessWindowHint);
    p=new FloatingSettingPen(parent, wb);
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

void FloatingSetting::setTransparent(){
        QPoint p = pos();
        setParent(nullptr);
  setWindowFlag(Qt::SubWindow);
  setWindowFlag(Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_TranslucentBackground, true);
        move(p);
        this->p->setTransparent();
    }
void FloatingSetting::setUntransparent(){
        QPoint p = pos();
        setParent(wb);
  setWindowFlag(Qt::SubWindow, 0);
  setWindowFlag(Qt::WindowStaysOnTopHint, 0);
  setAttribute(Qt::WA_TranslucentBackground, true);
        move(p);
        this->p->setUntransparent();
        // show();
    }
