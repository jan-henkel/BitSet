#ifndef BITSETSMAINWINDOW_H
#define BITSETSMAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "bitset.h"

namespace Ui {
class BitSetsMainWindow;
}

class BitSetsMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BitSetsMainWindow(QWidget *parent = 0);
    ~BitSetsMainWindow();
    void printToConsole(const QString &str);
    void printCommand(const QString &str);
    void printError(const QString &str);
    enum AssignmentOperator {AEQUALS,AAND,AOR,AXOR,AMINUS};
    enum BinaryOperator {BAND,BOR,BXOR,BMINUS};
    enum UnaryOperator {UNONE,UNOT};
    int parseSetExpression(QString &str,BitSet &set);
    int parseSingleSet(QString &str,BitSet &set);
    int parseUnaryOperator(QString &str);
    int parseBinaryOperator(QString &str);
    int parseSetIdentifier(QString &str);
    int parseAssignmentOperator(QString &str);
    int parseSetAssignment(QString &str);
    int parseCommand(QString &str);
    int parseSetSize(QString &str);
    int parseNewUpperBound(QString &str);
    //int parseFindElementByIndex(QString &str);
    void applyBinaryOperator(int binaryOperator,BitSet &set, const BitSet &other);
    void applyUnaryOperator(int unaryOperator,BitSet &set);
    void applyAssignmentOperator(int assignmentOperator,BitSet &set, const BitSet &other);

private slots:
    void receivedConsoleCommand(const QString &str);
    void quit();
private:
    Ui::BitSetsMainWindow *ui;
    BitSet *bitSets;
    int upperBound;
};

#endif // BITSETSMAINWINDOW_H
