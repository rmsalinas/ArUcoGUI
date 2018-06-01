#include "mapperdialog.h"
#include "ui_mapperdialog.h"
#include <QTimer>
#include <QPushButton>
#include <opencv2/highgui.hpp>
#include "marker_mapper/debug.h"
#include <QMessageBox>
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

void mapperdialog::mapperFromVideo(const aruco::CameraParameters &cp,QStringList source,int refMarker,double markerSize,int stepVideo){

    _videoCap.open(source.at(0).toStdString());
    if (!_videoCap.isOpened()){
        QMessageBox::critical ( this,tr ( "Error" ),tr ( "Could not load file:" )+source.at(0));
        reject();
    }
    _Camera=cp;
    setWindowTitle("Mapper From Video");
    _mapper=aruco_mm::MarkerMapper::create();
    _mapper->setParams(_Camera,markerSize,refMarker);
    _mapper->getMarkerDetector().setParameters(ArucoMarkerDetector::get().getParameters());

    _stepVideo=stepVideo;
    ui->te_info->clear();
    _currentSourceIdx=0;
    isFromImages=false;
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
    isFromImages=true;
}

void  mapperdialog::showEvent(QShowEvent *event){
    processNext();

}
bool mapperdialog::processNext_Video(){
    if(!_videoCap.grab()){
        aruco_mm::debug::Debug::setLevel(10);
        aruco_mm::debug::Debug::getStringDebugInfo();

        _mapper->optimize(false);
        ui->te_info->insertPlainText(aruco_mm::debug::Debug::getStringDebugInfo().c_str());
        showOptimizationProgress();
        isOptimizing=true;
        ui->buttonBox->setEnabled(false);

        return false;

    }
    cv::Mat image;
    ui->te_info->insertPlainText( tr("Frame")+"#"+QString::number(_currentSourceIdx)+" ");

    _videoCap.retrieve(image);
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


    ui->te_info ->ensureCursorVisible();
    _currentSourceIdx++;

    for(int i=0;i<_stepVideo;i++) _videoCap.grab();
    return true;
}

bool mapperdialog::processNext_Image(){

    if (_currentSourceIdx>=_Source.count()) {

        aruco_mm::debug::Debug::setLevel(10);
        aruco_mm::debug::Debug::getStringDebugInfo();

        _mapper->optimize(false);
        ui->te_info->insertPlainText(aruco_mm::debug::Debug::getStringDebugInfo().c_str());
        showOptimizationProgress();
        isOptimizing=true;
        ui->buttonBox->setEnabled(false);

        return false;
    }


    ui->te_info->insertPlainText(_Source.at(_currentSourceIdx)+":");
    cv::Mat image=cv::imread(_Source.at(_currentSourceIdx).toStdString());
    if (image.empty()){
        ui->te_info->insertPlainText(tr("Empty image")+"\n");
    }
    else{
        if (image.rows==_Camera.CamSize.width && image.cols==_Camera.CamSize.height ){//auto orient by rotation
            cv::Mat aux;
            cv::transpose(image,aux);
            cv::flip(aux,image,0);
        }
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
    return true;

}

void mapperdialog::    processNext(){

    if (!_mapper)return;

    bool next=false;
    if(isFromImages) next=processNext_Image();
    else next=processNext_Video();


    if(next) QTimer::singleShot(10,this,SLOT(processNext()));
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
