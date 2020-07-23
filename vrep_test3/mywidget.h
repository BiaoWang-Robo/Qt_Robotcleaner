#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>

#include<QPainter>

class Mywidget : public QWidget
{
    Q_OBJECT
public:
    int posIndex;

    explicit Mywidget(QWidget *parent = NULL);

    void paintEvent(QPaintEvent *event);
};

#endif // MYWIDGET_H
