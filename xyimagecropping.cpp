#include "xyimagecropping.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFileDialog>
#include <QDebug>

XYImageCropping::XYImageCropping(QWidget *parent)
    : QWidget(parent)
{

}

void XYImageCropping::setImage(const QString &file)
{
    mPixmapFilePath = file;
    reloadPixmap();
}

void XYImageCropping::setPath(const QPainterPath &path)
{
    mCroppingPath = path;
    mPathPos = QPoint(0, 0);
    update();
}

void XYImageCropping::savePixmap()
{
    QString path = QFileDialog::getSaveFileName(this);
    if (!path.isEmpty())
    {
        mPixmap.save(path);
    }
}

void XYImageCropping::reloadPixmap()
{
    mPixmap.load(mPixmapFilePath);
    movePixmapToCenter();
}

void XYImageCropping::cropping()
{
    mCroppingPath.translate(mPathPos - mPixmapPos);

    QPainter painter;
    painter.begin(&mPixmap);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.setBrush(Qt::transparent);
    painter.drawPath(mCroppingPath);
    painter.end();

    clearPath();
}

void XYImageCropping::croppingXored()
{
    mCroppingPath.translate(mPathPos - mPixmapPos);
    QRect rect = mCroppingPath.boundingRect().toRect();
    mPixmap = mPixmap.copy(rect);

    QPixmap mask(rect.size());
    mask.fill(Qt::white);

    QPainter painter;
    painter.begin(&mask);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.setBrush(Qt::transparent);
    painter.drawPath(mCroppingPath.translated(-rect.topLeft()));
    painter.end();

    painter.begin(&mPixmap);
    painter.setCompositionMode(QPainter::CompositionMode_Xor);
    painter.drawPixmap(mPixmap.rect(), mask);
    painter.end();

    clearPath();
    movePixmapToCenter();
}

void XYImageCropping::paintEvent(QPaintEvent *)
{
    if (mPixmap.isNull())
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制图片
    painter.drawPixmap(mPixmapPos, mPixmap);

    // 绘制裁剪路径
    QTransform trs;
    trs.translate(mPathPos.x(), mPathPos.y());
    painter.setTransform(trs);
    painter.drawPath(mCroppingPath);
}

bool XYImageCropping::event(QEvent *event)
{
    static bool pressed = false;
    static QPoint pressedPos;
    static bool movePath = true;

    switch (event->type())
    {
    case QEvent::MouseButtonPress:
        pressedPos = static_cast<QMouseEvent *>(event)->pos();
        if (!mCroppingPath.isEmpty() && mCroppingPath.boundingRect()
                .translated(mPathPos).contains(pressedPos)) {
            pressed = true;
            movePath = true;
        } else if (!mPixmap.isNull() && QRect(mPixmapPos, mPixmap.size()).contains(pressedPos)) {
            pressed = true;
            movePath = false;
        }
        break;
    case QEvent::MouseButtonRelease:
        pressed = false;
        break;
    case QEvent::MouseMove:
        if (pressed) {
            QPoint pos = static_cast<QMouseEvent *>(event)->pos();
            if (movePath) {
                mPathPos += pos - pressedPos;
            } else {
                mPixmapPos += pos - pressedPos;
            }
            pressedPos = pos;
            update();
        }
        break;
    default:
        break;
    }

    return QWidget::event(event);
}

void XYImageCropping::movePixmapToCenter()
{
    if (!mPixmap.isNull())
    {
        QRect geometry = mPixmap.rect();
        int woffset = width() - geometry.width();
        int hoffset = height() - geometry.height();
        mPixmapPos = QPoint(woffset > 0 ? woffset/2:0, hoffset > 0 ? hoffset/2:0);
        update();
    }
}

void XYImageCropping::movePathToCenter()
{
    if (!mCroppingPath.isEmpty())
    {
        QRect geometry = mCroppingPath.boundingRect().toRect();
        int woffset = width() - geometry.width();
        int hoffset = height() - geometry.height();
        mPathPos = QPoint(woffset > 0 ? woffset/2:0, hoffset > 0 ? hoffset/2:0);
        update();
    }
}

void XYImageCropping::clearPath()
{
    mPathPos = QPoint();
    mCroppingPath = QPainterPath();
    update();
}
