#ifndef CONSOLEINPUTTEXTEDIT_H
#define CONSOLEINPUTTEXTEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QKeyEvent>
#include <QString>

class ConsoleInputTextEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit ConsoleInputTextEdit(QWidget *parent = 0);

protected:
    virtual void keyPressEvent(QKeyEvent *e);

signals:
    void outputLine(QString line);
};

#endif // CONSOLEINPUTTEXTEDIT_H
