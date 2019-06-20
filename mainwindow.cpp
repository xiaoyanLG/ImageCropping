#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    installEventFilter(this);
    ui->widget->installEventFilter(this);

    connect(ui->radioButton, &QRadioButton::clicked, this, [this](){
        this->mCurShape = Rect;
    });
    connect(ui->radioButton_2, &QRadioButton::clicked, this, [this](){
        this->mCurShape = Ellipse;
    });
    connect(ui->radioButton_3, &QRadioButton::clicked, this, [this](){
        this->mCurShape = Polygon;
    });
    connect(ui->radioButton_4, &QRadioButton::clicked, this, [this](){
        this->mCurShape = USER;
    });

    mCurShape = Rect;
    ui->radioButton->clicked(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    static bool pathMode = false;
    static bool pressed = false;
    static bool startDraw = false;
    static QPoint pressedPos;

    switch (event->type())
    {
    case QEvent::KeyPress:
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Control) {
            mDrawPath = QPainterPath();
            pathMode = true;
        }
        break;
    case QEvent::KeyRelease:
        if (static_cast<QKeyEvent *>(event)->key() == Qt::Key_Control) {
            pathMode = false;
            startDraw = false;

            if (mCurShape == Polygon && !mDrawPath.isEmpty()) {
                mDrawPath.lineTo(pressedPos);
                ui->widget->setPath(mDrawPath);
            }
        }
        break;
    default:
        break;
    }

    if (ui->widget == obj && pathMode)
    {
        switch (event->type())
        {
        case QEvent::MouseButtonPress:
            if (mCurShape == Polygon) {
                QPoint pos = static_cast<QMouseEvent *>(event)->pos();
                if (!startDraw) {
                    mDrawPath.moveTo(pos);
                } else {
                    mDrawPath.lineTo(pos);
                    ui->widget->setPath(mDrawPath);
                    return true;
                }
            }
            pressed = true;
            startDraw = true;
            pressedPos = static_cast<QMouseEvent *>(event)->pos();
            return true;
        case QEvent::MouseButtonRelease:
            pressed = false;
            return true;
        case QEvent::MouseMove:
            if (pressed) {
                QPoint curPos = static_cast<QMouseEvent *>(event)->pos();
                int mx = qMin(curPos.x(), pressedPos.x());
                int my = qMin(curPos.y(), pressedPos.y());
                int w = qAbs(curPos.x() - pressedPos.x());
                int h = qAbs(curPos.y() - pressedPos.y());
                QPainterPath tmpPath;
                switch (mCurShape)
                {
                case Rect:
                    tmpPath.addRect(QRect(mx, my, w, h));
                    mDrawPath = tmpPath;
                    ui->widget->setPath(mDrawPath);
                    break;
                case Ellipse:
                    tmpPath.addEllipse(QRect(mx, my, w, h));
                    mDrawPath = tmpPath;
                    ui->widget->setPath(mDrawPath);
                    break;
                }
            }
            return true;
        default:
            break;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::on_pushButton_clicked() // 选择图片
{
    QString file = QFileDialog::getOpenFileName(this);
    if (!file.isEmpty())
    {
        ui->widget->setImage(file);
    }
}

void MainWindow::on_pushButton_3_clicked() // 裁剪
{
    ui->widget->cropping();
}

void MainWindow::on_pushButton_4_clicked() // 反向裁剪
{
    ui->widget->croppingXored();
}

void MainWindow::on_pushButton_2_clicked() // 保存
{
    ui->widget->savePixmap();
}

void MainWindow::on_pushButton_5_clicked() // 还原
{
    ui->widget->reloadPixmap();
}
