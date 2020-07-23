#include "mywidget.h"
#include "vrepcontrol.h"

Mywidget::Mywidget(QWidget *parent): QWidget(parent),posIndex(0)
{

}
void Mywidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(QColor(0, 160, 230), 4));
    painter.setBrush(Qt::green);
    int viewWidth = this->width();
    int viewHeight = this->height();

    painter.setWindow(0,0,viewWidth,-viewHeight);
    painter.setViewport(viewWidth/2,viewHeight/2,viewWidth, viewHeight);

    painter.drawLine(QPointF(-viewWidth/2, viewHeight/2), QPointF(viewWidth/2, viewHeight/2));
    painter.drawLine(QPointF(-viewWidth/2, -viewHeight/2), QPointF(viewWidth/2, -viewHeight/2));
    painter.drawLine(QPointF(-viewWidth/2, viewHeight/2), QPointF(-viewWidth/2, -viewHeight/2));
    painter.drawLine(QPointF(viewWidth/2, viewHeight/2), QPointF(viewWidth/2, -viewHeight/2));
    painter.drawEllipse(-15, -15, 30, 30);

    if(vrepControl::paintFlag[posIndex] == 1){
        painter.setPen(QPen(Qt::red, 4));
        QPointF pp;
        for(int i = 0 ; i < 360 ; i++){
            pp = QPointF(100*vrepControl::position[posIndex][i][0], 100*vrepControl::position[posIndex][i][1]);
            painter.drawPoint(pp);
        }
        vrepControl::paintFlag[posIndex] = 0;
    }
    painter.end();
}
