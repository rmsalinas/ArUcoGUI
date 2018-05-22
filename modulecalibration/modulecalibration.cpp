#include <QSplitter>
#include "modulecalibration.h"
#include "moduletools/appparams.h"
#include <iostream>

using namespace std;
ModuleCalibration::ModuleCalibration() {


    vplayer=new VideoPlayer();
    _tbar=new QToolBar ( getName().c_str() );
    reset_action= new QAction ( QIcon ( ":/images/reset.png" ), tr ( "&Reset..." ), this );
    connect(reset_action,SIGNAL(triggered()),this,SLOT(on_reset_action( )));
    _tbar->addAction(reset_action);




    readParamSet();
    _tbox=new QToolBox();
    gparam::ParamSetWdgt *gpwgt;
    gpwgt=new gparam::ParamSetWdgt ( &gauss_params,0,QDialogButtonBox::Ok );
    connect ( gpwgt,SIGNAL ( okBtnPressed() ),this,SLOT ( onGauss() ) );
    _tbox->addItem ( gpwgt,tr ( "Gauss Filter" ) );



    //register the elements created
    setCentralWidget(vplayer);
    setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/module-3d.png" ) ));
    setToolBar(_tbar);
    setControlPanel(_tbox);

}
void ModuleCalibration::on_reset_action(){

}
void ModuleCalibration::onGauss(){

    VideoPlayer *vp=new VideoPlayer();
    vp->show();
    return;
    AppParams::saveToSettings(gauss_params);//save the current params

    if(_gauss_thread) return;//already running
    _gauss_thread=std::make_shared<Gauss_Thread>( gauss_params);

    QObject::connect(_gauss_thread.get(), &Gauss_Thread::finished,this, &ModuleCalibration::on_gauss_thread_finished);
    QObject::connect(_gauss_thread.get(),&Gauss_Thread::notify_action_progress,this,&ModuleCalibration::notify_action_progress);
    _gauss_thread->start();
    gparam::ParamSet pset("onGauss");
    pset.push_back(gparam::Param("test",int(1)));
    emit global_action(pset);

}
void ModuleCalibration::on_gauss_thread_finished(){
_gauss_thread=nullptr;
}

void ModuleCalibration::readParamSet(){



    gauss_params.setName("GaussParams");
    gauss_params.push_back ( gparam::Param ( "wSize",int (3),3,13,2 ) );
    gauss_params.back().setDescription ( "Size of the filter" );
    gauss_params.push_back ( gparam::Param ( "sigma",float(1),1.,5.,0.1 ) );
    gauss_params.back().setDescription ( "Sigma of filter" );
    gauss_params.push_back ( gparam::Param ( "nTimes",int (1),1,10,1 ) );
    gauss_params.back().setDescription ( "Number of times the operation is repeated" );

    if (! AppParams::readFromSettings(gauss_params) )
        AppParams::saveToSettings(gauss_params);



}
void ModuleCalibration::onParamsOkPressed(){

}

void ModuleCalibration::on_activate (  ) {
    getControlPanel()->show();
    getToolBar()->show();

}


void ModuleCalibration::on_deactivate (  ){

}


void ModuleCalibration::on_globalaction(const gparam::ParamSet &paramset){
}
