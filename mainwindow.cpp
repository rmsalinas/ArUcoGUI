#include "mainwindow.h"
#include <iostream>
#include "modulecalibration/modulecalibration.h"
using namespace std;





MainWindow::MainWindow ( QWidget *parent  ) :
ModuleSetMainWindow ( parent )  {
    try {
        setWindowTitle ( "Qt Project" );
        resize ( 640,480 );
        addModule ( "Calibration", std::make_shared< ModuleCalibration> () );
        activateModule("Calibration");
    } catch ( std::exception &ex ) {
        cerr<<ex.what() <<endl;
    }



}

void MainWindow::on_global_action(const gparam::ParamSet &paramset){
cerr<<paramset<<endl;
}
