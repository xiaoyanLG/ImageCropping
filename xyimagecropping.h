#ifndef XYIMAGECROPPING_H
#define XYIMAGECROPPING_H

#include <QWidget>

class XYImageCropping : public QWidget
{
    Q_OBJECT
public:
    explicit XYImageCropping(QWidget *parent = 0);

    void setImage(const QString &file);
    void setPath(const QPainterPath &path);

protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);

private:
    void movePixmapToCenter();
    void movePathToCenter();

private:
    QPainterPath mCroppingPath;
    QPoint       mPathPos;
    QPixmap      mPixmap;
    QPoint       mPixmapPos;
};

#endif // XYIMAGECROPPING_H
