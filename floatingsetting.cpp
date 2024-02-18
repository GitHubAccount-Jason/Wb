#include "floatingsetting.h"
#include "ui_floatingsetting.h"

FloatingSetting::FloatingSetting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FloatingSetting)
{
    ui->setupUi(this);
}

FloatingSetting::~FloatingSetting()
{
    delete ui;
}
