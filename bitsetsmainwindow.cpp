#include "bitsetsmainwindow.h"
#include "ui_bitsetsmainwindow.h"
#include "bitset.h"
#include <QString>

BitSetsMainWindow::BitSetsMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BitSetsMainWindow)
{
    ui->setupUi(this);
}

BitSetsMainWindow::~BitSetsMainWindow()
{
    delete ui;
}

void BitSetsMainWindow::on_pushButton_clicked()
{
    BitSet bitSet(90);
    bitSet.add(1);
    bitSet.add(25);
    bitSet.add(7);
    bitSet.add(87);
    bitSet.remove(7);
    bitSet.complement();
    bitSet.removeM(3,36,20,9);
    bitSet.remove(0);
    ui->textEdit->setText(bitSet);
    QString str="{";
    for(int i=0;i<bitSet.size();++i)
        str+=QString::number(bitSet.indexOf(bitSet[i]))+((i<bitSet.size()-1)?",":"}");
    ui->textEdit->append(str);
    ui->textEdit->append(QString::number(bitSet.lowest()));
}
