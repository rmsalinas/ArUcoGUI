#ifndef PARAMSETWDGT_H
#define PARAMSETWDGT_H

#include <QWidget>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include "gparam.h"
#include "exports.h" 
namespace gparam
{
class GPARAM_API ParamSetWdgt : public QWidget
{
    Q_OBJECT
public:
    explicit ParamSetWdgt ( QWidget *parent = 0, QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::NoButton );
    explicit ParamSetWdgt ( const gparam::ParamSet &paramSet,QWidget *parent = 0, QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::NoButton );
    explicit ParamSetWdgt ( gparam::ParamSet *paramSet,QWidget *parent = 0, QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::NoButton );
    ~ParamSetWdgt();
    /**Works on a internal copy of the data
     */
    void setParamSet ( const gparam::ParamSet &paramSet );
    /**Works on a the paramset passed
     */
    void setParamSet ( gparam::ParamSet *paramSet );
    gparam::ParamSet getParamSet() const
    {
        return *_paramSet;
    };
    
    void setButtons(QDialogButtonBox::StandardButtons buttons) {
      _buttons = buttons;
      createButtonBox(buttons);
    };

private:
    void createButtonBox ( QDialogButtonBox::StandardButtons buttons );


    void addInteger ( gparam::Param intParam );
    void addString ( gparam::Param strParam );
    void addStringList ( gparam::Param strlistParam );
    void addReal ( gparam::Param realParam );
    void addBoolean ( gparam::Param boolParam );
    void addFilepath ( gparam::Param filepathParam );

    void storeChange ( int idx, QObject* obj );


    QFormLayout *_layout;
    gparam::ParamSet *_paramSet;
    gparam::ParamSet internal_paramSet;
    QDialogButtonBox* _buttonBox;
    QDialogButtonBox::StandardButtons _buttons;
 signals:
    void paramChanged ( int idx );
    void okBtnPressed();
    void cancelBtnPressed();
    void applyBtnPressed();

private slots:
    void okPressed();
    void cancelPressed();
    void applyPressed(); 
    void findSignalSender();
    void paramSetModified();
};



class QFilePathLineEdit : public QLineEdit
{
public:
    QFilePathLineEdit ( QWidget *parent = 0 )
    {
        _button = new QPushButton ( "..." );
        _button->setParent ( this );
        _fileDialog = new QFileDialog();
	_fileDialog->setModal(true);
        this->setTextMargins ( 0,0,30,0 ); // not write behind button
        connect ( _button, SIGNAL ( clicked() ), _fileDialog, SLOT ( show() ) );
        connect ( _fileDialog, SIGNAL ( fileSelected ( QString ) ), this, SLOT ( setText ( QString ) ) );
    }
    QFileDialog* getFileDialog()
    {
        return _fileDialog;
    }

protected:
    void resizeEvent ( QResizeEvent *event )
    {
        QLineEdit::resizeEvent ( event );
        updateButton();
    }

private:
    QPushButton *_button;
    QFileDialog *_fileDialog;
    void updateButton()
    {
        QRect buttonRect ( this->width()-31, 1, 30, this->height()-2 );
        _button->setGeometry ( buttonRect );
    }

};

}
#endif // PARAMSETWDGT_H
