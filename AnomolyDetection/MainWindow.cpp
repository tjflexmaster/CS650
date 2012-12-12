#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QAction>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QImage>
#include <QMessageBox>
#include "RxAlgorithm.h"
#include "ArmadilloExamples.h"
#include <QThreadPool>
#include <QDebug>
#include <QInputDialog>
#include <QGraphicsPixmapItem>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initialize();

    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _file_menu;
    delete _image_toolbar;
    delete _view;
    delete _scene;
    delete _open_action;
    delete _save_action;
    delete _zoom_in_action;
    delete _zoom_out_action;
    delete _rx_action;
    delete _connected_component_action;
    delete _image;
}

void MainWindow::initialize()
{
    //Setup Dock Widget
    ui->run_progress->hide();
    connect(ui->run_btn, SIGNAL(clicked()), this, SLOT(runRx()));
    _anomoly_detection_action = ui->properties_dockwidget->toggleViewAction();

    _image_toolbar = ui->mainToolBar;
    ui->mainToolBar->hide();

    //Setup Scene
    _view = new QGraphicsView(this);
    _scene = new QGraphicsScene();
    _view->setScene(_scene);
    _scale_factor = 1.15;
    setCentralWidget(_view);

    createActions();
    createMenu();
//    createToolbar();

    _image = new QImage();
    _rxalgorithm = new RxAlgorithm();
    _rxalgorithm->setAutoDelete(false);
    connect(_rxalgorithm, SIGNAL(processingComplete()), this, SLOT(rxComplete()));

    setWindowTitle(tr("Anomoly Detection"));
}

void MainWindow::createMenu()
{
    _file_menu = menuBar()->addMenu(tr("&File"));
    _file_menu->addAction(_open_action);
    _file_menu->addAction(_save_action);

    _dock_menu = menuBar()->addMenu(tr("&Docks"));
    _dock_menu->addAction(_anomoly_detection_action);

    _action_menu = menuBar()->addMenu(tr("&Actions"));
    _action_menu->addAction(_zoom_in_action);
    _action_menu->addAction(_zoom_out_action);
}

void MainWindow::createToolbar()
{
//    _image_toolbar->addAction(_zoom_in_action);
//    _image_toolbar->addAction(_zoom_out_action);
//    _image_toolbar->addAction(_rx_action);
//    _image_toolbar->addAction(_armadillo_action);
}

void MainWindow::createActions()
{
    _open_action = new QAction(tr("&Open..."), this);
    _open_action->setShortcut(tr("Ctrl+O"));
    connect(_open_action, SIGNAL(triggered()), this, SLOT(open()));

    _save_action = new QAction(tr("&Save..."), this);
    _save_action->setShortcut(tr("Ctrl+S"));
    connect(_save_action, SIGNAL(triggered()), this, SLOT(save()));

    _zoom_in_action = new QAction(tr("Zoom &In"), this);
    _zoom_in_action->setShortcut(tr("Ctrl++"));
    connect(_zoom_in_action, SIGNAL(triggered()), this, SLOT(zoomIn()));

    _zoom_out_action = new QAction(tr("Zoom &Out"), this);
    _zoom_out_action->setShortcut(tr("Ctrl+-"));
    connect(_zoom_out_action, SIGNAL(triggered()), this, SLOT(zoomOut()));

    _rx_action = new QAction(tr("Rx Algorithm"), this);
    connect(_rx_action, SIGNAL(triggered()), this, SLOT(runRx()));

    _armadillo_action = new QAction(tr("Armadillo Test"), this);
    connect(_armadillo_action, SIGNAL(triggered()), this, SLOT(armadillo()));
}

void MainWindow::open()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
    if ( !file_name.isEmpty() ) {
        if ( !_image->isNull() ) {
            delete _image;
            _image = NULL;
        }

        _image = new QImage(file_name);

        if (_image->isNull()) {
            QMessageBox::information(this, tr("Load Error"), tr("Cannot load %1.").arg(file_name));
            return;
        }

        //Add to QGraphicsScene
        _scene->clear();
        _scene->addPixmap(QPixmap::fromImage(*_image));
    }
}

void MainWindow::save()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath(), tr("Images (*.png *.jpg)"));
    if ( !file_name.isEmpty() ) {
        if ( !_image->save(file_name) ) {
            QMessageBox::information(this, tr("Save Error"), "Failed to save image");
        }
    }
}

void MainWindow::zoomIn()
{
    _view->scale(_scale_factor, _scale_factor);
}

void MainWindow::zoomOut()
{
    _view->scale((1.0/ _scale_factor), (1.0 / _scale_factor));
}

void MainWindow::runRx()
{
    ui->run_progress->show();
    ui->run_btn->hide();
    double sensitivity = (double) ui->sensitivitySlider->value() / 100.0;
    _rxalgorithm->setImage(_image);
    _rxalgorithm->setKernelProperties( ui->outerRadiusSpinBox->value(), ui->innerRadiusSpinBox->value());
    _rxalgorithm->setAlgorithmProperties(sensitivity);
    _rxalgorithm->setAnomolyProperties(ui->minimumDiameterPixelsSpinBox->value(), ui->redCheckBox->isChecked(), ui->greenCheckBox->isChecked(), ui->blueCheckBox->isChecked() );
    QThreadPool *threadpool = QThreadPool::globalInstance();
    threadpool->start(_rxalgorithm);
}

void MainWindow::rxComplete()
{
    qreal right_offset = _scene->itemsBoundingRect().right();
    QGraphicsPixmapItem *new_item = new QGraphicsPixmapItem(QPixmap::fromImage(_rxalgorithm->getImage()));
    new_item->setPos(right_offset + new_item->offset().x(), 0);
    _scene->addItem(new_item);

    ui->run_progress->hide();
    ui->run_btn->show();
}

void MainWindow::armadillo()
{
    try {
        ArmadilloExamples *test = new ArmadilloExamples();
        QThreadPool *threadpool = QThreadPool::globalInstance();
        threadpool->start(test);
        threadpool->waitForDone();

    } catch (...) {
        std::cout << "Armadillo Error" << std::endl;
    }
}
