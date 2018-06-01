#ifndef MAPPERDIALOG_H
#define MAPPERDIALOG_H

#include <QDialog>
#include "aruco/aruco.h"
#include "arucogparam.h"
#include "marker_mapper/markermapper.h"
#include <opencv2/highgui.hpp>
namespace Ui {
class mapperdialog;
}

class mapperdialog : public QDialog
{
    Q_OBJECT

public:
    explicit mapperdialog(QWidget *parent = 0);
    void mapperFromImages(const aruco::CameraParameters &cp,QStringList source,int refMarker,double markerSize);
    void mapperFromVideo(const aruco::CameraParameters &cp,QStringList source,int refMarker,double markerSize,int stepVideo);
    aruco::MarkerMap getMarkerMap() ;
 ~mapperdialog();
public slots:
    void processNext();
    void showOptimizationProgress();
    bool processNext_Image();
    bool processNext_Video();
   protected:
    void  showEvent(QShowEvent *event);

private slots:
    void on_mapperdialog_finished(int result);

private:
    Ui::mapperdialog *ui;
    QStringList _Source;
    bool isOptimizing=false;
    int _currentSourceIdx;
    aruco::CameraParameters _Camera;
    std::shared_ptr<aruco_mm::MarkerMapper> _mapper;
    aruco::MarkerMap _mmap;
    int _stepVideo;
    bool isFromImages=true;

    cv::VideoCapture _videoCap;
};

#endif // MAPPERDIALOG_H
