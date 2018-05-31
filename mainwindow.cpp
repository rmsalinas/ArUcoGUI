#include "mainwindow.h"

#include <iostream>
#include "modulearucotest/modulearucotest.h"
#include "modulecalibration/modulecalibration.h"
#include "modulemapper/modulemapper.h"
#include "moduletools/appparams.h"
#include "arucogparam.h"

using namespace std;


MainWindow::MainWindow ( QWidget *parent  ) :
    ModuleSetMainWindow ( parent )  {
    try {
        setWindowTitle ( "ArUco GUI" );
        setWindowIcon(QIcon ( ":/images/aruco_params.png" ));
        resize ( 640,480 );

        _tbar=new QToolBar (  "Main Toolbar" );

        arucoParamsShowAction=new QAction ( QIcon ( ":/images/aruco_params.png" ), tr ( "&Aruco Params" ), this );
        arucoParamsShowAction->setCheckable(true);
        arucoParamsShowAction->setChecked(true);


//        connect(arucoParamsShowAction,SIGNAL(triggered()),this,SLOT(on_arucoParamsShowAction_triggered( )));
        _tbar->addAction(arucoParamsShowAction);
        addToolBar ( _tbar);


        //ARUCO PARAMS AND DOCK
        AppParams::readFromSettings(ArucoGParams::get());
        _arucoWdgt= new gparam::ParamSetWdgt (&ArucoGParams::get(),0 );
        connect(_arucoWdgt,SIGNAL(paramChanged(int)),this,SLOT(on_ArucoParamsChanged()));
        _arucoDock=new QDockWidget (  "Aruco Params" );
        _arucoDock->setWidget (  _arucoWdgt);
        addDockWidget ( Qt::LeftDockWidgetArea, _arucoDock );

        connect(arucoParamsShowAction,&QAction::triggered, _arucoDock,&QDockWidget::setVisible);

        //ADD MODULES
        addModule ( "ArucoTest", std::make_shared< ModuleArucoTest> () );
        addModule ( "Calibration", std::make_shared< ModuleCalibration> () );
        addModule ( "Mapper", std::make_shared< ModuleMapper> () );
        activateModule("ArucoTest");
    } catch ( std::exception &ex ) {
        cerr<<ex.what() <<endl;
    }
}

void MainWindow::on_module_activated(std::string moduleName,ModuleInfo minfo){
    cerr<<"Module activated:" <<moduleName<<endl;
    if (moduleName=="Calibration"){
        //change detection to ARUCO_MIP_32
       ArucoGParams::get()["Dictionary"] ="ARUCO_MIP_36h12";
        //update
        _arucoWdgt->setParamSet(&ArucoGParams::get());
        on_ArucoParamsChanged();
    }

}

void MainWindow::on_global_action(const gparam::ParamSet &paramset){
    if (paramset.getName()=="showArucoParams"){
        _arucoDock->show();
        arucoParamsShowAction->setChecked(true);
    }
    if (paramset.getName()=="hideArucoParams"){
        _arucoDock->hide();
        arucoParamsShowAction->setChecked(false);
    }

cerr<<paramset<<endl;
}

void MainWindow::on_ArucoParamsChanged(){
    AppParams::saveToSettings(ArucoGParams::get());
    emit global_action_triggered( gparam::ParamSet("arucoParamsChanged"));

}

