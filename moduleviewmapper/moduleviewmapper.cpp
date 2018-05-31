#include <QSplitter>
#include "moduleviewmapper.h"
#include "moduletools/appparams.h"
#include <iostream>

using namespace std;
ModuleViewMapper::ModuleViewMapper() {
    _labelImage=new QLabel(0);
    _labelImage->setScaledContents(true);
    _labelImage->setPixmap(QPixmap ( QString:: fromUtf8 ( ":/images/cityoflove.jpg" ) ));


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
    setCentralWidget(_labelImage);
    setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/aruco_visualizer.png" ) ));
    setToolBar(_tbar);
    setControlPanel(_tbox);

}
void ModuleViewMapper::on_reset_action(){

}
void ModuleViewMapper::onGauss(){

    AppParams::saveToSettings(gauss_params);//save the current params

    if(_gauss_thread) return;//already running
    _gauss_thread=std::make_shared<Gauss_Thread>( gauss_params);

    QObject::connect(_gauss_thread.get(), &Gauss_Thread::finished,this, &ModuleViewMapper::on_gauss_thread_finished);
    QObject::connect(_gauss_thread.get(),&Gauss_Thread::notify_action_progress,this,&ModuleViewMapper::notify_action_progress);
    _gauss_thread->start();
    gparam::ParamSet pset("onGauss");
    pset.push_back(gparam::Param("test",int(1)));
    emit global_action_triggered(pset);

}
void ModuleViewMapper::on_gauss_thread_finished(){
_gauss_thread=nullptr;
}

void ModuleViewMapper::readParamSet(){



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
void ModuleViewMapper::onParamsOkPressed(){

}

void ModuleViewMapper::on_activate (  ) {
    getControlPanel()->show();
    getToolBar()->show();

}


void ModuleViewMapper::on_deactivate (  ){

}


void ModuleViewMapper::on_globalaction(const gparam::ParamSet &paramset){
}
