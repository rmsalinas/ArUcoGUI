#include "mainwindow.h"
#include "arucogparam.h"
#include <iostream>
#include "modulearucotest/modulearucotest.h"
#include "modulecalibration/modulecalibration.h"
#include "moduletools/appparams.h"


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
        connect(arucoParamsShowAction,SIGNAL(triggered()),this,SLOT(on_arucoParamsShowAction_triggered( )));
        _tbar->addAction(arucoParamsShowAction);
        addToolBar ( _tbar);


        //ARUCO PARAMS AND DOCK
        AppParams::readFromSettings(ArucoGParams::get());
        gparam::ParamSetWdgt* _arucoWdgt= new gparam::ParamSetWdgt (&ArucoGParams::get(),0 );
        connect(_arucoWdgt,SIGNAL(paramChanged(int)),this,SLOT(on_ArucoParamsChanged()));
         _arucoDock=new QDockWidget (  "Aruco Params" );
        _arucoDock->setWidget (  _arucoWdgt);
        addDockWidget ( Qt::TopDockWidgetArea, _arucoDock );


        //ADD MODULES
        addModule ( "ArucoTest", std::make_shared< ModuleArucoTest> () );
         addModule ( "Calibration", std::make_shared< ModuleCalibration> () );
        activateModule("ArucoTest");
    } catch ( std::exception &ex ) {
        cerr<<ex.what() <<endl;
    }



}

void MainWindow::on_global_action(const gparam::ParamSet &paramset){
cerr<<paramset<<endl;
}

void MainWindow::on_ArucoParamsChanged(){
    AppParams::saveToSettings(ArucoGParams::get());
    emit global_action_triggered( gparam::ParamSet("arucoParamsChanged"));

}
void MainWindow::on_arucoParamsShowAction_triggered(){
    if(!arucoParamsShowAction->isChecked())
        _arucoDock->hide();
    else _arucoDock->show();
}

