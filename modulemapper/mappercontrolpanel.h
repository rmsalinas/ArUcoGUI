#ifndef MAPPERCONTROLPANEL_H
#define MAPPERCONTROLPANEL_H

#include <QWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QThread>
#include "aruco/aruco.h"
#include <opencv2/core.hpp>
#include "marker_mapper/markermapper.h"

namespace Ui {
class mapperControlPanel;
}
  class mapperControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit mapperControlPanel(QWidget *parent = 0);
    ~mapperControlPanel();

     QString getCameraPath()const{return cameraParamsPath;}
    int getRefMarker()const;
    double getMarkerSize()const;
private slots:



    void on_pb_compute_clicked();

    void on_pb_saveMapper_clicked();

    void on_pb_loadCalFile_clicked();


 signals:
     void process();

 public:

private:
    Ui::mapperControlPanel *ui;

   QString cameraParamsPath;

 };


#endif // MAPPERCONTROLPANEL_H
