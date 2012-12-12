#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

#include <QLabel>
#include <QPoint>
#include <QImage>

class ImageDisplay : public QLabel
{
    Q_OBJECT
public:
    explicit ImageDisplay(QWidget *parent = 0);
    
signals:
    void mousePress(QMouseEvent *ev);
    void mouseMove(QMouseEvent *ev);
    void mouseRelease(QMouseEvent *ev);
    
public slots:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

private:
    QPoint start;
    QPoint current;
    bool mouse_down;
    
};

#endif // IMAGEDISPLAY_H
