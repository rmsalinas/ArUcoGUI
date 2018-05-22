#ifndef PARAMSETDLG_H
#define PARAMSETDLG_H

 
#include "gparam.h"
#include "paramsetwdgt.h"
#include "exports.h" 

namespace gparam
{
  
class GPARAM_API ParamSetDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ParamSetDialog ( QWidget *parent=0, Qt::WindowFlags f = 0 );
    explicit ParamSetDialog ( const gparam::ParamSet &paramSet, QWidget *parent=0, Qt::WindowFlags f = 0 );
    explicit ParamSetDialog ( gparam::ParamSet *paramSet, QWidget *parent=0, Qt::WindowFlags f = 0 );
    ~ParamSetDialog();
    void setParamSet ( const  gparam::ParamSet &paramSet );
    void setParamSet ( gparam::ParamSet *paramSet );
    gparam::ParamSet getParamSet() const;
    void setButtons(QDialogButtonBox::StandardButtons buttons) { _paramSetWgt->setButtons(buttons);   };
private:
    void createDialog();
    void applyChanges();
    ParamSetWdgt *_paramSetWgt;
    QVBoxLayout *_layout;
 signals:
    void paramChanged ( int idx );
    void okBtnPressed();
    void cancelBtnPressed();
    void applyBtnPressed();

private slots:
    void okPressed();
    void cancelPressed();
    void applyPressed(); 
    void paramSetModified(int);

};


}
#endif
