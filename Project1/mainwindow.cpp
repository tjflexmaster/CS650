#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QAction>
#include <QPalette>
#include <QScrollArea>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include "HistogramWidget.h"
#include "Histogram.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    image_toolbar = ui->mainToolBar;

    image_label = new QLabel;

    scroll_area = new QScrollArea;
    scroll_area->setWidget(image_label);
    scroll_area->setWidgetResizable(true);

    this->setCentralWidget(scroll_area);

    createActions();
    createMenus();
    createToolbars();

    setWindowTitle(tr("Computer Vision"));
    resize(500, 400);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete histogram;
}

void MainWindow::createMenus()
{
    file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(open_action);
    file_menu->addAction(save_action);
    file_menu->addSeparator();
    file_menu->addAction(exit_action);
}

void MainWindow::createToolbars()
{
    image_toolbar->addAction(zoom_in_action);
    image_toolbar->addAction(zoom_out_action);
    image_toolbar->addSeparator();
    image_toolbar->addAction(view_histogram_action);
}

void MainWindow::createActions()
{
    open_action = new QAction(tr("&Open..."), this);
    open_action->setShortcut(tr("Ctrl+O"));
    connect(open_action, SIGNAL(triggered()), this, SLOT(open()));

    save_action = new QAction(tr("&Save..."), this);
    save_action->setShortcut(tr("Ctrl+S"));
    connect(save_action, SIGNAL(triggered()), this, SLOT(save()));

    exit_action = new QAction(tr("&Exit"), this);
    exit_action->setShortcut(tr("Ctrl+Q"));
    connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));

    zoom_in_action = new QAction(tr("Zoom &In"), this);
    zoom_in_action->setShortcut(tr("Ctrl++"));
    zoom_in_action->setEnabled(false);
    connect(zoom_in_action, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoom_out_action = new QAction(tr("Zoom &Out"), this);
    zoom_out_action->setShortcut(tr("Ctrl+-"));
    zoom_out_action->setEnabled(false);
    connect(zoom_out_action, SIGNAL(triggered()), this, SLOT(zoomOut()));

    view_histogram_action = new QAction(tr("View &Histogram"), this);
    view_histogram_action->setIcon(QIcon(":/barcode.png"));
    connect(view_histogram_action, SIGNAL(triggered()), this, SLOT(viewHistogram()));
}

void MainWindow::open()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
    if ( !file_name.isEmpty() ) {
        image = QImage(file_name);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Computer Graphics"), tr("Cannot load %1.").arg(file_name));
            return;
        }

        image_label->setPixmap(QPixmap::fromImage(image));
        scale_factor = 1.0;

        updateActions();
    }
}

void MainWindow::save()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath(), tr("Images (*.ppm *.pgm)"));
    if ( !file_name.isEmpty() ) {
        if ( !image.save(file_name) ) {
            QMessageBox::information(this, tr("Unable to save file"), "File failed to save");
        }
    }
}

void MainWindow::zoomIn()
{
    scaleImage(.25);
}

void MainWindow::zoomOut()
{
    scaleImage(-.25);
}

void MainWindow::viewHistogram()
{
    Histogram hist(&image);
    histogram = new HistogramWidget(hist, 0);
    histogram->show();
}

void MainWindow::updateActions()
{
    zoom_in_action->setEnabled(scale_factor < 3.0);
    zoom_out_action->setEnabled(scale_factor > 0.333);
    save_action->setEnabled(!image_label->pixmap()->isNull());
}

void MainWindow::scaleImage(double factor)
{
    Q_ASSERT(!image.isNull());
    scale_factor += factor;
    QImage scaled_image = image.scaled(scale_factor * image.size());

    image_label->setPixmap(QPixmap::fromImage(scaled_image));

    zoom_in_action->setEnabled(scale_factor < 3.0);
    zoom_out_action->setEnabled(scale_factor > 0.333);
}

//void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
//{
//    scrollBar->setValue(int(factor * scrollBar->value()
//                         + ((factor - 1) * scrollBar->pageStep()/2)));
//}
