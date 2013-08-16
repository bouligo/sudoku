#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>

bool MainWindow::check()
{
    std::list<int> lineList;
    for(int i = 1; i < 82; i++) {
        QString name = QString("lineEdit_%1").arg(i);
        QLineEdit* line = this->findChild<QLineEdit*>(name);
        if(line) {
            lineList.push_back(line->text().toInt());
        }
    }

    return (check1(lineList) && check2(lineList) && check3(lineList));
}

bool MainWindow::check1(std::list<int> lineList)
{
    std::list<int> lineBak;
    std::list<int> linePart;
    std::list<int>::iterator it;

    lineBak = lineList;
    int toBegin[9] = {0,1,2,9,10,11,18,19,20};

    for(int i=0;i<9;i++) {
        it = lineList.begin();
        std::advance(it,toBegin[i]);
        lineList.erase(lineList.begin(),it);

        linePart.splice(linePart.begin(),lineList,it);
        it = lineList.begin();
        std::advance(it,2);
        linePart.splice(linePart.begin(),lineList,it);
        it = lineList.begin();
        std::advance(it,4);
        linePart.splice(linePart.begin(),lineList,it);
        it = lineList.begin();
        std::advance(it,24);
        linePart.splice(linePart.begin(),lineList,it);
        it = lineList.begin();
        std::advance(it,26);
        linePart.splice(linePart.begin(),lineList,it);
        it = lineList.begin();
        std::advance(it,28);
        linePart.splice(linePart.begin(),lineList,it);
        it = lineList.begin();
        std::advance(it,48);
        linePart.splice(linePart.begin(),lineList,it);
        it = lineList.begin();
        std::advance(it,50);
        linePart.splice(linePart.begin(),lineList,it);
        it = lineList.begin();
        std::advance(it,52);
        linePart.splice(linePart.begin(),lineList,it);

        if(!compare(linePart))
            return false;

        linePart.erase(linePart.begin(),linePart.end());
        lineList = lineBak;
    }
    return true;
}

bool MainWindow::check2(std::list<int> lineList)
{
    std::list<int> lineBak;
    std::list<int> linePart;
    std::list<int>::iterator it;

    lineBak = lineList;
    int toBegin[9] = {0,3,6,27,30,33,54,57,60};

    for(int i=0;i<9;i++) {
        it = lineList.begin();
        std::advance(it,toBegin[i]);
        lineList.erase(lineList.begin(),it);

        std::advance(it,3);
        linePart.splice(linePart.begin(),lineList,lineList.begin(),it);
        std::advance(it,6);
        lineList.erase(lineList.begin(),it);
        std::advance(it,3);
        linePart.splice(linePart.begin(),lineList,lineList.begin(),it);
        std::advance(it,6);
        lineList.erase(lineList.begin(),it);
        std::advance(it,3);
        linePart.splice(linePart.begin(),lineList,lineList.begin(),it);

        if(!compare(linePart))
            return false;

        linePart.erase(linePart.begin(),linePart.end());
        lineList = lineBak;
    }
    return true;
}

bool MainWindow::check3(std::list<int> lineList)
{
    std::list<int> linePart;
    std::list<int>::iterator it;

    for(int i=0;i<9;i++) {
        it = lineList.begin();
        std::advance(it,9);

        linePart.splice(linePart.begin(),lineList,lineList.begin(), it);

        if(!compare(linePart))
            return false;


        linePart.erase(linePart.begin(),linePart.end());
    }
    return true;
}

bool MainWindow::compare(std::list<int> cases)
{
    QList<int> Qbase;
    Qbase << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9;
    std::list<int> base = Qbase.toStdList();
    base.sort();
    cases.sort();

    if (base.size () == cases.size ())
        return  std::equal(base.begin(),base.end(),cases.begin());
    else
        return false;
}
