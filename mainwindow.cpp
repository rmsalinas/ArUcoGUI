#include <QMenuBar>
#include <QMessageBox>
#include "mainwindow.h"
#include <iostream>
#include "modulearucotest/modulearucotest.h"
#include "modulecalibration/modulecalibration.h"
#include "modulemapper/modulemapper.h"
#include "moduletools/appparams.h"
#include "moduleviewmapper/moduleviewmapper.h"
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
        addModule ( "Map Viewer", std::make_shared< ModuleViewMapper> () );


        //Add menus

        FileMenu = menuBar()->addMenu(tr("&File"));
        act_openMarkerMap= new QAction(tr("Open &Marker Map"), this);
        connect(act_openMarkerMap,SIGNAL(triggered(bool)),this,SLOT(onFileOpenMarkerMap()));
        act_exit= new QAction(tr("&Exit"), this);
        connect(act_exit,SIGNAL(triggered(bool)),this,SLOT(onExit()));

        FileMenu->addAction(act_openMarkerMap);
        FileMenu->addAction(act_exit);

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
    if(moduleName=="Map Viewer")
        setArucoParamsDockVisible(false);

}

void MainWindow::setArucoParamsDockVisible(bool v)
{
    if (v){
        _arucoDock->show();
        arucoParamsShowAction->setChecked(true);
    }
    else{
        _arucoDock->hide();
        arucoParamsShowAction->setChecked(false);
    }
}

void MainWindow::on_global_action(const gparam::ParamSet &paramset){
    if (paramset.getName()=="showArucoParams")
        setArucoParamsDockVisible(true);
    if (paramset.getName()=="hideArucoParams")
        setArucoParamsDockVisible(false);
    if (paramset.getName()=="NewMarkerMapComputed"){
        getModuleMap() ["Map Viewer"].cast<ModuleViewMapper> ()->setMarkerMap(
                    getModuleMap() ["Mapper"].cast<ModuleMapper>()->getMarkerMapGenerated()
                );
        activateModule("Map Viewer");

    }
}

void MainWindow::on_ArucoParamsChanged(){
    AppParams::saveToSettings(ArucoGParams::get());
    emit global_action_triggered( gparam::ParamSet("arucoParamsChanged"));

}


void MainWindow::onFileOpenMarkerMap(){
//    QSettings settings;
//    QString file = QFileDialog::getOpenFileName (
//                this,
//                tr ( "Select an marker map file" ),
//                settings.value ( "currDir" ).toString(),
//                tr ( "Open Marker Map file (*.yml)" ) );
//    if ( file==QString() ) return;

//    settings.setValue ( "currDir",QFileInfo ( file ).absolutePath() );
//    aruco::MarkerMap mmap;

//    try{
//        mmap.readFromFile(file.toStdString());
//    }catch(std::exception &ex){
//        QMessageBox::critical ( this,tr ( "Error" ),tr ( "Could not load marker map from file:" )+file);
//        return;
//    }
//    //open the file in the appropriate module
    activateModule("Map Viewer");

    getModuleMap() ["Map Viewer"].cast<ModuleViewMapper> ()-> onFileOpenMarkerMap();


}
void MainWindow::onExit(){

}
