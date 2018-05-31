#ifndef MODULEVIEWMAPPER_MODULE_H
#define MODULEVIEWMAPPER_MODULE_H



#include <QToolBox>
#include <QWidget>
#include <QThread>
#include <memory>
#include "moduletools/appmodule.h"
#include "gparam/paramsetwdgt.h"
#include <iostream>
#include "moduleviewmapper_exports.h"
#include "sglviewer/sglviewer.h"
#include "aruco/aruco.h"
class APP_MODULEVIEWMAPPER_TOOLS_API  ModuleViewMapper: public AppModule {
    Q_OBJECT
public:
    ModuleViewMapper();
    std::string getName() const {return "ModuleViewMapper";}
    std::string getToolBoxTitle() const {return "";}


    void setMarkerMap(const aruco::MarkerMap &map);
public slots:
    void on_globalaction(const gparam::ParamSet &paramset);

private slots:
private:
    SglViewer *_sglViewer;
    aruco::MarkerMap map;



};



#endif // TSDFTOOLBOX_H
