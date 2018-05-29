#ifndef ModuleArucoTest_MODULE_H
#define ModuleArucoTest_MODULE_H



#include <QToolBox>
#include <QWidget>
#include <QThread>
#include <QPlainTextEdit>
#include <memory>
#include "moduletools/appmodule.h"
#include "gparam/paramsetwdgt.h"
#include "videoplayer/videoplayer.h"
#include "aruco/aruco.h"
#include <QAction>
#include "modulearucotest_exports.h"

#include <iostream>
class APP_MODULE_ARUCOTEST_TOOLS_API  ModuleArucoTest: public AppModule {
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
      void on_clearDetections();
      void on_saveDetections();
      void redraw();
signals:
    void mesh_generated(std::string);
private:


    QPlainTextEdit *detectionsLabel;
    QAction *act_ShowThresImage;
    VideoPlayer *vplayer=0;
    aruco::MarkerDetector _arucoMDetector;
    cv::Mat currImage;



    void  getThresholdedImage(cv::Mat &TCopy);
    void printDetectionsText(const std::vector<aruco::Marker> &markers);

};

#endif // TSDFTOOLBOX_H
