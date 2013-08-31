#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cstdlib> //std::rand
#include <QtGui>

int MainWindow::generateNumber(int max) { return std::rand() % max + 1; }

void MainWindow::identifyCase(Case* current) {
    /*
     * Initialisation des variables de r√©f√©rence
     */
    int ligne[82], colonne[82], carre[82];

    /* Initialisation des colonnes */
    for(int i=1,iteration=1,j=1;i<82;i++) {
        colonne[i]=j;
        j++;

        if(j==4 && iteration<9) j=1;
        if(j==7 && iteration<18) j=4;
        if(j==10 && iteration<27) j=7;
        if(j==10 && iteration==27) { j=1; iteration=0; }
        iteration++;
    }

    /* TODO: Initialisation des lignes*/
    for(int i=1,iteration=1,j=1;i<82;i++) {
        ligne[i]=j;

        iteration++;

        if(iteration==4) j++;
        if(iteration==7) j++;
        if(iteration==10) { j-=2; iteration=1;}

        if(i==27 || i==54 ) j+=3;

    }

    /* Initialisation des carrÈs */
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
     *Initialisation des tableaux
     */
    for(int i=0; i<10;i++)
        for(int j=0;j<10;j++)
            lignes[i][j]=colonnes[i][j]=carres[i][j]=false;


    Case current,random;
    for(int i=1;i<82;i++) {
        current.number=i;
        identifyCase(&current);

        /* premier controle */
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


    if(!check()) // fail, aucune combinaison possible
        this->generateGrid();

    return;
}

void MainWindow::hideCase(level currentLevel)
{
    for(int i = 1; i < 82; i++) {
        QString name = QString("lineEdit_%1").arg(i);
        QLineEdit* line = this->findChild<QLineEdit*>(name);
        solution.push_back(line->text());
    }

    int random;
    std::vector<int> numbers;

    qDebug() << "Starting game with difficulty" << currentLevel;
    for(int i=0;i<9;i++) {
        for(int z=1;z<10;z++) //on ins√®re 1,2,3,4,5,6,7,8,9 dans un vecteur
            numbers.push_back(z);
        random_shuffle(numbers.begin(),numbers.end()); // on mÈlange l'ordre des chiffres pour les prendre un par un
        for(int j=currentLevel+1;j>0;j--) {
            random=numbers.back();
            numbers.pop_back();
            QString name = QString("lineEdit_%1").arg(i*9+random);
            QLineEdit* QLine = this->findChild<QLineEdit*>(name);
            QLine->setText("");
        }
        numbers.clear();
    }
}

void MainWindow::lockCases() {
    for(int i = 1; i < 82; i++) {
        QString name = QString("lineEdit_%1").arg(i);
        QLineEdit* line = this->findChild<QLineEdit*>(name);
        if(line->text()!="") {
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
