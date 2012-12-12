#include "ImageViewer.h"
#include "ui_ImageViewer.h"
#include <QLabel>
#include <QAction>
#include <QPalette>
#include <QScrollArea>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include "HistogramWidget.h"
#include "Histogram.h"
#include <QWidget>
#include "Otsu.h"
#include "Morphology.h"
#include "EdgeDetection.h"
#include "HoughTransform.h"
#include "Utility.h"
#include <QInputDialog>
#include <ImageDisplay.h>
#include <QMouseEvent>
#include "GraphCut.h"
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "ImageItem.h"
#include "GrabCut.h"
#include "ConnectedComponents.h"
#include "PatternRecognition.h"
#include "ImageStitching.h"

ImageViewer::ImageViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ImageViewer)
{
    ui->setupUi(this);

    initialize();

    showMaximized();
}

ImageViewer::ImageViewer(QImage original_image, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ImageViewer)
{
    ui->setupUi(this);

    initialize();

    showMaximized();

    if (original_image.isNull()) {
        QMessageBox::information(this, tr("Computer Graphics"), tr("Original Image is Null"));
        return;
    }
    image = original_image;
//    image_label->setPixmap(QPixmap::fromImage(image));
//    image_label->resize(image.size());
//    scale_factor = 1.0;

    //Add to QGraphicsScene
    _item = new ImageItem(QPixmap::fromImage(image));
    _scene->addItem(_item);

//    updateActions();
}

ImageViewer::~ImageViewer()
{
    delete ui;
//    delete histogram;
    delete _item;
    _item = NULL;
    delete _item2;
    _item2 = NULL;
    delete _scene;
    delete image_label;
    delete _pattern;
    delete zoom_in_action;
    delete zoom_out_action;
    delete view_histogram_action;
    delete otsu_action;
    delete morph_dilation_action;
    delete morph_erosion_action;
    delete morph_majority_action;
    delete morph_opening_action;
    delete morph_closing_action;
    delete gradient_magnitude;
    delete gradient_orientation;
    delete blur_action;
    delete comb_edge_detect_action;
    delete hough_action;
    delete grayscale_action;
    delete threshold_action;
    delete graphcut_action;
    delete grabcut_action;
    delete connect_components_action;
    delete train_pr_action;
    delete reset_training_action;
    delete recognize_pattern_action;
    delete image_stitching_action;
    delete clear_markers_action;
    delete pad_image_action;
}

void ImageViewer::initialize()
{
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    image_toolbar = ui->mainToolBar;

    image_label = ui->image_display;
    scroll_area = ui->scroll_area;
    image_label->setMouseTracking(false);
    scroll_area->setWidgetResizable(true);

    _foreground_pixels = new QList<QPoint>;
    _background_pixels = new QList<QPoint>;

    _pattern = new PatternRecognition();

    ui->bg_btn->setCheckable(true);
    ui->bg_btn->setChecked(false);
    ui->fg_btn->setCheckable(true);
    ui->fg_btn->setChecked(false);
    ui->rect_select_btn->setCheckable(true);
    ui->rect_select_btn->setChecked(false);

    this->setStyleSheet("QPushButton:checked { background-color: white; }");

    //Setup Scene
    _scene = new QGraphicsScene();
    ui->_view->setScene(_scene);
    _scale_factor = 1.15;
    _item = NULL;
    _item2 = NULL;

    createActions();
    createMenus();
    createToolbars();
    connectButtons();

    setWindowTitle(tr("Computer Vision"));
}

void ImageViewer::createMenus()
{
    file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(open_action);
    file_menu->addAction(open_image2_action);
    file_menu->addAction(save_action);
    file_menu->addSeparator();
    file_menu->addAction(exit_action);
}

void ImageViewer::createToolbars()
{
    image_toolbar->addAction(zoom_in_action);
    image_toolbar->addAction(zoom_out_action);
    image_toolbar->addSeparator();
    image_toolbar->addAction(view_histogram_action);
    image_toolbar->addSeparator();
    image_toolbar->addAction(otsu_action);
    image_toolbar->addSeparator();
    image_toolbar->addAction(morph_dilation_action);
    image_toolbar->addAction(morph_erosion_action);
    image_toolbar->addAction(morph_majority_action);
    image_toolbar->addAction(morph_opening_action);
    image_toolbar->addAction(morph_closing_action);
    image_toolbar->addSeparator();
    image_toolbar->addAction(gradient_magnitude);
    image_toolbar->addAction(gradient_orientation);
    image_toolbar->addAction(blur_action);
    image_toolbar->addAction(comb_edge_detect_action);
    image_toolbar->addAction(hough_action);
    image_toolbar->addAction(grayscale_action);
    image_toolbar->addAction(threshold_action);
    image_toolbar->addAction(graphcut_action);
    image_toolbar->addAction(grabcut_action);
    image_toolbar->addSeparator();
    image_toolbar->addAction(connect_components_action);
    image_toolbar->addAction(train_pr_action);
    image_toolbar->addAction(reset_training_action);
    image_toolbar->addAction(recognize_pattern_action);
    image_toolbar->addSeparator();
    image_toolbar->addAction(image_stitching_action);
    image_toolbar->addAction(clear_markers_action);
    image_toolbar->addAction(pad_image_action);
}

void ImageViewer::createActions()
{
    open_action = new QAction(tr("&Open..."), this);
    open_action->setShortcut(tr("Ctrl+O"));
    connect(open_action, SIGNAL(triggered()), this, SLOT(open()));

    open_image2_action = new QAction(tr("&Open..."), this);
    open_image2_action->setShortcut(tr("Ctrl+P"));
    connect(open_image2_action, SIGNAL(triggered()), this, SLOT(openImageTwo()));

    save_action = new QAction(tr("&Save..."), this);
    save_action->setShortcut(tr("Ctrl+S"));
    connect(save_action, SIGNAL(triggered()), this, SLOT(save()));

    exit_action = new QAction(tr("&Exit"), this);
    exit_action->setShortcut(tr("Ctrl+Q"));
    connect(exit_action, SIGNAL(triggered()), this, SLOT(close()));

    zoom_in_action = new QAction(tr("Zoom &In"), this);
    zoom_in_action->setShortcut(tr("Ctrl++"));
//    zoom_in_action->setEnabled(false);
    connect(zoom_in_action, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoom_out_action = new QAction(tr("Zoom &Out"), this);
    zoom_out_action->setShortcut(tr("Ctrl+-"));
//    zoom_out_action->setEnabled(false);
    connect(zoom_out_action, SIGNAL(triggered()), this, SLOT(zoomOut()));

    view_histogram_action = new QAction(tr("View &Histogram"), this);
//    view_histogram_action->setIcon(QIcon(":/barcode.png"));
    connect(view_histogram_action, SIGNAL(triggered()), this, SLOT(viewHistogram()));

    otsu_action = new QAction(tr("Otsu &Threshold"), this);
    connect(otsu_action, SIGNAL(triggered()), this, SLOT(otsu()));

    morph_dilation_action = new QAction(tr("Dilation"), this);
    connect(morph_dilation_action, SIGNAL(triggered()), this, SLOT(dilation()));

    morph_erosion_action = new QAction(tr("Erosion"), this);
    connect(morph_erosion_action, SIGNAL(triggered()), this, SLOT(erosion()));

    morph_majority_action = new QAction(tr("Majority"), this);
    connect(morph_majority_action, SIGNAL(triggered()), this, SLOT(majority()));

    morph_opening_action = new QAction(tr("Opening Morph"), this);
    connect(morph_opening_action, SIGNAL(triggered()), this, SLOT(opening()));

    morph_closing_action = new QAction(tr("Closing Morph"), this);
    connect(morph_closing_action, SIGNAL(triggered()), this, SLOT(closing()));

    gradient_magnitude = new QAction(tr("Gradient Magnitude"), this);
    connect(gradient_magnitude, SIGNAL(triggered()), this, SLOT(gradientMagnitude()));

    gradient_orientation = new QAction(tr("Gradient Orientation"), this);
    connect(gradient_orientation, SIGNAL(triggered()), this, SLOT(gradientOrientation()));

    blur_action = new QAction(tr("Blur"), this);
    connect(blur_action, SIGNAL(triggered()), this, SLOT(blur()));

    comb_edge_detect_action = new QAction(tr("Combo Edge Detection"), this);
    connect(comb_edge_detect_action, SIGNAL(triggered()), this, SLOT(combinationEdgeDetection()));

    hough_action = new QAction(tr("Hough Transform"), this);
    connect(hough_action, SIGNAL(triggered()), this, SLOT(houghTransform()));

    grayscale_action = new QAction(tr("Grayscale"), this);
    connect(grayscale_action, SIGNAL(triggered()), this, SLOT(grayScale()));

    threshold_action = new QAction(tr("Threshold"), this);
    connect(threshold_action, SIGNAL(triggered()), this, SLOT(threshold()));

    graphcut_action = new QAction(tr("Graph Cut"), this);
    connect(graphcut_action, SIGNAL(triggered()), this, SLOT(graphCut()));

    grabcut_action = new QAction(tr("Grab Cut"), this);
    grabcut_action->setEnabled(false);
    connect(grabcut_action, SIGNAL(triggered()), this, SLOT(grabCut()));

    connect_components_action = new QAction(tr("Connect Components"), this);
    connect(connect_components_action, SIGNAL(triggered()), this, SLOT(connectComponents()));

    train_pr_action = new QAction(tr("Train PR"), this);
    connect(train_pr_action, SIGNAL(triggered()), this, SLOT(trainPatternRecognition()));

    reset_training_action = new QAction(tr("Reset Training"), this);
    connect(reset_training_action, SIGNAL(triggered()), this, SLOT(resetTraining()));

    recognize_pattern_action = new QAction(tr("Recognize Patterns"), this);
    connect(recognize_pattern_action, SIGNAL(triggered()), this, SLOT(recognizePatterns()));

    image_stitching_action = new QAction(tr("Image Stitching"), this);
    connect(image_stitching_action, SIGNAL(triggered()), this, SLOT(imageStitching()));

    clear_markers_action = new QAction(tr("Clear Markers"), this);
    connect(clear_markers_action, SIGNAL(triggered()), this, SLOT(clearMarkers()));

    pad_image_action = new QAction(tr("Pad Image"), this);
    connect(pad_image_action, SIGNAL(triggered()), this, SLOT(padImage()));
}

void ImageViewer::connectButtons()
{
    connect(ui->clear_btn, SIGNAL(clicked()), this, SLOT(clearSelections()));
    connect(ui->fg_btn, SIGNAL(clicked()), this, SLOT(selectForegroundMode()));
    connect(ui->bg_btn, SIGNAL(clicked()), this, SLOT(selectBackgroundMode()));
    connect(ui->rect_select_btn, SIGNAL(clicked()), this, SLOT(selectRectangleMode()));
}

void ImageViewer::open()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
    if ( !file_name.isEmpty() ) {
        qDebug() << "Opening File";
        image = QImage(file_name);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Computer Graphics"), tr("Cannot load %1.").arg(file_name));
            return;
        }
//        image_label->setPixmap(QPixmap::fromImage(image));
//        image_label->resize(image.size());
//        scroll_area->resize(image.size());
//        scale_factor = 1.0;

        //Add to QGraphicsScene
        if ( _item != NULL ) {
            _scene->removeItem(_item);
            delete _item;
            _item = NULL;
        }
        _item = new ImageItem(QPixmap::fromImage(image));
        _scene->addItem(_item);

//        updateActions();
    }
}

void ImageViewer::openImageTwo()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Image Two"), QDir::currentPath());
    if ( !file_name.isEmpty() ) {
        qDebug() << "Opening File";
        image2 = QImage(file_name);

        if ( _item2 != NULL ) {
            _scene->removeItem(_item2);
            delete _item2;
            _item2 = NULL;
        }

        //Add to QGraphicsScene
        QRectF neighbor = _item->boundingRect();

        _item2 = new ImageItem(QPixmap::fromImage(image2));
        _item2->setPos(neighbor.right() + (image2.width() / 2), 0);
        _scene->addItem(_item2);

//        updateActions();
    }
}

void ImageViewer::save()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath(), tr("Images (*.png *.ppm *.pgm)"));
    if ( !file_name.isEmpty() ) {
        if ( !image.save(file_name) ) {
            QMessageBox::information(this, tr("Unable to save file"), "File failed to save");
        }
    }
}

void ImageViewer::zoomIn()
{
//    scaleImage(.25);
    ui->_view->scale(_scale_factor, _scale_factor);
}

void ImageViewer::zoomOut()
{
//    scaleImage(-.25);
    ui->_view->scale((1.0/ _scale_factor), (1.0 / _scale_factor));
}

void ImageViewer::viewHistogram()
{
    Histogram hist(&image);
    histogram = new HistogramWidget(hist, 0);
    histogram->show();
}

void ImageViewer::otsu()
{
    Otsu otsu;

    ImageViewer* viewer = new ImageViewer(otsu.processImage(&image), 0);
    viewer->setWindowTitle("Otsu Threshold");
    viewer->show();
}

void ImageViewer::dilation()
{
    Morphology m;
    QList<QPoint> structure = m.getSquareStruct(3);
    ImageViewer *viewer = new ImageViewer(m.dilation(&image, &structure), 0);
    viewer->setWindowTitle("Dilation");
    viewer->show();
}

void ImageViewer::erosion()
{
    Morphology m;
    QList<QPoint> structure = m.getSquareStruct(3);
    ImageViewer *viewer = new ImageViewer(m.erosion(&image, &structure), 0);
    viewer->setWindowTitle("Erosion");
    viewer->show();
}

void ImageViewer::majority()
{
    Morphology m;
    QList<QPoint> structure = m.getSquareStruct(3);
    ImageViewer *viewer = new ImageViewer(m.majority(&image, &structure), 0);
    viewer->setWindowTitle("Majority");
    viewer->show();
}

void ImageViewer::opening()
{
    Morphology m;
    QList<QPoint> structure = m.getCustomStruct();
    ImageViewer *viewer = new ImageViewer(m.opening(&image, &structure), 0);
    viewer->setWindowTitle("Opening Morph");
    viewer->show();
}

void ImageViewer::closing()
{
    Morphology m;
    QList<QPoint> structure = m.getSquareStruct(3);
    ImageViewer *viewer = new ImageViewer(m.closing(&image, &structure), 0);
    viewer->setWindowTitle("Closing Morph");
    viewer->show();
}

void ImageViewer::gradientMagnitude()
{
    EdgeDetection e;
    ImageViewer *viewer = new ImageViewer(e.gradientMagnitudeImage(&image), 0);
    viewer->setWindowTitle("Gradient Magnitude");
    viewer->show();
}

void ImageViewer::gradientOrientation()
{
    EdgeDetection e;
    ImageViewer *viewer = new ImageViewer(e.gradientOrientationImage(&image), 0);
    viewer->setWindowTitle("Gradient Orientation");
    viewer->show();
}

void ImageViewer::blur()
{
    EdgeDetection e;
    ImageViewer *viewer = new ImageViewer(e.blurColorImage(&image), 0);
    viewer->setWindowTitle("Blur");
    viewer->show();
}

void ImageViewer::combinationEdgeDetection()
{
    EdgeDetection e;
    ImageViewer *viewer = new ImageViewer(e.combinationEdgeDetection(&image), 0);
    viewer->setWindowTitle("Combination Edge Detection");
    viewer->show();
}

void ImageViewer::houghTransform()
{
    HoughTransform h;
    ImageViewer * viewer = new ImageViewer(h.transform(&image), 0);
    viewer->setWindowTitle("Hough Transform");
    viewer->show();
}

void ImageViewer::grayScale()
{
    ImageViewer *viewer = new ImageViewer(Utility::grayScaleImage(&image), 0);
    viewer->setWindowTitle("Grayscale");
    viewer->show();
}

void ImageViewer::threshold()
{
    int threshold = QInputDialog::getInteger(0, "Threshold", "Threshold value:", 1, 0, 255);
    ImageViewer *viewer = new ImageViewer(Utility::threshold(&image, threshold), 0);
    viewer->setWindowTitle("Threshold");
    viewer->show();
}

void ImageViewer::graphCut()
{
    if ( !_foreground_pixels->isEmpty() && !_background_pixels->isEmpty() ) {
        GraphCut g;
        ImageViewer *viewer = new ImageViewer(g.graphCutImage(&image, _foreground_pixels, _background_pixels), 0);
        viewer->setWindowTitle("Graph Cut");
        viewer->show();
    } else {
        QMessageBox::warning(this, tr("Unable to complete Graph Cut"), "Missing Foreground and/or Background seeds");
    }
}

void ImageViewer::grabCut()
{
    GrabCut g;
    ImageViewer *viewer = new ImageViewer(g.getGrabCutImage(&image, _item->getRectF()), 0);
    viewer->setWindowTitle("Grab Cut");
    viewer->show();
}

void ImageViewer::connectComponents()
{
    ImageViewer *viewer = new ImageViewer(_pattern->getConnectedComponents(&image), 0);
    viewer->setWindowTitle("Connected Components");
    viewer->show();
}

void ImageViewer::trainPatternRecognition()
{
    ImageViewer *viewer = new ImageViewer(_pattern->trainPatterns(&image, _foreground_pixels));
    viewer->setWindowTitle("Training Pattern Recognition");
    viewer->show();
}

void ImageViewer::resetTraining()
{
    _pattern->resetTraining();
    QMessageBox::information(this, "Reset Pattern Recognition Training Data", "The training data for pattern recognition has been reset.");
}

void ImageViewer::recognizePatterns()
{
    ImageViewer *viewer = new ImageViewer(_pattern->recognizePatterns(&image));
    viewer->setWindowTitle("Pattern Recognition");
    viewer->show();
}

void ImageViewer::imageStitching()
{
    ImageStitching im;
    ImageViewer *viewer = new ImageViewer(im.getStitchedImages(&image, _item->getMarkers(), &image2, _item2->getMarkers()));
    viewer->setWindowTitle("Image Stitching");
    viewer->show();
}

void ImageViewer::clearMarkers()
{
    if ( _item != NULL ) {
        _item->clearMarkers();
    }

    if ( _item2 != NULL ) {
        _item2->clearMarkers();
    }
}

void ImageViewer::padImage()
{
    ImageStitching im;
    image = im.getPaddedImage(&image);
    //Add to QGraphicsScene
    if ( _item != NULL ) {
        _scene->removeItem(_item);
        delete _item;
        _item = NULL;
    }
    _item = new ImageItem(QPixmap::fromImage(image));
    _scene->addItem(_item);
}

void ImageViewer::updateActions()
{
//    qDebug() << "Updating Actions";
//    zoom_in_action->setEnabled(scale_factor < 3.0);
//    zoom_out_action->setEnabled(scale_factor > 0.333);
//    save_action->setEnabled(!image_label->pixmap()->isNull());
}

void ImageViewer::scaleImage(double factor)
{
    Q_ASSERT(!image.isNull());
    scale_factor += factor;
    QImage scaled_image = image.scaled(scale_factor * image.size());

    image_label->setPixmap(QPixmap::fromImage(scaled_image));

    zoom_in_action->setEnabled(scale_factor < 3.0);
    zoom_out_action->setEnabled(scale_factor > 0.333);
}

void ImageViewer::clearSelections()
{
    disconnect(image_label, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(foregroundClick(QMouseEvent*)));
    disconnect(image_label, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(backgroundClick(QMouseEvent*)));
    disconnect(image_label, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(rectangleStart(QMouseEvent*)));
    disconnect(image_label, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(rectangleMove(QMouseEvent*)));
    disconnect(image_label, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(rectangleEnd(QMouseEvent*)));
    ui->foreground_list->clear();
    ui->background_list->clear();
    ui->fg_btn->setChecked(false);
    ui->bg_btn->setChecked(false);
    ui->rect_select_btn->setChecked(false);
    _foreground_pixels->clear();
    _background_pixels->clear();
    rect = QRect();
    ui->statusBar->showMessage("");
}

void ImageViewer::selectForegroundMode()
{
    ui->fg_btn->setChecked(true);
    ui->bg_btn->setChecked(false);
    ui->rect_select_btn->setChecked(false);
    disconnect(image_label, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(backgroundClick(QMouseEvent*)));
    disconnect(image_label, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(rectangleStart(QMouseEvent*)));
    disconnect(image_label, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(rectangleMove(QMouseEvent*)));
    disconnect(image_label, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(rectangleEnd(QMouseEvent*)));
    connect(image_label, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(foregroundClick(QMouseEvent*)));
    ui->statusBar->showMessage("Foreground Mode");
}

void ImageViewer::selectBackgroundMode()
{
    ui->fg_btn->setChecked(false);
    ui->bg_btn->setChecked(true);
    ui->rect_select_btn->setChecked(false);
    disconnect(image_label, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(foregroundClick(QMouseEvent*)));
    disconnect(image_label, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(rectangleStart(QMouseEvent*)));
    disconnect(image_label, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(rectangleMove(QMouseEvent*)));
    disconnect(image_label, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(rectangleEnd(QMouseEvent*)));
    connect(image_label, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(backgroundClick(QMouseEvent*)));
    ui->statusBar->showMessage("Background Mode");

}

void ImageViewer::selectRectangleMode()
{
    ui->fg_btn->setChecked(false);
    ui->bg_btn->setChecked(false);
    ui->rect_select_btn->setChecked(true);
    ui->statusBar->showMessage("Rectangle Mode");

    QRectF rect = _item->getRectF();
    QMessageBox::information(this, "Rect Info", QString("(%1, %2)").arg(rect.center().x()).arg(rect.center().y()));

}

void ImageViewer::foregroundClick(QMouseEvent *e)
{
    _foreground_pixels->append(e->pos());
    ui->foreground_list->addItem(QString("(%1,%2)").arg(e->pos().x()).arg(e->pos().y()));
    qDebug() << "FG Click";
}

void ImageViewer::backgroundClick(QMouseEvent *e)
{
    _background_pixels->append(e->pos());
    ui->background_list->addItem(QString("(%1,%2)").arg(e->pos().x()).arg(e->pos().y()));
    qDebug() << "BG Click";
}


void ImageViewer::rectangleStart(QMouseEvent *e)
{
    selecting = true;
    rect_start = e->pos();
    qDebug() << "Rect Click";
}

void ImageViewer::rectangleMove(QMouseEvent *e)
{
    qDebug() << "Rect Move";
    if ( selecting ) {
        rect_current = e->pos();
    }
}

void ImageViewer::rectangleEnd(QMouseEvent *e)
{
    qDebug() << "Rect End";
    selecting = false;
    rect = QRect(rect_start, rect_current);
}

