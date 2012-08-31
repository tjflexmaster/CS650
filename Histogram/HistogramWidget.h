#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include "Histogram_global.h"

#include <QWidget>
#include "Histogram.h"

class HISTOGRAMSHARED_EXPORT HistogramWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HistogramWidget(QWidget *parent = 0);
    HistogramWidget(Histogram histogram, QWidget *parent = 0);
    
private:
    Histogram _histogram;
    int height;
    int width;
    int column_width;

    void initialize();

protected:
    void paintEvent(QPaintEvent *);
    
};

#endif // HISTOGRAMWIDGET_H
