#ifndef ModuleCalibration_MODULE_H
#define ModuleCalibration_MODULE_H



#include <QToolBox>
#include <QWidget>
#include <QThread>
#include <memory>
#include "moduletools/appmodule.h"
#include "gparam/paramsetwdgt.h"
#include "videoplayer.h"
#include "aruco/aruco.h"

#include <iostream>
class Gauss_Thread;
class   ModuleCalibration: public AppModule {
    Q_OBJECT
public:
    ModuleCalibration();
    std::string getName() const {return "Calibration";}
    std::string getToolBoxTitle() const {return "Example";}


public slots:
    void on_activate (  );
    void on_deactivate (  );
    void on_global_action(const gparam::ParamSet &paramset);

private slots:
    void onParamsOkPressed();
    void on_reset_action();
    void onGauss();
    void on_gauss_thread_finished();
    void on_newVideoImage(cv::Mat &im);
signals:
    void mesh_generated(std::string);
private:
     QToolBox *_tbox=0;
    QToolBar *_tbar=0;
    gparam::ParamSet gauss_params;
    QAction *reset_action;
    std::shared_ptr<Gauss_Thread> _gauss_thread;

    void readParamSet();


    VideoPlayer *vplayer=0;
    aruco::MarkerDetector _arucoMDetector;

};

class Gauss_Thread : public QThread{
    Q_OBJECT
public:
    Gauss_Thread( const gparam::ParamSet &params){_params=params;}
    void run(){
        int ntimes=_params["nTimes"].asInt()+1;
        int progressStep= 100/ntimes;
        std::cout<<"NTIMES="<<ntimes<<" progressStep="<<progressStep<<std::endl;
        emit notify_action_progress("Gauss",0,"processing",-1,-1);
        for(int i=0;i<ntimes;i++){
            QThread::sleep(2);
            emit notify_action_progress("Gauss",(i+1)*progressStep,"processing",-1,-1);
        }
        emit notify_action_progress("Gauss",100,"Finished",-1,-1);
    }
signals:
    void notify_action_progress(std::string,int,std::string,int,int);

private:
    gparam::ParamSet _params;
};

#endif // TSDFTOOLBOX_H
