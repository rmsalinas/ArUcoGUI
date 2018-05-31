#include <QtGui>
#include <QBoxLayout>
#include "sglviewer.h"
#include <iostream>
SglViewer::SglViewer(QWidget *parent){
    setScaledContents(true);
    setMinimumSize(QSize(640,480));
    setPixmap(QPixmap ( QString:: fromUtf8 ( ":/images/cityoflove.jpg" ) ));

    aruco::MarkerMap mmap;
    mmap.readFromFile("/home/salinas/map.yml");
    setParams(mmap);
    redraw();
}

void	SglViewer::moveEvent(QMoveEvent *event){
}

void	SglViewer::mouseDoubleClickEvent(QMouseEvent *event){

}
void	SglViewer::mouseMoveEvent(QMouseEvent *event){
    bool needUpdate=false;
    if (mouseAction==MA_TRANSLATE){
        float xdif=event->windowPos().x() - prevMousePos.x();
        float ydif=event->windowPos().y() - prevMousePos.y();
        _drawer.translate(xdif*0.01*strengthFactor,ydif*0.01*strengthFactor);
        needUpdate=true;
    }
    else if(mouseAction==MA_ROTATE){
        float xdif=event->windowPos().x() - prevMousePos.x();
        float ydif=event->windowPos().y() - prevMousePos.y();
        _drawer.rotate(ydif*0.01*strengthFactor,xdif*0.01*strengthFactor);
        needUpdate=true;
    }
    else{

    }


    if (needUpdate){
        prevMousePos=event->windowPos();
        redraw();
    }
}
void	SglViewer::mousePressEvent(QMouseEvent *event){
     if (event->button()== Qt::LeftButton){
        mouseAction=MA_TRANSLATE;
        prevMousePos=event->windowPos();
    }
    else if( event->button()== Qt::RightButton){
        mouseAction=MA_ROTATE;
        prevMousePos=event->windowPos();
    }
}

void	SglViewer::mouseReleaseEvent(QMouseEvent *event){
    mouseAction=MA_NONE;
}

void	SglViewer::wheelEvent(QWheelEvent *event){
    std::cerr<<"wheelEvent"<<std::endl;
    QPoint numDegrees = event->angleDelta() / 8;
   _drawer.zoom(numDegrees .ry()*0.025*strengthFactor);
    redraw();

}

void SglViewer::setParams(const aruco::MarkerMap &mmap,int w,int h){

    _w=w;
    _h=h;
    _image.create(_h,_w,CV_8UC3);
    _drawer.setParams(_w,_h,mmap);
}


void SglViewer::redraw(){
    if (_image.empty()){
        setPixmap(QPixmap ( QString:: fromUtf8 ( ":/images/cityoflove.jpg" ) ));
    }
    else{
        _drawer.draw(_image,true);
        QImage _qimgR ( ( const uchar * ) ( _image.ptr<uchar> ( 0 ) ),
                        _image.cols,_image.rows, QImage::Format_RGB888 ) ;

       setPixmap ( QPixmap::fromImage ( _qimgR.rgbSwapped() ) );

    }
}
