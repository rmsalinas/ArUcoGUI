#include <QSplitter>
#include "modulearucotest.h"
#include "moduletools/appparams.h"
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include "arucogparam.h"
using namespace std;
ModuleArucoTest::ModuleArucoTest() {


    vplayer=new modulearucotest::VideoPlayer();
    connect(vplayer,SIGNAL(newImage(cv::Mat&)),this,SLOT(on_newVideoImage(cv::Mat&)));
    setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/aruco_params.png" ) ));
    QToolBar *tbar=new QToolBar("Aruco");
    act_ShowThresImage=  new QAction ( QIcon ( ":/images/blackwhite.png" ), tr ( "&Show thresholded Image..." ), this );
    act_ShowThresImage->setCheckable(true);
    connect(act_ShowThresImage,SIGNAL(triggered()),this,SLOT(on_act_ShowThresImage_triggered( )));
    tbar->addAction(act_ShowThresImage);
    setCentralWidget(vplayer);
    setToolBar(tbar);
}



void ModuleArucoTest::on_global_action(const gparam::ParamSet &paramset){
    if (paramset.getName()=="arucoParamsChanged") redraw();

}

void ModuleArucoTest::getThresholdedImage(cv::Mat &TCopy){
    cv::cvtColor( ArucoMarkerDetector::get(false).getThresholdedImage(0),TCopy,CV_GRAY2BGR);

}

void ModuleArucoTest::redraw(){
    if (currImage.empty()) return;
    cv::Mat im2;
    currImage.copyTo(im2);
    auto markers=ArucoMarkerDetector::get().detect(currImage);
    cv::Mat im2show;
    if (act_ShowThresImage->isChecked()){
        getThresholdedImage(im2show);
    }
    else{
        for(auto m:markers) m.draw(im2);
        im2show=im2;
    }
    vplayer->setImage(im2show);
}

void ModuleArucoTest::on_act_ShowThresImage_triggered(){
    redraw();
}
void ModuleArucoTest::on_newVideoImage(cv::Mat &im){
    im.copyTo(currImage);
    auto markers=ArucoMarkerDetector::get().detect(currImage);
    if (!act_ShowThresImage->isChecked()){
        for(auto m:markers) m.draw(im);
    }
    else{
        getThresholdedImage(im);
    }
}
