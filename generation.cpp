#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cstdlib> //std::rand
#include <QDebug>

/* generates random number between 1 and max */
int MainWindow::generateNumber(int max) { return std::rand() % max + 1; }

/* generates random number between min and max */
int MainWindow::generateNumber(int min, int max) { return (std::rand() % (max-min+1) + min); }

/* generates an array of numbers, from 1 to 9, in random order */
std::vector<int> MainWindow::generate9RandomNumbers() {
    std::vector<int> numbers;
    numbers.clear();
    for(int z=1;z<10;z++) //inserting 1,2,3,4,5,6,7,8,9 in a vector
        numbers.push_back(z);
    random_shuffle(numbers.begin(),numbers.end()); // shuffle numbers order to take them 1 by 1 later

    return numbers;
}

void MainWindow::identifyCase(Case* current) {
    /*
     * base variables initialization
     */
    int ligne[82], colonne[82], carre[82];

    /* colomns initialization */
    for(int i=1,iteration=1,j=1;i<82;i++) {
        colonne[i]=j;
        j++;

        if(j==4 && iteration<9) j=1;
        if(j==7 && iteration<18) j=4;
        if(j==10 && iteration<27) j=7;
        if(j==10 && iteration==27) { j=1; iteration=0; }
        iteration++;
    }

    /* lines initialization */
    for(int i=1,iteration=1,j=1;i<82;i++) {
        ligne[i]=j;

        iteration++;

        if(iteration==4) j++;
        if(iteration==7) j++;
        if(iteration==10) { j-=2; iteration=1;}

        if(i==27 || i==54 ) j+=3;

    }

    /* Squares initialization */
    for(int i=1;i<82;i++)
        carre[i]=((i-1)/9)+1;


    current->carre = carre[current->number];
    current->colonne = colonne[current->number];
    current->ligne = ligne[current->number];

}

void MainWindow::generateGrid()
{
    bool lignes[10][10], colonnes[10][10], carres[10][10];
    std::vector<int> possible;
    solution.clear();

    /*
     * Arrays initialization
     */
    for(int i=0; i<10;i++)
        for(int j=0;j<10;j++)
            lignes[i][j]=colonnes[i][j]=carres[i][j]=false;


    Case current,random;
    for(int i=1;i<82;i++) {
        current.number=i;
        identifyCase(&current);

        /* first control */
        for(int j=1;j<10;j++) {
            if(lignes[current.ligne][j]==false
                    && colonnes[current.colonne][j]==false
                    && carres[current.carre][j]==false)
                possible.push_back(j);
        }

        if(possible.empty())
            break;

        random.number = possible.at(generateNumber(possible.size())-1);
        identifyCase(&random);

        if(lignes[current.ligne][random.number]==false
                && colonnes[current.colonne][random.number]==false
                && carres[current.carre][random.number]==false) {
            QString name = QString("lineEdit_%1").arg(i);
            QLineEdit* QLine = this->findChild<QLineEdit*>(name);
            QLine->setText(QString::number(random.number));
            lignes[current.ligne][random.number]=colonnes[current.colonne][random.number]=carres[current.carre][random.number]=true;
        }

        possible.clear();
    }


    if(!check()) // fail, no combinaison possible
        this->generateGrid();

    return;
}

void MainWindow::hideCases(level currentLevel)
{
    /*
     *Inserting grid into solution
     */
    for(int i = 1; i < 82; i++) {
        QString name = QString("lineEdit_%1").arg(i);
        QLineEdit* line = this->findChild<QLineEdit*>(name);
        solution.push_back(line->text());
    }

    qDebug() << "Starting game with difficulty" << currentLevel;

    /* --hiding--
     * easier: 45
     * easy: 47
     * medium: 49
     * hard: 52
     * harder: 54
     * hardcore: 56
     */
    int numbersToHide[6] = {45,47,49,52,54,56};
    int numbersToHideThisLoop, random;

    /* first round */
    for(int i=0;i<9;i++) {

        do
            if(numbersToHide[currentLevel]>=9) numbersToHideThisLoop = generateNumber(3,6); // hides minimum 3 / maximum 6 numbers per square
            else numbersToHideThisLoop = generateNumber(numbersToHide[currentLevel]);
        while(((numbersToHide[currentLevel] - numbersToHideThisLoop) < (9-i-1))); // condition not to hide too much numbers

        numbersToHide[currentLevel] -= numbersToHideThisLoop;

        std::vector<int> numbers = generate9RandomNumbers();
        for(int j=numbersToHideThisLoop;j>0;j--) {
            random=numbers.back();
            numbers.pop_back();
            QString name = QString("lineEdit_%1").arg(i*9+random);
            QLineEdit* QLine = this->findChild<QLineEdit*>(name);
            QLine->setText("");
        }
    }

    /*
     * At the end of first round, there is still several numbers to hide
     * (unless we're very lucky)
     * So here begins second round
     *
     * Picking randomly a big square of 9 and hide more numbers
     */

    while(numbersToHide[currentLevel]>0) {
        int bigSquare = generateNumber(9);
        int emptyCases=0;
        for(int j=bigSquare*9-8;j<bigSquare*9+1;j++) {
            QString name = QString("lineEdit_%1").arg(j);
            QLineEdit* QLine = this->findChild<QLineEdit*>(name);
            if(QLine->text().isEmpty())
                emptyCases++;
        }
        if(emptyCases>7) /* might change depending on the current level ? */
            continue;

        /*
         *I want at least 1 shown number in a square of 9
         */
        do
            if(emptyCases<2) numbersToHideThisLoop = generateNumber(7); // hide maximum 7 numbers per square
            else numbersToHideThisLoop = generateNumber(8-emptyCases);
        while(numbersToHideThisLoop > emptyCases);

        numbersToHide[currentLevel] -= numbersToHideThisLoop;


        std::vector<int> numbers = generate9RandomNumbers();
        for(int j=numbersToHideThisLoop;j>0;j--) {
            random=numbers.back();
            numbers.pop_back();
            QString name = QString("lineEdit_%1").arg(bigSquare*9-9+random);
            QLineEdit* QLine = this->findChild<QLineEdit*>(name);
            if(!QLine->text().isEmpty())
                QLine->setText("");
            else
                j++; //assumes the rest of the code is reliable !
        }
    }

}

void MainWindow::lockCases() {
    for(int i = 1; i < 82; i++) {
        QString name = QString("lineEdit_%1").arg(i);
        QLineEdit* line = this->findChild<QLineEdit*>(name);
        if(!line->text().isEmpty()) {
            line->setEnabled(false);
            line->setStyleSheet("font-weight:bold;color:black;");
        }
    }
}

void MainWindow::unlockCases() {
    for(int i = 1; i < 82; i++) {
        QString name = QString("lineEdit_%1").arg(i);
        QLineEdit* line = this->findChild<QLineEdit*>(name);
        line->setEnabled(true);
    }
}
