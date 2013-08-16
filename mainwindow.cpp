#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*
     *only digits allowed
     */
    std::list<int> lineList;
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
    //this->setStyleSheet("background-color:white");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionA_pto_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_actionA_propos_triggered()
{
    QMessageBox::question(this, tr("Aide"),
                             tr("Sudoku <b>v1.0</b> By Louis Toulet<br /><br />Pour démarrer une nouvelle partie, <br />cliquez sur fichier->nouvelle partie <br />et choisissez une difficulté."));
    //this->createDialog("Sudoku <b>v1.0</b> By Louis Toulet<br /><br />Pour démarrer une nouvelle partie, <br />cliquez sur fichier->nouvelle partie <br />et choisissez une difficulté.");
}

void MainWindow::on_actionQuitter_triggered()
{
    QApplication::closeAllWindows();
}

void MainWindow::on_pushButton_clicked()
{
    /*
     *Vérification de la grille (check.cpp)
     */
    bool won = check();
    if(won) {
        timer->stop();
        QMessageBox::information(this, tr("Aide"),
                                 tr("Victoire !<br />Encore une ? :)"));
        //this->createDialog("Victoire !<br />Encore une ? :)");
        ui->actionMontrer_solution->setEnabled(false);
        ui->actionDonner_un_indice->setEnabled(false);
    }
    else
        QMessageBox::critical(this, tr("Erreur"),
                                 tr("La grille est incorrecte !"));
    //this->createDialog("La grille est incorrecte !");

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
     *Grille remplie ?
     */
    if(this->isGridFull()) {
        //createDialog("La grille est complète ! Impossible de donner un indice !");
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

            /*
             *Nombre d'indice donné max atteint ?
             */
            if(hintsLeft==0)
                ui->actionDonner_un_indice->setEnabled(false);

            break;
        }
    } while (true); // Attention à la boucle infinie
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
    ui->actionAutofiill->setText("0"); // debug
    ui->actionEffacer_grille->trigger();
    this->generateGrid();
    this->hideCase(currentLevel);
    this->lockCases();
    ui->statusBar->showMessage("");
    ui->actionMontrer_solution->setEnabled(true);
    ui->actionEffacer_grille->setEnabled(true);
    ui->pushButton->setEnabled(true);
    ui->actionDonner_un_indice->setEnabled(true);

    heures = minutes = secondes = 0;

    hintsLeft = currentLevel;
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
    if(minutes==60) { minutes=0; heures++; }

    QString display = "Temps écoulé: ";

    if(heures<10)
        display = display + "0";
    display = display + QString::number(heures) + ":";

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
     *mettre les cases erronées en rouge
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
     *mettre style de base pour toutes les cases
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
