#include <QSplitter>
#include <QTimer>
#include "modulemapper.h"
#include "moduletools/appparams.h"
#include <iostream>
#include "arucogparam.h"
#include "moduleviewmapper/mapperscenedrawer.h"
#include <mapperdialog.h>
#include <QMessageBox>
using namespace std;
ModuleMapper::ModuleMapper() {
    setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/aruco_map.png" ) ));

     vplayer=new VideoPlayer();
     connect(vplayer,SIGNAL(openedImageOrVideo()),this,SLOT(on_vplayer_opened()));


    Btn_gotoNextFrame=new QPushButton(tr("&Next Image..."));
    connect(Btn_gotoNextFrame, &QAbstractButton::clicked, vplayer, &VideoPlayer::playNextFrame);
    Btn_gotoNextFrame->setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/arrow-right-small.png" )));
    Btn_gotoNextFrame->hide();
    vplayer->addButton(Btn_gotoNextFrame);





    //register the elements created
    setCentralWidget(vplayer);


    CalibPanel=new mapperControlPanel();
     connect(CalibPanel,&mapperControlPanel::process,this,&ModuleMapper::on_process);
    setControlPanel(CalibPanel);
}



void ModuleMapper::on_activate(){


}


void ModuleMapper::on_process(){


    if (CalibPanel->getCameraPath().isEmpty())return;
    QStringList source=vplayer->getSource();
    if (source.count()==0)return;


    aruco::CameraParameters Camera;
    try{
    Camera.readFromXMLFile(CalibPanel->getCameraPath().toStdString());
    }catch(std::exception &ex){
        QMessageBox::critical ( vplayer,tr ( "Error" ),tr ( "Could not load file ")+CalibPanel->getCameraPath());
        return;
    }

    mapperdialog *mdlg=new mapperdialog();
    mdlg->mapperFromImages(Camera,source,CalibPanel->getRefMarker(),CalibPanel->getMarkerSize());
    auto res=mdlg->exec();
    if (res==QDialog::Accepted){
        CalibPanel->setMarkerMapInfo(mdlg->getMarkerMap());
        _mmap=mdlg->getMarkerMap();
        emit global_action_triggered(gparam::ParamSet("NewMarkerMapComputed"));
     }

}




void ModuleMapper::on_vplayer_opened(){
     Btn_gotoNextFrame->show();
}





void ModuleMapper::on_global_action(const gparam::ParamSet &paramset){
    if (paramset.getName()=="arucoParamsChanged")
        vplayer->updateImage();
}

