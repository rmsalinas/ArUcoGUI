#ifndef MODULEEXAMPLE_MODULE_H
#define MODULEEXAMPLE_MODULE_H



#include <QToolBox>
#include <QWidget>
#include <QThread>
#include <memory>
#include "moduletools/appmodule.h"
#include "gparam/paramsetwdgt.h"
#include <iostream>
#include "moduleexample_exports.h"
class Gauss_Thread;
class APP_MODULEEXAMPLE_TOOLS_API  ModuleExample: public AppModule {
    Q_OBJECT
public:
    ModuleExample();
    std::string getName() const {return "ModuleExample";}
    std::string getToolBoxTitle() const {return "Example";}


public slots:
    void on_activate (  );
    void on_deactivate (  );
    void on_globalaction(const gparam::ParamSet &paramset);

private slots:
    void onParamsOkPressed();
    void on_reset_action();
    void onGauss();
    void on_gauss_thread_finished();
signals:
    void mesh_generated(std::string);
private:
    QLabel *_labelImage=0;
    QToolBox *_tbox=0;
    QToolBar *_tbar=0;
    gparam::ParamSet gauss_params;
    QAction *reset_action;
    std::shared_ptr<Gauss_Thread> _gauss_thread;

    void readParamSet();


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
