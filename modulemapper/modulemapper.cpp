#include <QSplitter>
#include <QTimer>
#include "modulemapper.h"
#include "moduletools/appparams.h"
#include <iostream>
#include "arucogparam.h"
#include "sglviewer/mapperscenedrawer.h"
#include <mapperdialog.h>
#include <QMessageBox>
using namespace std;
ModuleMapper::ModuleMapper() {

    stckWdtgs=new QStackedWidget();
    vplayer=new VideoPlayer();
    sglviewer=new SglViewer();
    connect(vplayer,SIGNAL(openedImageOrVideo()),this,SLOT(on_vplayer_opened()));
    stckWdtgs->addWidget(vplayer);
    stckWdtgs->addWidget(sglviewer);


    Btn_gotoNextFrame=new QPushButton(tr("&Next Image..."));
    connect(Btn_gotoNextFrame, &QAbstractButton::clicked, vplayer, &VideoPlayer::playNextFrame);
    Btn_gotoNextFrame->setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/arrow-right-small.png" )));
    Btn_gotoNextFrame->hide();
    vplayer->addButton(Btn_gotoNextFrame);





    //register the elements created
    setCentralWidget(stckWdtgs);
    setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/mapper.png" ) ));


    _tbar=new QToolBar("Mapper");
    act_View3D=  new QAction ( QIcon ( ":/images/icon3d.png" ), tr ( "&Show 3D..." ), this );
    act_View3D->setCheckable(true);
    connect(act_View3D,SIGNAL(triggered()),this,SLOT(on_act_View3D_triggered( )));
    _tbar->addAction(act_View3D);
    setToolBar(_tbar);


   // setToolBar(_tbar);
    CalibPanel=new mapperControlPanel();
     connect(CalibPanel,&mapperControlPanel::process,this,&ModuleMapper::on_process);
    setControlPanel(CalibPanel);
}

void ModuleMapper::show3DView(){
    act_View3D->setChecked(true);
     stckWdtgs->setCurrentIndex(1);
     getControlPanel()->hide();
     emit global_action_triggered(gparam::ParamSet("hideArucoParams"));

}
void ModuleMapper::showImageViewer(){
    stckWdtgs->setCurrentIndex(0);
    getControlPanel()->show();
}

void ModuleMapper::on_act_View3D_triggered( ){

    if (act_View3D->isChecked())
        show3DView();
    else
        showImageViewer();

}

void ModuleMapper::on_activate(){
    if (stckWdtgs->currentIndex()==1 ){
         emit global_action_triggered(gparam::ParamSet("hideArucoParams"));
        getControlPanel()->hide();

    }

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
         sglviewer->setDrawer(std::make_shared<MapperSceneDrawer>(mdlg->getMarkerMap()));
        show3DView();
    }

}




void ModuleMapper::on_vplayer_opened(){
     Btn_gotoNextFrame->show();
}





void ModuleMapper::on_global_action(const gparam::ParamSet &paramset){
    if (paramset.getName()=="arucoParamsChanged")
        vplayer->updateImage();
}

