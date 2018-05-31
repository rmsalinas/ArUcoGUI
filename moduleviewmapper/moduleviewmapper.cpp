#include <QSplitter>
#include "moduleviewmapper.h"
#include "moduletools/appparams.h"
#include "mapperscenedrawer.h"
#include <iostream>

using namespace std;
ModuleViewMapper::ModuleViewMapper() {
    _sglViewer=new SglViewer(0);
    setCentralWidget(_sglViewer);
    setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/aruco_visualizer.png" ) ));

}


void ModuleViewMapper::setMarkerMap(const aruco::MarkerMap &map){
    _sglViewer->setDrawer(std::make_shared<MapperSceneDrawer>(map));

}


void ModuleViewMapper::on_globalaction(const gparam::ParamSet &paramset){
}
