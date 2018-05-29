#include <QSplitter>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QMessageBox>
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

    //create a panel showing the detections
    QWidget *controlPanel=new QWidget;

     detectionsLabel=new QPlainTextEdit;
     detectionsLabel->setWordWrapMode(QTextOption::NoWrap);
     on_clearDetections();

    QAbstractButton *saveButton = new QPushButton(tr("Save..."));
    saveButton->setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/save.png" )));
    connect(saveButton,SIGNAL(clicked(bool)),this,SLOT(on_saveDetections()));
    QAbstractButton *clearButton = new QPushButton(tr("Clear..."));
    clearButton->setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/reset.png" )));
    connect(clearButton,SIGNAL(clicked(bool)),this,SLOT(on_clearDetections()));
//    connect(openButton, &QAbstractButton::clicked, this, &VideoPlayer::saveDetections);



    QBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(0);

    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(saveButton);


    QBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(detectionsLabel);
    controlLayout->addLayout(buttonLayout);

    controlPanel->setLayout(controlLayout);
    setControlPanel(controlPanel);


}

void ModuleArucoTest::on_clearDetections(){

    detectionsLabel->clear();
    detectionsLabel->insertPlainText("#Frame/Image,NOfDetections,MarkerId,c0_x,c0_y,c1_x,c1_y,c2_x,c2_y,c3_x,c3_y ....\n");
}
void ModuleArucoTest::on_saveDetections(){
    QSettings settings;
    QString filepath = QFileDialog::getSaveFileName(
                vplayer,
                tr ( "Select an output file" ),
                settings.value ( "currDir" ).toString()+"/arucodetections.csv",
                tr ( "Text File (*.csv)" ) );
    if ( filepath==QString() ) return;
    settings.setValue ( "currDir",QFileInfo ( filepath ).absolutePath() );
    QFileInfo qfile(filepath);
    if (qfile.completeSuffix()!="csv")
        filepath+=".csv";
    ofstream file(filepath.toStdString());
    if (!file)
        QMessageBox::critical ( vplayer,tr ( "Error" ),tr ( "Could not open file for writing " )+filepath );
    else file<<detectionsLabel->toPlainText().toStdString();

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
    printDetectionsText(markers);
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

void ModuleArucoTest::printDetectionsText(const std::vector<aruco::Marker> &markers){
QString text=vplayer->getCurrentImageInfo().c_str();
text+=","+QString::number(markers.size());
        for(const auto &m:markers){
            std::stringstream str;
            str<<",  "<<m.id<<",";
            for(int i=0;i<4;i++){
                str<<m[i].x<<","<<m[i].y;
                if (i!=3)str<<",";
            }
            text+=str.str().c_str();
    }
        text+="\n";
        detectionsLabel->insertPlainText(text);
}

void ModuleArucoTest::on_act_ShowThresImage_triggered(){
    redraw();
}
void ModuleArucoTest::on_newVideoImage(cv::Mat &im){
    im.copyTo(currImage);
    auto markers=ArucoMarkerDetector::get().detect(currImage);
    printDetectionsText(markers);
    if (!act_ShowThresImage->isChecked()){
        for(auto m:markers) m.draw(im);
    }
    else{
        getThresholdedImage(im);
    }
}
