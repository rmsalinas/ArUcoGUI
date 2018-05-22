#include "paramsetdlg.h"
#include <iostream>
using namespace std;
namespace gparam{

ParamSetDialog::ParamSetDialog ( QWidget* parent, Qt::WindowFlags f ) : QDialog ( parent,f )
{
    createDialog();
}

ParamSetDialog::ParamSetDialog ( const gparam::ParamSet &paramSet, QWidget* parent, Qt::WindowFlags f ) : QDialog ( parent, f )
{
    createDialog();
    setParamSet ( paramSet );
}
ParamSetDialog::ParamSetDialog ( gparam::ParamSet *paramSet, QWidget* parent, Qt::WindowFlags f ) : QDialog ( parent, f )
{
    createDialog();
    setParamSet ( paramSet );
}

ParamSetDialog::~ParamSetDialog(){
  delete _paramSetWgt;
  delete _layout;
}


void ParamSetDialog::setParamSet ( const gparam::ParamSet &paramSet )
{
    setWindowTitle ( QString::fromStdString ( paramSet.getName() ) );
    _paramSetWgt->setParamSet ( paramSet );
}
void ParamSetDialog::setParamSet ( gparam::ParamSet *paramSet )
{
    setWindowTitle ( QString::fromStdString ( paramSet->getName() ) );
    _paramSetWgt->setParamSet ( paramSet );
}

gparam::ParamSet ParamSetDialog::getParamSet() const
{
    return _paramSetWgt->getParamSet();
}


void ParamSetDialog::createDialog()
{
  QDialogButtonBox::StandardButtons buttons= QDialogButtonBox::Apply | QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
     _paramSetWgt = new ParamSetWdgt(0,buttons);
    _layout = new QVBoxLayout();
    setLayout ( _layout );
    _layout->addWidget(_paramSetWgt);

    connect ( _paramSetWgt, SIGNAL ( okBtnPressed()), this, SLOT ( okPressed() ) ) ;
    connect ( _paramSetWgt, SIGNAL ( cancelBtnPressed (  ) ), this, SLOT ( cancelPressed() ) ) ;
    connect ( _paramSetWgt, SIGNAL ( applyBtnPressed (  ) ), this, SLOT ( applyPressed() ) ) ;
    connect ( _paramSetWgt, SIGNAL ( paramChanged ( int ) ), this, SLOT ( paramSetModified(int) ) ) ;

}
 

void ParamSetDialog::okPressed()
{
     emit okBtnPressed();
     done(1);
}

void ParamSetDialog::cancelPressed()
{
    emit cancelBtnPressed();
    done(0);
}

void ParamSetDialog::applyPressed()
{

  emit applyBtnPressed();
}

void ParamSetDialog::paramSetModified(int idx)
{

  emit paramChanged(idx);
}

}