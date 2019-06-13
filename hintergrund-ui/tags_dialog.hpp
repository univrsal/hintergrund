#pragma once

#include <QDialog>

namespace Ui {
class tags_dialog;
}

class tags_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit tags_dialog(QWidget *parent = nullptr);
    ~tags_dialog();

private:
    Ui::tags_dialog *ui;
};

