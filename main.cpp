#include <QApplication>
#include <iostream>
#include <QSplashScreen>
#include <QCoreApplication>
#include <QMessageBox>
#include <QMainWindow>
#include <QDir>
#include "mainwindow.h"

using namespace std;
class CmdLineParser{int argc; char **argv; public:
                    CmdLineParser(int _argc,char **_argv):argc(_argc),argv(_argv){}
                                    bool operator[] ( string param ) {int idx=-1;  for ( int i=0; i<argc && idx==-1; i++ ) if ( string ( argv[i] ) ==param ) idx=i;    return ( idx!=-1 ) ;    }
                                                    string operator()(string param,string defvalue="-1"){int idx=-1;    for ( int i=0; i<argc && idx==-1; i++ ) if ( string ( argv[i] ) ==param ) idx=i; if ( idx==-1 ) return defvalue;   else  return ( argv[  idx+1] ); }
                   };


int main(int argc, char *argv[])
{



    std::setlocale(LC_ALL, "spanish");
    std::cout << std::locale().name() << std::endl;

       CmdLineParser cml(argc,argv);

    //check if the settings file is not created and create it
    QApplication QApp(argc, argv);
    QCoreApplication::setOrganizationName("UCOAVA");
    QCoreApplication::setApplicationName("QTPROJECT");


    QApp.processEvents();
    //go to application filepath to ensure the settings file can be properly readed

    QDir::setCurrent(QCoreApplication::applicationDirPath());

    MainWindow *mw=new MainWindow();

//    MainWindow *mw=new MainWindow("settings",cml["-iamtheteacher"]);
    QApp.processEvents();

    //now, go to user space
    QDir::setCurrent(QDir::homePath());
    mw->show();
    return QApp.exec();

}
