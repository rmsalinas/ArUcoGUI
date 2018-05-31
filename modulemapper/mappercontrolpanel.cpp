#include "mappercontrolpanel.h"
#include "ui_mappercontrolpanel.h"
#include <QListWidgetItem>
#include <QMessageBox>
#include <QSettings>
#include <QFileInfo>
#include <QFileDialog>
#include <QTimer>
#include <QProcess>
#include "marker_mapper/debug.h"
#include <dirent.h>
#include "arucogparam.h"
#include <opencv2/highgui.hpp>
using namespace std;
mapperControlPanel::mapperControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mapperControlPanel)
{
    ui->setupUi(this);
 }

mapperControlPanel::~mapperControlPanel()
{
    delete ui;
}


double mapperControlPanel::getMarkerSize()const{
    return ui->dsp_markerSize->value();
}

int mapperControlPanel::getRefMarker()const{
    int refMarker=-1;
    if(!ui->cb_refMarker->currentText().isEmpty())
        refMarker=ui->cb_refMarker->currentText().toInt();
return refMarker;
}

void mapperControlPanel::on_pb_compute_clicked()
{
emit process();
}




void mapperControlPanel::on_pb_saveMapper_clicked()
{


}

void mapperControlPanel::on_pb_loadCalFile_clicked()
{
    QSettings settings;
    QString file = QFileDialog::getOpenFileName (
                this,
                tr ( "Select an camera calibration file" ),
                settings.value ( "currDir" ).toString(),
                tr ( "Open Camera Calibration (*.yml)" ) );
    if ( file==QString() ) return;

    settings.setValue ( "currDir",QFileInfo ( file ).absolutePath() );
    cameraParamsPath=file ;
aruco::CameraParameters camParams;
    try{
    camParams.readFromXMLFile(file.toStdString());
    }catch(std::exception &ex){
        QMessageBox::critical ( this,tr ( "Error" ),tr ( "Could not load file" )+file);
        return;
    }
    ui->warningIcon->hide();
}

