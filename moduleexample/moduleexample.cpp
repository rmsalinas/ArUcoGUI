#include <QSplitter>
#include "moduleexample.h"
#include "moduletools/appparams.h"
#include <iostream>

using namespace std;
ModuleExample::ModuleExample() {
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
    setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/module-3d.png" ) ));
    setToolBar(_tbar);
    setControlPanel(_tbox);

}
void ModuleExample::on_reset_action(){

}
void ModuleExample::onGauss(){

    AppParams::saveToSettings(gauss_params);//save the current params

    if(_gauss_thread) return;//already running
    _gauss_thread=std::make_shared<Gauss_Thread>( gauss_params);

    QObject::connect(_gauss_thread.get(), &Gauss_Thread::finished,this, &ModuleExample::on_gauss_thread_finished);
    QObject::connect(_gauss_thread.get(),&Gauss_Thread::notify_action_progress,this,&ModuleExample::notify_action_progress);
    _gauss_thread->start();
    gparam::ParamSet pset("onGauss");
    pset.push_back(gparam::Param("test",int(1)));
    emit global_action(pset);

}
void ModuleExample::on_gauss_thread_finished(){
_gauss_thread=nullptr;
}

void ModuleExample::readParamSet(){



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
void ModuleExample::onParamsOkPressed(){

}

void ModuleExample::on_activate (  ) {
    getControlPanel()->show();
    getToolBar()->show();

}


void ModuleExample::on_deactivate (  ){

}


void ModuleExample::on_globalaction(const gparam::ParamSet &paramset){
}
