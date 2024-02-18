#include "settingspanel.h"
#include "ui_settingspanel.h"

SettingsPanel::SettingsPanel(QWidget *parent, QPen *pen, QBrush *brush,
                             int *mode)
    : QDialog(parent), ui(new Ui::SettingsPanel), penPen(pen), penBrush(brush), penMode(mode) {
  qDebug() << "SettingsPanel:mode=" << *mode;
  ui->setupUi(this);
  ui->penBtnPenColor->setAutoFillBackground(true);
  ui->penBtnPenColor->setFlat(true);
  ui->penBtnBrushColor->setAutoFillBackground(true);
  ui->penBtnBrushColor->setFlat(true);
  connect(ui->penBtnPenColor, &QPushButton::clicked, [this]() {
    QColorDialog *penColorDialog;
    penColorDialog = new QColorDialog(this);
    penColorDialog->setOption(QColorDialog::NoButtons, true);
    penColorDialog->setOption(QColorDialog::ShowAlphaChannel, true);
    penColorDialog->move(0, 0);
    penColorDialog->resize(200, 400);
    penColorDialog->setCurrentColor(penPen->color());
    connect(penColorDialog, &QColorDialog::currentColorChanged,
            [this](const QColor &color) {
              QPalette p = ui->penBtnPenColor->palette();
              p.setColor(QPalette::Button, color);
              ui->penBtnPenColor->setPalette(p);
              penPen->setColor(color);
            });
    penColorDialog->exec();
  });
  connect(ui->penBtnBrushColor, &QPushButton::clicked, [this]() {
    QColorDialog *brushColorDialog;
    brushColorDialog = new QColorDialog(this);
    brushColorDialog->setOption(QColorDialog::NoButtons, true);
    brushColorDialog->setOption(QColorDialog::ShowAlphaChannel, true);
    brushColorDialog->move(0, 0);
    brushColorDialog->resize(200, 400);
    brushColorDialog->setCurrentColor(penBrush->color());
    connect(brushColorDialog, &QColorDialog::currentColorChanged,
            [this](const QColor &color) {
              QPalette p = ui->penBtnBrushColor->palette();
              p.setColor(QPalette::Button, color);
              ui->penBtnBrushColor->setPalette(p);
              penBrush->setColor(color);
            });
    brushColorDialog->exec();
  });
  ui->penSpinBoxPenWidth->setRange(1, 16);
  ui->penSpinBoxPenWidth->setValue(penPen->width());
  connect(ui->penSpinBoxPenWidth, &QSpinBox::valueChanged,
          [this](int i) { penPen->setWidth(i); });
  ui->penComboBoxPenMode->addItem("自由画笔", QVariant(WBMODE_PEN));
  ui->penComboBoxPenMode->addItem("选择板擦", QVariant(WBMODE_ERASER));
  connect(ui->penComboBoxPenMode, &QComboBox::activated, [mode, this](int cur) {
    *mode = ui->penComboBoxPenMode->currentData().toInt();
  });
}

SettingsPanel::~SettingsPanel() { delete ui; }

void SettingsPanel::showEvent(QShowEvent *ev) {
  QPalette p1 = ui->penBtnPenColor->palette();
  p1.setColor(QPalette::Button, penPen->color());
  ui->penBtnPenColor->setPalette(p1);
  QPalette p2 = ui->penBtnBrushColor->palette();
  p2.setColor(QPalette::Button, penBrush->color());
  ui->penBtnBrushColor->setPalette(p2);
  ui->penComboBoxPenMode->setCurrentIndex(*penMode);
}
