#ifndef ModuleArucoTest_MODULE_H
#define ModuleArucoTest_MODULE_H



#include <QToolBox>
#include <QWidget>
#include <QThread>
#include <memory>
#include "moduletools/appmodule.h"
#include "gparam/paramsetwdgt.h"
#include "videoplayer.h"
#include "aruco/aruco.h"
#include <QAction>

#include <iostream>
class   ModuleArucoTest: public AppModule {
    Q_OBJECT
public:
    ModuleArucoTest();
    std::string getName() const {return "ArucoTest";}
    std::string getToolBoxTitle() const {return "Detections";}


public slots:
    void on_global_action(const gparam::ParamSet &paramset);

private slots:
      void on_newVideoImage(cv::Mat &im);
      void on_act_ShowThresImage_triggered();
      void redraw();
signals:
    void mesh_generated(std::string);
private:


    QAction *act_ShowThresImage;
    modulearucotest::VideoPlayer *vplayer=0;
    aruco::MarkerDetector _arucoMDetector;
    cv::Mat currImage;



    void  getThresholdedImage(cv::Mat &TCopy);

};

#endif // TSDFTOOLBOX_H
