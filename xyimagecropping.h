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

    void savePixmap();
    void reloadPixmap();
    void cropping();
    void croppingXored();

protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);

private:
    void movePixmapToCenter();
    void movePathToCenter();
    void clearPath();

private:
    QPainterPath mCroppingPath;
    QPoint       mPathPos;
    QImage       mImage;
    QPoint       mImagePos;

    QString      mImageFilePath;
};

#endif // XYIMAGECROPPING_H
