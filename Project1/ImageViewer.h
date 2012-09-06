#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <QScrollBar>
#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QImage>
#include <QToolBar>
#include <QScrollArea>
#include "HistogramWidget.h"
#include "ImageViewer.h"

namespace Ui {
class ImageViewer;
}

class ImageViewer : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ImageViewer(QWidget *parent = 0);
    ImageViewer(QImage original_image, QWidget *parent = 0);
    ~ImageViewer();

private slots:
    void open();
    void save();
    void zoomIn();
    void zoomOut();
    void viewHistogram();
    void otsu();
    
private:
    Ui::ImageViewer *ui;

    void initialize();
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
    QAction* otsu_action;

    QMenu *file_menu;
    QToolBar *image_toolbar;

};

#endif // IMAGEVIEWER_H
