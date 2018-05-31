#include "mapperdialog.h"
#include "ui_mapperdialog.h"
#include <QTimer>
#include <QPushButton>
#include <opencv2/highgui.hpp>
#include "marker_mapper/debug.h"
mapperdialog::mapperdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mapperdialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

mapperdialog::~mapperdialog()
{
    delete ui;
}

aruco::MarkerMap mapperdialog::getMarkerMap() {
     return _mmap;
}

void mapperdialog::mapperFromImages(const aruco::CameraParameters &camera,QStringList source,int refMarker,double markerSize)
{

    if (_mapper)return;//already working


    _Source=source;
    _currentSourceIdx=0;
    _Camera=camera;
    setWindowTitle("Mapper From images");
    _mapper=aruco_mm::MarkerMapper::create();
    _mapper->setParams(_Camera,markerSize,refMarker);
    _mapper->getMarkerDetector().setParameters(ArucoMarkerDetector::get().getParameters());

    ui->te_info->clear();
}
void  mapperdialog::showEvent(QShowEvent *event){
    processNextImage();

}

void mapperdialog::    processNextImage(){

    if (!_mapper)return;
if (_currentSourceIdx>=_Source.count()) {

    aruco_mm::debug::Debug::setLevel(10);
    aruco_mm::debug::Debug::getStringDebugInfo();

    _mapper->optimize(false);
    ui->te_info->insertPlainText(aruco_mm::debug::Debug::getStringDebugInfo().c_str());
    showOptimizationProgress();
    isOptimizing=true;
    ui->buttonBox->setEnabled(false);

    return;
}


    ui->te_info->insertPlainText(_Source.at(_currentSourceIdx)+":");
    cv::Mat image=cv::imread(_Source.at(_currentSourceIdx).toStdString());
    if (image.empty()){
       ui->te_info->insertPlainText(tr("Empty image")+"\n");
    }
    else{

       if( image.rows!=_Camera.CamSize.height || image.cols!=_Camera.CamSize.width ){
           ui->te_info->insertPlainText(tr("Can't process image because is not of the dimensions of the camera calibration file provided.")+"\n");
       }
       else{
           _mapper-> process( image ,_currentSourceIdx);
           ui->te_info->insertPlainText(tr("processed.")+"\n");
           _mapper-> drawDetectedMarkers ( image,3);

           QImage _qimgR ( ( const uchar * ) ( image.ptr<uchar> ( 0 ) ),image.cols,image.rows, QImage::Format_RGB888 ) ;
           ui->lb_image -> setPixmap ( QPixmap::fromImage ( _qimgR.rgbSwapped() ) );
       }
    }
    _currentSourceIdx++;
    ui->te_info ->ensureCursorVisible();
    QTimer::singleShot(10,this,SLOT(processNextImage()));
}


void mapperdialog::showOptimizationProgress(){
    if (!_mapper)return;
    if( _mapper->isOptimizationFinished()){
        _mapper->waitForOptimizationFinished();
        isOptimizing=false;
        ui->buttonBox->setEnabled(true);
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    else   QTimer::singleShot(30,this,SLOT(showOptimizationProgress()));
    ui->te_info->insertPlainText(aruco_mm::debug::Debug::getStringDebugInfo().c_str());
    ui->te_info ->ensureCursorVisible();
}


void mapperdialog::on_mapperdialog_finished(int result)
{
    if (!_mapper)return;
    if( !isOptimizing) {
        _mmap=_mapper->getMarkerMap();
        _mapper=nullptr;
    }
}
