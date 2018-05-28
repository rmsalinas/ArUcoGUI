#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "moduletools/modulesetmainwindow.h"


class MainWindow : public ModuleSetMainWindow {
    Q_OBJECT

public:
    explicit MainWindow ( QWidget *parent = 0  );

public slots:
    void on_global_action(const gparam::ParamSet &paramset);
    void on_ArucoParamsChanged();
    void on_arucoParamsShowAction_triggered();
private :
    QAction *arucoParamsShowAction;
    QToolBar *_tbar;
    QDockWidget *_arucoDock;

};

#endif // MAINWINDOW_H
