#ifndef ModuleCalibration_MODULE_H
#define ModuleCalibration_MODULE_H



#include <QToolBox>
#include <QWidget>
#include <QThread>
#include <memory>
#include "moduletools/appmodule.h"
#include "gparam/paramsetwdgt.h"
#include "videoplayer/videoplayer.h"
#include "aruco/aruco.h"
#include "modulecalibration_exports.h"

#include "calibrationcontrolpanel.h"
#include <iostream>
class Gauss_Thread;
class APP_MODULE_CALIBRATION_TOOLS_API  ModuleCalibration: public AppModule {
    Q_OBJECT
public:
    ModuleCalibration();
    std::string getName() const {return "Calibration";}
    std::string getToolBoxTitle() const {return "Calibration Parameters";}


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
    void on_vplayer_opened();
    void on_addCurrent();
    void on_addAll();
 signals:
    void mesh_generated(std::string);
private:
    QToolBox *_tbox=0;
    QToolBar *_tbar=0;
    gparam::ParamSet gauss_params;
    QAction *reset_action;
    std::shared_ptr<Gauss_Thread> _gauss_thread;

    void readParamSet();

    QAbstractButton *Btn_addCurImage,*Btn_addAllImages,*Btn_gotoNextFrame;
    VideoPlayer *vplayer=0;
    calibrationControlPanel *CalibPanel;

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
