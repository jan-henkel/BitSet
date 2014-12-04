#include "bitsetsmainwindow.h"
#include "ui_bitsetsmainwindow.h"
#include <QTimer>

#define FORMATTING_COMMAND_PRE QString("<font color='blue'><i>")
#define FORMATTING_COMMAND_POST QString("</i></font>")
#define FORMATTING_HIGHLIGHT_PRE QString("<font color='green'><i>")
#define FORMATTING_HIGHLIGHT_POST QString("</i></font>")
#define FORMATTING_ERROR_PRE QString("<font color='red'>")
#define FORMATTING_ERROR_POST QString("</font>")
#define UPPER_BOUND_INITIAL 100
#define MAX_UPPER_BOUND 1024

BitSetsMainWindow::BitSetsMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BitSetsMainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->consoleInputTextEdit,SIGNAL(outputLine(QString)),this,SLOT(receivedConsoleCommand(QString)));
    upperBound=UPPER_BOUND_INITIAL;
    BitSet prototype(upperBound);
    bitSets=new BitSet[26];
    for(int i=0;i<26;++i)
        bitSets[i]=prototype;
    printToConsole("Usage: Perform simple set operations on predefined sets "+FORMATTING_HIGHLIGHT_PRE+"A,B,...,Z"+FORMATTING_HIGHLIGHT_POST+".");
    printToConsole("Sets "+FORMATTING_HIGHLIGHT_PRE+"A"+FORMATTING_HIGHLIGHT_POST+" through "+FORMATTING_HIGHLIGHT_PRE+"Z"+FORMATTING_HIGHLIGHT_POST+" are initially empty. They hold a maximum of "+FORMATTING_HIGHLIGHT_PRE+"#max"+FORMATTING_HIGHLIGHT_POST+" numbers out of {0,...,#max-1}. "+FORMATTING_HIGHLIGHT_PRE+"#max"+FORMATTING_HIGHLIGHT_POST+" is initially set to "+QString::number(upperBound)+".");
    printToConsole("Sets may be modified thusly: "+FORMATTING_COMMAND_PRE+"IDENTIFIER ASSIGNMENT_OPERATOR SET_EXPRESSION"+FORMATTING_COMMAND_POST+", whereby:");
    printToConsole("1. "+FORMATTING_HIGHLIGHT_PRE+"IDENTIFIER"+FORMATTING_HIGHLIGHT_POST+" is any upper case character from "+FORMATTING_HIGHLIGHT_PRE+"A"+FORMATTING_HIGHLIGHT_POST+" to "+FORMATTING_HIGHLIGHT_PRE+"Z"+FORMATTING_HIGHLIGHT_POST+",");
    printToConsole("2. "+FORMATTING_HIGHLIGHT_PRE+"ASSIGNMENT_OPERATOR"+FORMATTING_HIGHLIGHT_POST+" is "+FORMATTING_HIGHLIGHT_PRE+"="+FORMATTING_HIGHLIGHT_POST+" (assign right hand side to left hand side),<br>\
    "+FORMATTING_HIGHLIGHT_PRE+"|="+FORMATTING_HIGHLIGHT_POST+", "+FORMATTING_HIGHLIGHT_PRE+"+="+FORMATTING_HIGHLIGHT_POST+" (OR-operation, unite right hand side with left hand side), "+FORMATTING_HIGHLIGHT_PRE+"&="+FORMATTING_HIGHLIGHT_POST+" (AND-operation, intersect right hand side and left hand side),<br>\
    "+FORMATTING_HIGHLIGHT_PRE+"-="+FORMATTING_HIGHLIGHT_POST+" (subtract right hand side from left hand side) or "+FORMATTING_HIGHLIGHT_PRE+"^="+FORMATTING_HIGHLIGHT_POST+" (XOR-operation, symmetric difference of right hand side and left hand side).");
    printToConsole("3. "+FORMATTING_HIGHLIGHT_PRE+"SET_EXPRESSION"+FORMATTING_HIGHLIGHT_POST+" is any sequence alternating between sets and binary operators (OR: "+FORMATTING_HIGHLIGHT_PRE+"|"+FORMATTING_HIGHLIGHT_POST+","+FORMATTING_HIGHLIGHT_PRE+"+"+FORMATTING_HIGHLIGHT_POST+", AND: "+FORMATTING_HIGHLIGHT_PRE+"&"+FORMATTING_HIGHLIGHT_POST+", MINUS: "+FORMATTING_HIGHLIGHT_PRE+"-"+FORMATTING_HIGHLIGHT_POST+", XOR: "+FORMATTING_HIGHLIGHT_PRE+"^"+FORMATTING_HIGHLIGHT_POST+").<br>\
    Sets may be given either by identifiers "+FORMATTING_HIGHLIGHT_PRE+"A"+FORMATTING_HIGHLIGHT_POST+" through "+FORMATTING_HIGHLIGHT_PRE+"Z"+FORMATTING_HIGHLIGHT_POST+" or an expression "+FORMATTING_HIGHLIGHT_PRE+"{x_1,...,x_n}"+FORMATTING_HIGHLIGHT_POST+",<br>\
    whereby x_i denote the elements. Sets can be preceded by the unary operator "+FORMATTING_COMMAND_PRE+"~"+FORMATTING_COMMAND_POST+" (NOT-operator, complement with respect to {0,...,#max-1})");
    printToConsole("All binary operators are resolved from left to right (no operator precedence, binding levels).");
    printToConsole("Entering "+FORMATTING_COMMAND_PRE+"#SET_EXPRESSION"+FORMATTING_COMMAND_POST+" yields the number of elements of the set described by "+FORMATTING_HIGHLIGHT_PRE+"SET_EXPRESSION."+FORMATTING_HIGHLIGHT_POST);
    printToConsole(FORMATTING_COMMAND_PRE+"#max=NUMBER"+FORMATTING_COMMAND_POST+" defines a new strict upper bound. "+FORMATTING_HIGHLIGHT_PRE+"NUMBER"+FORMATTING_HIGHLIGHT_POST+" must be less than or equal to "+QString::number(MAX_UPPER_BOUND)+". "+FORMATTING_COMMAND_PRE+"#max"+FORMATTING_COMMAND_POST+" yields the current strict upper bound.");
    printToConsole("Entering an "+FORMATTING_COMMAND_PRE+"IDENTIFIER"+FORMATTING_COMMAND_POST+" yields the content of the corresponding set.");
    printToConsole("<br>Example:<br><br>"+FORMATTING_COMMAND_PRE+"A={1,2,9}"+FORMATTING_COMMAND_POST+"<br>"+FORMATTING_COMMAND_PRE+"B={5,6,9}"+FORMATTING_COMMAND_POST+"<br>"+FORMATTING_COMMAND_PRE+"C=A^B+{71}"+FORMATTING_COMMAND_POST+"<br><br>yields C={1,2,5,6,71}.<br>"+FORMATTING_COMMAND_PRE+"#C"+FORMATTING_COMMAND_POST+" yields 5<br><br>"+FORMATTING_COMMAND_PRE+"D=~C&{1,2,3}"+FORMATTING_COMMAND_POST+" yields D={3}");
}

BitSetsMainWindow::~BitSetsMainWindow()
{
    delete ui;
}

void BitSetsMainWindow::printToConsole(const QString &str)
{
    ui->consoleOutputTextEdit->append(str);
}

void BitSetsMainWindow::printError(const QString &str)
{
    ui->consoleOutputTextEdit->append(FORMATTING_ERROR_PRE+"Error: "+str+FORMATTING_ERROR_POST);
}

void BitSetsMainWindow::printCommand(const QString &str)
{
    ui->consoleOutputTextEdit->append(FORMATTING_COMMAND_PRE+str+FORMATTING_COMMAND_POST);
}

int BitSetsMainWindow::parseSetIdentifier(QString &str)
{
    if(str.length()==0)
    {
        printError("Set identifier expected.");
        return -1;
    }
    if(str[0]>='A' && str[0]<='Z')
    {
        int result=str[0].unicode()-((QChar)'A').unicode();
        str.remove(0,1);
        return result;
    }
    else
    {
        printError("Set identifier expected.");
        return -1;
    }
}

int BitSetsMainWindow::parseAssignmentOperator(QString &str)
{
    if(str.length()==0)
    {
        printError("Assignment operator expected.");
        return -1;
    }
    int result=-1;
    switch(str[0].toLatin1())
    {
    case '=':
        result=AssignmentOperator::AEQUALS;
        break;
    case '+':
    case '|':
        result=AssignmentOperator::AOR;
        break;
    case '&':
        result=AssignmentOperator::AAND;
        break;
    case '^':
        result=AssignmentOperator::AXOR;
        break;
    case '-':
        result=AssignmentOperator::AMINUS;
        break;
    default:
        printError("Assignment operator expected.");
        return -1;
    }
    str.remove(0,1);
    if(result!=AEQUALS)
    {
        if(str.length()==0 || str[0]!='=')
        {
            printError("Assignment operator expected.");
            return -1;
        }
        else
        {
            str.remove(0,1);
        }
    }
    return result;
}

int BitSetsMainWindow::parseBinaryOperator(QString &str)
{
    if(str.length()==0)
    {
        printError("Binary operator expected.");
        return -1;
    }
    int result=-1;
    switch(str[0].toLatin1())
    {
    case '&':
        result=BinaryOperator::BAND;
        break;
    case '|':
    case '+':
        result=BinaryOperator::BOR;
        break;
    case '^':
        result=BinaryOperator::BXOR;
        break;
    case '-':
        result=BinaryOperator::BMINUS;
        break;
    }
    if(result==-1)
    {
        printError("Unary operator expected.");
        return -1;
    }
    str.remove(0,1);
    return result;
}

int BitSetsMainWindow::parseUnaryOperator(QString &str)
{
    if(str.length()==0)
    {
        printError("Unary operator expected.");
        return -1;
    }
    int result=-1;
    switch(str[0].toLatin1())
    {
    case '~':
        result=UnaryOperator::UNOT;
        break;
    default:
        result=UnaryOperator::UNONE;
        break;
    }
    if(result!=UnaryOperator::UNONE)
        str.remove(0,1);
    return result;
}

int BitSetsMainWindow::parseSingleSet(QString &str, BitSet &set)
{
    BitSet parsedSet(upperBound);
    int unaryOperator;
    if(str.length()==0)
    {
        printError("Set expected.");
        return -1;
    }
    unaryOperator=parseUnaryOperator(str);
    if(str.length()==0)
    {
        printError("Set expected.");
        return -1;
    }
    if(str[0]>='A' && str[0]<='Z')
    {
        parsedSet=bitSets[str[0].unicode()-((QChar)'A').unicode()];
        applyUnaryOperator(unaryOperator,parsedSet);
        set=parsedSet;
        str.remove(0,1);
        return 0;
    }
    if(str[0]=='{')
    {
        str.remove(0,1);
        int end=str.indexOf('}');
        int comma;
        int num;
        bool numOk;
        QString substr;
        if(end==-1)
        {
            printError("Set expected.");
            return -1;
        }
        if(end==0)
        {
            str.remove(0,1);
            set=parsedSet;
            return 0;
        }
        comma=str.indexOf(',');
        while(comma<end && comma>=0)
        {
            substr=str.left(comma);
            num=substr.toInt(&numOk);
            if(numOk && num>=0 && num<upperBound)
            {
                parsedSet.add(num);
                str.remove(0,comma+1);
                comma=str.indexOf(',');
                end=str.indexOf('}');
            }
            else
            {
                printError("Number within legal range expected.");
                return -1;
            }
        }
        substr=str.left(end);
        num=substr.toInt(&numOk);
        if(numOk && num>=0 && num<upperBound)
        {
            parsedSet.add(num);
            str.remove(0,end+1);
        }
        else
        {
            printError("Number within legal range expected.");
            return -1;
        }
        applyUnaryOperator(unaryOperator,parsedSet);
        set=parsedSet;
        return 0;
    }
    printError("Set expected.");
    return -1;
}

void BitSetsMainWindow::applyBinaryOperator(int binaryOperator, BitSet &set, const BitSet &other)
{
    switch(binaryOperator)
    {
    case BinaryOperator::BOR:
        set|=other;
        break;
    case BinaryOperator::BAND:
        set&=other;
        break;
    case BinaryOperator::BXOR:
        set^=other;
        break;
    case BinaryOperator::BMINUS:
        set-=other;
        break;
    }
}

void BitSetsMainWindow::applyUnaryOperator(int unaryOperator, BitSet &set)
{
    switch(unaryOperator)
    {
    case UnaryOperator::UNOT:
        set.complement();
        break;
    }
}

int BitSetsMainWindow::parseSetExpression(QString &str, BitSet &set)
{
    BitSet newSet,newSet1;
    int ok;
    int binaryOperator;
    if(str.length()==0)
    {
        printError("Set expression expected.");
        return -1;
    }
    ok=parseSingleSet(str,newSet);
    if(ok==-1)
        return -1;
    if(str.length()==0)
    {
        set=newSet;
        return 0;
    }
    while(str.length()>0)
    {
        binaryOperator=parseBinaryOperator(str);
        if(binaryOperator==-1)
            return -1;
        ok=parseSingleSet(str,newSet1);
        if(ok==-1)
            return -1;
        applyBinaryOperator(binaryOperator,newSet,newSet1);
    }
    set=newSet;
    return 0;
}

void BitSetsMainWindow::applyAssignmentOperator(int assignmentOperator, BitSet &set, const BitSet &other)
{
    switch(assignmentOperator)
    {
    case AssignmentOperator::AEQUALS:
        set=other;
        break;
    case AssignmentOperator::AOR:
        set|=other;
        break;
    case AssignmentOperator::AAND:
        set&=other;
        break;
    case AssignmentOperator::AXOR:
        set^=other;
        break;
    case AssignmentOperator::AMINUS:
        set-=other;
        break;
    }
}

int BitSetsMainWindow::parseSetAssignment(QString &str)
{
    if(str.length()==0)
    {
        printError("Input expected.");
        return -1;
    }
    BitSet newSet;
    int leftHandSideIndex=parseSetIdentifier(str);
    if(leftHandSideIndex==-1)
        return -1;
    int assignmentOperator=parseAssignmentOperator(str);
    if(assignmentOperator==-1)
        return -1;
    int ok;
    ok=parseSetExpression(str,newSet);
    if(ok==-1)
        return -1;
    applyAssignmentOperator(assignmentOperator,bitSets[leftHandSideIndex],newSet);
    printToConsole(QString(QChar(((QChar)'A').unicode()+leftHandSideIndex))+"="+bitSets[leftHandSideIndex]);
    return 0;
}

int BitSetsMainWindow::parseSetSize(QString &str)
{
    if(str.length()==0 || str[0]!='#')
    {
        printError("'Size of set' expression expected.");
        return -1;
    }
    str.remove(0,1);
    QString singleSetIdentifier=(str.length()==1)?QString(str[0]):"";
    BitSet set;
    int ok;
    ok=parseSetExpression(str,set);
    if(ok==-1)
    {
        printError("'Size of set' expression expected.");
        return -1;
    }
    QString str1="#";
    str1=str1+(singleSetIdentifier!=""?singleSetIdentifier:set)+"="+QString::number(set.size());
    printToConsole(str1);
    return 0;
}

int BitSetsMainWindow::parseNewUpperBound(QString &str)
{
    if(str.indexOf("#max=")!=0)
    {
        printError("Assignment of new upper bound expected.");
        return -1;
    }
    str.remove(0,5);
    int num;
    bool ok;
    num=str.toInt(&ok);
    if(!ok || num<=0 || num>MAX_UPPER_BOUND)
    {
        printError("Upper bound in range 1 to "+QString::number(MAX_UPPER_BOUND)+"expected.");
        return -1;
    }
    else
    {
        for(int i=0;i<26;++i)
            bitSets[i].resize(num,true);
        upperBound=num;
        printToConsole("New upper bound: #max="+QString::number(num));
        return 0;
    }
}

int BitSetsMainWindow::parseCommand(QString &str)
{
    if(str=="commands")
    {
        static int easteregg=0;
        if(easteregg==0)
            printToConsole("<font face='Comic Sans MS'>Looks like have ourselves a comedian.</font>");
        if(easteregg==1)
            printToConsole("Repetition might be one of the elements of humor, but you can stop now.");
        if(easteregg==2)
        {
            printToConsole("<i>Right... i don't have to take this.</i>");
            QTimer::singleShot(2000,this,SLOT(quit()));
        }
        easteregg++;
        return -1;
    }
    if(str.contains("#max="))
        return parseNewUpperBound(str);
    if(str=="#max")
    {
        str="";
        printToConsole("Current upper bound: #max="+QString::number(upperBound));
        return 0;
    }
    if(str.contains("="))
        return parseSetAssignment(str);
    if(str.contains("#"))
        return parseSetSize(str);
    if(str.length()==1)
    {
        int setIndex=parseSetIdentifier(str);
        if(setIndex!=-1)
            printToConsole(QString(QChar(((QChar)'A').unicode()+setIndex))+"="+bitSets[setIndex]);
        return 0;
    }

    printError("Valid command expected.");
    return -1;
}

void BitSetsMainWindow::receivedConsoleCommand(const QString &str)
{
    QString str1;
    str1=str.simplified();
    printCommand(str1);
    str1.replace(" ","");
    parseCommand(str1);
}

void BitSetsMainWindow::quit()
{
    this->close();
}
