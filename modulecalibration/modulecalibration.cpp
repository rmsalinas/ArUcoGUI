#include <QSplitter>
#include <QTimer>
#include "modulecalibration.h"
#include "moduletools/appparams.h"
#include <iostream>
#include "arucogparam.h"
using namespace std;
ModuleCalibration::ModuleCalibration() {


    vplayer=new VideoPlayer();
     connect(vplayer,SIGNAL(openedImageOrVideo()),this,SLOT(on_vplayer_opened()));

    Btn_addCurImage = new QPushButton(tr("&Add Current..."));
    connect(Btn_addCurImage, &QAbstractButton::clicked, this, &ModuleCalibration::on_addCurrent);
    Btn_addCurImage->setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/plus.png" )));
    Btn_addCurImage->hide();
    vplayer->addButton(Btn_addCurImage);



    Btn_gotoNextFrame=new QPushButton(tr("&Next Image..."));
    connect(Btn_gotoNextFrame, &QAbstractButton::clicked, vplayer, &VideoPlayer::playNextFrame);
    Btn_gotoNextFrame->setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/arrow-right-small.png" )));
    Btn_gotoNextFrame->hide();
    vplayer->addButton(Btn_gotoNextFrame);

    Btn_addAllImages=new QPushButton(tr("&Add All..."));
    connect(Btn_addAllImages, &QAbstractButton::clicked, this, &ModuleCalibration::on_addAll);
    Btn_addAllImages->setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/done.png" )));
    Btn_addAllImages->hide();
    vplayer->addButton(Btn_addAllImages);





    //register the elements created
    setCentralWidget(vplayer);
    setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/module-3d.png" ) ));
   // setToolBar(_tbar);
    CalibPanel=new calibrationControlPanel();
    setControlPanel(CalibPanel);

}

void ModuleCalibration::on_addCurrent(){

    if( vplayer->getDetectedMarkers().size()==0)return;
    CalibPanel->add(vplayer->getShownImage(),vplayer->getDetectedMarkers(),QString("Frame: #")+vplayer->getCurrentImageInfo().c_str() );
    vplayer->playNextFrame();
}

void ModuleCalibration::on_addAll(){

    if( vplayer->getDetectedMarkers().size()!=0)
        CalibPanel->add(vplayer->getShownImage(),vplayer->getDetectedMarkers(),QString("Frame: #")+vplayer->getCurrentImageInfo().c_str() );
    if(    vplayer->playNextFrame())
         QTimer::singleShot(30,this,SLOT(on_addAll()));
}


void ModuleCalibration::on_vplayer_opened(){
    Btn_addCurImage->show();
    Btn_gotoNextFrame->show();
    if(vplayer->isVideo())
        Btn_addAllImages->hide();
    else Btn_addAllImages->show();
}






void ModuleCalibration::on_global_action(const gparam::ParamSet &paramset){
    if (paramset.getName()=="arucoParamsChanged")
        vplayer->updateImage();
}


