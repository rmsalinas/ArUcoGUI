

#ifndef _SGLVIEWER_H
#define _SGLVIEWER_H

#include <QWidget>
#include <QLabel>
#include "sglviewer_exports.h"
#include "mapperscenedrawer.h"

class APP_SGLVIEWER_TOOLS_API SglViewer : public QLabel
{
    Q_OBJECT
public:
    SglViewer(QWidget *parent  = nullptr);


    void setParams(const aruco::MarkerMap &mmap,int w=1280,int h=960);
public slots:
    void redraw();

private:

    MapperSceneDrawer _drawer;
    int _w=1280,_h=960;
    aruco::MarkerMap _mmap;
    cv::Mat _image;


    void	 moveEvent(QMoveEvent *event);
    void	mouseDoubleClickEvent(QMouseEvent *event);
    void	mouseMoveEvent(QMouseEvent *event);
    void	mousePressEvent(QMouseEvent *event);
    void	mouseReleaseEvent(QMouseEvent *event);
    void	wheelEvent(QWheelEvent *event);


    QPointF prevMousePos;
    enum MouseActions: int{MA_NONE,MA_TRANSLATE,MA_ROTATE,MA_ZOOM};
    int mouseAction=MA_NONE;

    float strengthFactor=1;

};
#endif

