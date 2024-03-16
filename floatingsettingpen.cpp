#include "floatingsettingpen.h"
#include "ui_floatingsettingpen.h"
#include "whiteboard.h"
FloatingSettingPen::FloatingSettingPen(Whiteboard *parent)
    : QWidget(parent), ui(new Ui::floatingsettingpen), wb(parent) {
  ui->setupUi(this);
  setAttribute(Qt::WA_StyledBackground);
  // setAttribute(Qt::WA_TranslucentBackground, false);
  // setWindowFlags (Qt::FramelessWindowHint);
}

FloatingSettingPen::~FloatingSettingPen() { delete ui; }

void FloatingSettingPen::on_colorRed_pressed() { wb->pen.setColor(Qt::red); }

void FloatingSettingPen::on_colorBlue_pressed() { wb->pen.setColor(Qt::blue); }

void FloatingSettingPen::on_colorGreen_pressed() {
  wb->pen.setColor(Qt::green);
}
