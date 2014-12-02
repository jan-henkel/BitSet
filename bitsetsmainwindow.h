#ifndef BITSETSMAINWINDOW_H
#define BITSETSMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class BitSetsMainWindow;
}

class BitSetsMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BitSetsMainWindow(QWidget *parent = 0);
    ~BitSetsMainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::BitSetsMainWindow *ui;
};

#endif // BITSETSMAINWINDOW_H
