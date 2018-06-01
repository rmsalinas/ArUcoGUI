#include <QSplitter>
#include <QMessageBox>
#include <QFileDialog>
#include "moduleviewmapper.h"
#include "moduletools/appparams.h"
#include "mapperscenedrawer.h"
#include <iostream>
using namespace std;
ModuleViewMapper::ModuleViewMapper() {
    _sglViewer=new SglViewer(0);
    setCentralWidget(_sglViewer);
    setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/aruco_visualizer.png" ) ));
    QAction *openFile=  new QAction ( QIcon ( ":/images/open.png" ), tr ( "&Open Marker map..." ), this );
    connect(openFile,&QAction::triggered,this,&ModuleViewMapper::onFileOpenMarkerMap);

    QToolBar *tbar=new QToolBar();
    tbar->addAction(openFile);
    setToolBar(tbar);

}


void ModuleViewMapper::setMarkerMap(const aruco::MarkerMap &map){
    _sglViewer->setDrawer(std::make_shared<MapperSceneDrawer>(map));

}


void ModuleViewMapper::on_globalaction(const gparam::ParamSet &paramset){
}


void ModuleViewMapper::onFileOpenMarkerMap(){
    QSettings settings;
    QString file = QFileDialog::getOpenFileName (
                _sglViewer,
                tr ( "Select an marker map file" ),
                settings.value ( "currDir" ).toString(),
                tr ( "Open Marker Map file (*.yml)" ) );
    if ( file==QString() ) return;

    settings.setValue ( "currDir",QFileInfo ( file ).absolutePath() );
    aruco::MarkerMap mmap;

    try{
        mmap.readFromFile(file.toStdString());
    }catch(std::exception &ex){
        QMessageBox::critical ( _sglViewer,tr ( "Error" ),tr ( "Could not load marker map from file:" )+file);
        return;
    }
    setMarkerMap(mmap);

}
