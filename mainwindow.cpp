#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());

    /*
     *only digits allowed
     */
    for(int i = 1; i < 82; i++) {
        QString name = QString("lineEdit_%1").arg(i);
        QLineEdit* line = this->findChild<QLineEdit*>(name);
        line->setValidator(new QIntValidator(0, 9, this));
    }

    /*
     *Initialize timer
     */
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateChrono()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionA_propos_triggered()
{
    QMessageBox::question(this, tr("Aide"),
                             tr("Sudoku <b>v1.0</b><br />Louis Toulet<br /><br />Pour démarrer une nouvelle partie, <br />cliquez sur fichier->nouvelle partie <br />et choisissez une difficulté."));
}

void MainWindow::on_actionQuitter_triggered()
{
    QApplication::closeAllWindows();
}

void MainWindow::on_pushButton_clicked()
{
    /*
     *Checking grid (check.cpp)
     */
    bool won = check();
    if(won) {
        timer->stop();
        QMessageBox::information(this, tr("Aide"),
                                 tr("Victoire !<br />Encore une ? :)"));

        ui->actionMontrer_solution->setEnabled(false);
        ui->actionDonner_un_indice->setEnabled(false);
    }
    else
        QMessageBox::critical(this, tr("Erreur"),
                                 tr("La grille est incorrecte !"));

    colorCases();
}

void MainWindow::on_actionEffacer_grille_triggered()
{
    QList<QLineEdit*> Texts = findChildren<QLineEdit*>();
    foreach ( QLineEdit * text, Texts) {
        text->setText("");
    }
    ui->statusBar->showMessage("");
    ui->actionEffacer_grille->setEnabled(false);
    ui->actionMontrer_solution->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->actionDonner_un_indice->setEnabled(false);
    this->unlockCases();
    timer->stop();
    uncolorCases();
}

void MainWindow::on_actionDonner_un_indice_triggered()
{
    /*
     *Is grid full ?
     */
    if(this->isGridFull()) {
        QMessageBox::information(this, tr("Grille complète"),
                                 tr("La grille est complète, Impossible de donner un indice !"));
        return;
    }

    do {
        int i = this->generateNumber(81);
        QString name = QString("lineEdit_%1").arg(i);
        QLineEdit* line = this->findChild<QLineEdit*>(name);
        if(line->text()== "") {
            std::list<QString>::iterator it;
            it = solution.begin();
            std::advance(it,i-1);
            line->setText(*it);
            line->setStyleSheet("font-weight:bold;color:green;");
            line->setEnabled(false);
            hintsLeft--;
            ui->actionDonner_un_indice->setText("Donner un indice (" + QString::number(hintsLeft) + ")");

            /*
             * Is max hints number reached ?
             */
            if(hintsLeft==0)
                ui->actionDonner_un_indice->setEnabled(false);


            break;
        }
    } while (true); // Care of this ...
}

void MainWindow::on_actionMontrer_solution_triggered()
{
    for(int i = 1; i < 82; i++) {
        QString name = QString("lineEdit_%1").arg(i);
        QLineEdit* line = this->findChild<QLineEdit*>(name);
        line->setText(solution.front());
        solution.pop_front();
    }
    ui->actionMontrer_solution->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->actionDonner_un_indice->setEnabled(false);
    timer->stop();
    colorCases();
}

void MainWindow::newGame(level currentLevel)
{
    ui->statusBar->showMessage("Génération de la grille en cours...");
    ui->actionEffacer_grille->trigger();
    this->generateGrid();
    this->hideCases(currentLevel);
    this->lockCases();
    ui->statusBar->showMessage("");
    ui->actionMontrer_solution->setEnabled(true);
    ui->actionEffacer_grille->setEnabled(true);
    ui->pushButton->setEnabled(true);
    ui->actionDonner_un_indice->setEnabled(true);

    hours = minutes = secondes = 0;

    hintsLeft = currentLevel;
    ui->actionDonner_un_indice->setText("Donner un indice (" + QString::number(hintsLeft) + ")");
    if(currentLevel == easier)
        ui->actionDonner_un_indice->setEnabled(false);
    else
        ui->actionDonner_un_indice->setEnabled(true);

    timer->start(1000);
}

void MainWindow::on_actionTr_s_facile_triggered()
{
    this->newGame(easier);
}

void MainWindow::on_actionFacile_triggered()
{
    this->newGame(easy);
}

void MainWindow::on_actionMoyen_triggered()
{
    this->newGame(medium);
}

void MainWindow::on_actionDifficile_triggered()
{
    this->newGame(hard);
}

void MainWindow::on_actionTr_s_difficile_triggered()
{
    this->newGame(harder);
}

void MainWindow::on_actionHardcore_triggered()
{
    this->newGame(hardcore);
}

void MainWindow::updateChrono()
{
    secondes++;
    if(secondes==60) { secondes=0; minutes++; }
    if(minutes==60) { minutes=0; hours++; }

    QString display = "Temps écoulé: ";

    if(hours<10)
        display = display + "0";
    display = display + QString::number(hours) + ":";

    if(minutes<10)
        display = display + "0";
    display = display + QString::number(minutes) + ":";

    if(secondes<10)
        display = display + "0";
    display = display + QString::number(secondes);

    ui->label->setText(display);
}

void MainWindow::colorCases()
{
    /*
     * Put red in wrong cases
     */
    for(int i=1;i<82;i++) {
        std::list<QString>::iterator it;
        it = solution.begin();
        std::advance(it,i-1);

        QString name = QString("lineEdit_%1").arg(i);
        QLineEdit* line = this->findChild<QLineEdit*>(name);

        if(line->styleSheet().contains("font-weight:bold"))
            continue;
        if(line->text()=="") {
            line->setStyleSheet("");
            continue;
        }

        if(line->text()==*it || solution.size()==0) // si la solution de la grille a été donnée
            line->setStyleSheet("color:green;");
        else
            line->setStyleSheet("color:red;");
    }
}

void MainWindow::uncolorCases()
{
    /*
     * base style for all cases
     */
    for(int i=1;i<82;i++) {
        std::list<QString>::iterator it;
        it = solution.begin();
        std::advance(it,i-1);

        QString name = QString("lineEdit_%1").arg(i);
        QLineEdit* line = this->findChild<QLineEdit*>(name);

        line->setStyleSheet("");
    }
}

bool MainWindow::isGridFull()
{
    QList<QLineEdit*> Texts = findChildren<QLineEdit*>();
    foreach ( QLineEdit * text, Texts) {
        if(text->text()=="")
            return false;
    }
    return true;
}
