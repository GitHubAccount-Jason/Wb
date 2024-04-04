#include "floatingsettingpen.h"
#include "ui_floatingsettingpen.h"
#include "whiteboard.h"
FloatingSettingPen::FloatingSettingPen(QWidget* parent, Whiteboard *wb)
    : QWidget(parent), ui(new Ui::floatingsettingpen), wb(wb) {
  ui->setupUi(this);
  setAttribute(Qt::WA_StyledBackground);
    setWindowFlag(Qt::WindowStaysOnTopHint);
  // setAttribute(Qt::WA_TranslucentBackground, false);
    hide();
  setWindowFlags (Qt::FramelessWindowHint);
}

FloatingSettingPen::~FloatingSettingPen() { delete ui; }

void FloatingSettingPen::on_colorRed_pressed() { wb->pen.setColor(Qt::red); }

void FloatingSettingPen::on_colorBlue_pressed() { wb->pen.setColor(Qt::blue); }

void FloatingSettingPen::on_colorGreen_pressed() {
  wb->pen.setColor(Qt::green);
}
