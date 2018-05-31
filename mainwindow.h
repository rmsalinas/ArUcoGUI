#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "moduletools/modulesetmainwindow.h"
#include "gparam/paramsetwdgt.h"

class MainWindow : public ModuleSetMainWindow {
    Q_OBJECT

public:
    explicit MainWindow ( QWidget *parent = 0  );

public slots:
    void on_global_action(const gparam::ParamSet &paramset);
    void on_ArucoParamsChanged();
 public slots:
    void on_module_activated(std::string moduleName,ModuleInfo minfo);

private :
    QAction *arucoParamsShowAction;
    QToolBar *_tbar;
    QDockWidget *_arucoDock;
    gparam::ParamSetWdgt* _arucoWdgt;

};

#endif // MAINWINDOW_H
