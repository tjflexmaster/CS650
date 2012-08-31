#include "HistogramWidget.h"
#include <QPainter>
#include <QColor>
#include <QDebug>

HistogramWidget::HistogramWidget(QWidget *parent) :
    QWidget(parent)
{
    initialize();
}

HistogramWidget::HistogramWidget(Histogram histogram, QWidget *parent) :
    QWidget(parent)
{
    _histogram = histogram;
    initialize();
}

void HistogramWidget::initialize()
{
    height = 300;
    column_width = 3;
    width = column_width * 255 + 10;
    resize(width, height);
}

void HistogramWidget::paintEvent(QPaintEvent *)
{
    //We need to normalize the values of the histogram so we will need the max value
    int max = _histogram.getMaxValue();


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(5, height-5);

    QMap<int, int>::iterator i;
    for ( i=_histogram.begin(); i != _histogram.end(); i++ ) {
        painter.setBrush(QColor(i.key(), i.key(), i.key()));
        double bar_height = ((height - 15) * i.value()) / max;
        int h = static_cast<int>(bar_height);
        int x = i.key();
        painter.drawRect( x * column_width, 0, column_width, -h);
    }
}
