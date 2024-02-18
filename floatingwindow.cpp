#include "floatingwindow.h"
#include "ui_floatingwindow.h"
#include <QMouseEvent>
FloatingWindow::FloatingWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FloatingWindow),fs(new FloatingSetting(parent))
{
    ui->setupUi(this);
    setWindowFlags (Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    ui->pushButton->setAttribute(Qt::WA_TransparentForMouseEvents);
    fs->hide();
}

FloatingWindow::~FloatingWindow()
{
    delete ui;
}
static QPoint lastPoint;
void FloatingWindow::mousePressEvent(QMouseEvent* ev){
    if(ev->button() == Qt::LeftButton){
        this->raise(); //将此按钮移动到顶层显示
       lastPoint = ev->pos();
    }
}
void FloatingWindow::mouseMoveEvent(QMouseEvent* ev){
    if(ev->buttons() == Qt::LeftButton){

        this->move(this->mapToParent(ev->pos() - lastPoint));
        //防止按钮移出父窗口
        if(this->mapToParent(this->rect().topLeft()).x() <= 0){
            this->move(0, this->pos().y());
        }
        if(this->mapToParent(this->rect().bottomRight()).x() >= this->parentWidget()->rect().width()){
            this->move(this->parentWidget()->rect().width() - this->width(), this->pos().y());
        }
        if(this->mapToParent(this->rect().topLeft()).y() <= 0){
            this->move(this->pos().x(), 0);
        }
        if(this->mapToParent(this->rect().bottomRight()).y() >= this->parentWidget()->rect().height()){
            this->move(this->pos().x(), this->parentWidget()->rect().height() - this->height());
        }
        if (!fs->isHidden()){
            fs->move(QPoint(pos().x()+128, pos().y()));
        }
    }
}
void FloatingWindow::mouseReleaseEvent(QMouseEvent* ev){
    if (ev->pos()==lastPoint){
        fs->move(QPoint(pos().x()+128, pos().y()));
        if (fs->isHidden()) fs->show();
        else fs->hide();
    }
}
