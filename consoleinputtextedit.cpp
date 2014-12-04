#include "consoleinputtextedit.h"

ConsoleInputTextEdit::ConsoleInputTextEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void ConsoleInputTextEdit::keyPressEvent(QKeyEvent *e)
{
    if(e->key()==Qt::Key_Enter || e->key()==Qt::Key_Return)
    {
        outputLine(this->text());
        this->setText("");
    }
    else
        QLineEdit::keyPressEvent(e);
}
