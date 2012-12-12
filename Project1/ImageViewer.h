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
#include "ImageDisplay.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "ImageItem.h"
#include "PatternRecognition.h"

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
    void openImageTwo();
    void save();
    void zoomIn();
    void zoomOut();
    void viewHistogram();
    void otsu();
    void dilation();
    void erosion();
    void majority();
    void opening();
    void closing();
    void gradientMagnitude();
    void gradientOrientation();
    void blur();
    void combinationEdgeDetection();
    void houghTransform();
    void grayScale();
    void threshold();
    void graphCut();
    void grabCut();
    void connectComponents();
    void trainPatternRecognition();
    void resetTraining();
    void recognizePatterns();
    void imageStitching();
    void clearMarkers();
    void padImage();

    void clearSelections();
    void selectForegroundMode();
    void selectBackgroundMode();
    void selectRectangleMode();

    void foregroundClick(QMouseEvent *e);
    void backgroundClick(QMouseEvent *e);

    void rectangleStart(QMouseEvent *e);
    void rectangleMove(QMouseEvent *e);
    void rectangleEnd(QMouseEvent *e);
    
private:
    Ui::ImageViewer *ui;

    void initialize();
    void createActions();
    void createMenus();
    void createToolbars();
    void connectButtons();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollbar, double factor);

    QImage image;
    QImage image2;
    ImageDisplay *image_label;
    QScrollArea *scroll_area;
    double scale_factor;
    HistogramWidget* histogram;

    QAction *open_action;
    QAction *open_image2_action;
    QAction *save_action;
    QAction *exit_action;

    QAction *zoom_in_action;
    QAction *zoom_out_action;
    QAction *view_histogram_action;
    QAction *otsu_action;
    QAction *morph_dilation_action;
    QAction *morph_erosion_action;
    QAction *morph_majority_action;
    QAction *morph_opening_action;
    QAction *morph_closing_action;
    QAction *gradient_magnitude;
    QAction *gradient_orientation;
    QAction *blur_action;
    QAction *comb_edge_detect_action;
    QAction *hough_action;
    QAction *grayscale_action;
    QAction *threshold_action;
    QAction *graphcut_action;
    QAction *grabcut_action;
    QAction *connect_components_action;
    QAction *train_pr_action;
    QAction *reset_training_action;
    QAction *recognize_pattern_action;
    QAction *image_stitching_action;
    QAction *clear_markers_action;
    QAction *pad_image_action;

    QMenu *file_menu;
    QToolBar *image_toolbar;

    QList<QPoint> *_foreground_pixels;
    QList<QPoint> *_background_pixels;

    //Rectangle
    bool selecting;
    QPoint rect_start;
    QPoint rect_current;
    QRect rect;

    //Graphics Scene
    qreal _scale_factor;
    QGraphicsScene *_scene;
    ImageItem *_item;
    ImageItem *_item2;

    //Pattern Recognition
    PatternRecognition *_pattern;

};

#endif // IMAGEVIEWER_H
