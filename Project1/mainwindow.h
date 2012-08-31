#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollBar>
#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QImage>
#include <QToolBar>
#include <QScrollArea>
#include "HistogramWidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void open();
    void save();
    void zoomIn();
    void zoomOut();
    void viewHistogram();
    
private:
    Ui::MainWindow *ui;
    void createActions();
    void createMenus();
    void createToolbars();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollbar, double factor);

    QImage image;
    QLabel *image_label;
    QScrollArea *scroll_area;
    double scale_factor;
    HistogramWidget* histogram;

    QAction *open_action;
    QAction *save_action;
    QAction *exit_action;
    QAction *zoom_in_action;
    QAction *zoom_out_action;
    QAction* view_histogram_action;

    QMenu *file_menu;
    QToolBar *image_toolbar;

};

#endif // MAINWINDOW_H
