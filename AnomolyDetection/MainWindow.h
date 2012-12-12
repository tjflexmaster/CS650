#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "RxAlgorithm.h"
#include <QImage>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void open();
    void save();
    void zoomIn();
    void zoomOut();
    void runRx();
    void rxComplete();
    void armadillo();

    
private:
    void initialize();
    void createToolbar();
    void createMenu();
    void createActions();

    Ui::MainWindow *ui;

    QMenu *_file_menu;
    QMenu *_dock_menu;
    QMenu *_action_menu;
    QToolBar *_image_toolbar;

    QGraphicsView *_view;
    QGraphicsScene *_scene;
    double _scale_factor;

    //Actions
    QAction *_open_action;
    QAction *_save_action;
    QAction *_anomoly_detection_action;
    QAction *_zoom_in_action;
    QAction *_zoom_out_action;
    QAction *_rx_action;
    QAction *_armadillo_action;
    QAction *_connected_component_action;

    //Image
    QImage *_image;

    //Rx
    RxAlgorithm *_rxalgorithm;
};

#endif // MAINWINDOW_H
