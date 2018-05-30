#include "paramsetwdgt.h"
#include <QDialogButtonBox>
#include <iostream>
using namespace std;
namespace gparam
{
void ParamSetWdgt::createButtonBox ( QDialogButtonBox::StandardButtons buttons )
{

//     _layout->addWidget ( _paramSetWgt );
    _layout->removeWidget(_buttonBox);
    if(_buttonBox!=NULL) delete _buttonBox;

    if ( !buttons.testFlag ( QDialogButtonBox::NoButton ) )
    {

        _buttonBox = new QDialogButtonBox ( buttons );
        if ( buttons.testFlag ( QDialogButtonBox::Ok ) )
        {

            connect ( _buttonBox->button ( QDialogButtonBox::Ok ), SIGNAL ( clicked ( bool ) ), this, SLOT ( okPressed() ) ) ;
        }
        if ( buttons.testFlag ( QDialogButtonBox::Cancel ) )
        {

            connect ( _buttonBox->button ( QDialogButtonBox::Cancel ), SIGNAL ( clicked ( bool ) ), this, SLOT ( cancelPressed() ) ) ;
        }

        if ( buttons.testFlag ( QDialogButtonBox::Apply ) )
        {
            _buttonBox->button ( QDialogButtonBox::Apply )->setEnabled ( false );

            connect ( _buttonBox->button ( QDialogButtonBox::Apply ), SIGNAL ( clicked ( bool ) ), this, SLOT ( applyPressed() ) ) ;
        }
        connect ( this, SIGNAL ( paramChanged ( int ) ), this, SLOT ( paramSetModified() ) ) ;

        _layout->addWidget ( _buttonBox );
    }
}
ParamSetWdgt::ParamSetWdgt ( QWidget *parent, QDialogButtonBox::StandardButtons buttons ) :
    QWidget ( parent )
{
    _buttonBox=0;

    _buttons=buttons;
    _layout = new QFormLayout();
    setLayout ( _layout );
}

ParamSetWdgt::ParamSetWdgt ( const gparam::ParamSet &paramSet,QWidget *parent, QDialogButtonBox::StandardButtons buttons ) :
    QWidget ( parent )
{
    _buttonBox=0;

    _buttons=buttons;
    setParamSet ( paramSet );
}

ParamSetWdgt::ParamSetWdgt ( gparam::ParamSet *paramSet,QWidget *parent , QDialogButtonBox::StandardButtons buttons ) : QWidget ( parent )
{
    _buttonBox=0;
    _buttons=buttons;
    setParamSet ( paramSet );
}
ParamSetWdgt::~ParamSetWdgt()
{
if (_buttonBox)    delete _buttonBox;
if(_layout)    delete _layout;
}

void ParamSetWdgt::clear(){
    for(auto &elm:elements) delete elm;
    elements.clear();
    if(_layout) delete _layout;
    _layout=nullptr;
}

void ParamSetWdgt::setParamSet ( gparam::ParamSet *paramSet )
{

    clear();
    _layout = new QFormLayout();
    setLayout ( _layout );
 // cout<<"Paramset ="<<paramSet->getPrintString()<<endl;
    _paramSet =  paramSet;
    setWindowTitle ( paramSet->getName().c_str() );
    for ( size_t i=0; i<_paramSet->size(); i++ )
    {
        switch ( _paramSet->at ( i ).getType() )
        {

        case gparam::Param::STRING:
            addString ( _paramSet->at ( i ) );
            break;

        case gparam::Param::STRINGLIST:
            addStringList ( _paramSet->at ( i ) );
            break;

        case gparam::Param::INTEGER:
            addInteger ( _paramSet->at ( i ) );
            break;

        case gparam::Param::REAL:
            addReal ( _paramSet->at ( i ) );
            break;

        case gparam::Param::BOOLEAN:
            addBoolean ( _paramSet->at ( i ) );
            break;

        case gparam::Param::FILEPATH:
        case gparam::Param::EXISTING_FILEPATH:
        case gparam::Param::DIRPATH:
            addFilepath ( _paramSet->at ( i ) );
            break;

        case gparam::Param::UNKNOWN:
        default:
            ;

        }
    }

    //finally, add button box
    createButtonBox ( _buttons );
}


void ParamSetWdgt::setParamSet ( const gparam::ParamSet & paramSet )
{

    internal_paramSet=paramSet;
    setParamSet ( &internal_paramSet );
}

void ParamSetWdgt::findSignalSender()
{
    QObject* senderObj = QObject::sender();
    for ( int i=0; i<children().size(); i++ )
    {
        if ( ( children() [i] )->objectName() == senderObj->objectName() )
        {
            int row;
            QFormLayout::ItemRole role;
            _layout->getWidgetPosition ( ( QWidget* ) ( children() [i] ), &row, &role );
            storeChange ( row, children() [i] );
//             std::cout << "Changed: " << row << std::endl;
            emit paramChanged ( row );
            break;
        }
    }
}


void ParamSetWdgt::storeChange ( int idx, QObject* obj )
{
    switch ( _paramSet->at ( idx ).getType() )
    {
    case gparam::Param::INTEGER :
    {
        QSpinBox* spinBox = static_cast<QSpinBox*> ( obj );
        _paramSet->at ( idx ) = int ( spinBox->value() );
        break;
    }

    case gparam::Param::REAL :
    {
        QDoubleSpinBox* doublespinBox = static_cast<QDoubleSpinBox*> ( obj );
        _paramSet->at ( idx ) = double ( doublespinBox->value() );
        break;
    }

    case gparam::Param::STRING :
    {
        QLineEdit* lineEdit = static_cast<QLineEdit*> ( obj );
        _paramSet->at ( idx ) = lineEdit->text().toStdString();
        break;
    }

    case gparam::Param::STRINGLIST :
    {
        QComboBox* comboBox = static_cast<QComboBox*> ( obj );
        _paramSet->at ( idx ) = comboBox->currentText().toStdString();
        break;
    }

    case gparam::Param::BOOLEAN :
    {
        QCheckBox* checkBox = static_cast<QCheckBox*> ( obj );
        _paramSet->at ( idx ) = checkBox->isChecked();
        break;
    }

    case gparam::Param::FILEPATH:
    case gparam::Param::EXISTING_FILEPATH:
    case gparam::Param::DIRPATH:
    {
        QFilePathLineEdit* filepathLineEdit = static_cast<QFilePathLineEdit*> ( obj );
        _paramSet->at ( idx ) = filepathLineEdit->text().toStdString();
        break;
    }

    case gparam::Param::UNKNOWN:
    default:
        ;

    }
}



void ParamSetWdgt::addInteger ( gparam::Param intParam )
{
    QSpinBox* spinBox = new QSpinBox();
    spinBox->setToolTip ( QString::fromStdString ( intParam.getDescription() ) );
    spinBox->setWhatsThis ( QString::fromStdString ( intParam.getDescription() ) );
    if ( intParam.hasLimits() )
    {
        spinBox->setMinimum ( intParam.getLowerLimit<int>() );
        spinBox->setMaximum ( intParam.getUpperLimit<int>() );
    }
    else
    {
        spinBox->setMinimum ( std::numeric_limits<int>::min() );
        spinBox->setMaximum ( std::numeric_limits<int>::max() );
    }
    if ( intParam.getStepIncrement() !=0 )
        spinBox->setSingleStep ( intParam.getStepIncrement() );
    //set valuue must be after setting limits
    spinBox->setValue ( intParam.get<int>() );
    _layout->addRow ( QString::fromStdString ( intParam.getName() ), spinBox );
    spinBox->setObjectName ( QString::fromStdString ( intParam.getName() ) );
    connect ( spinBox, SIGNAL ( valueChanged ( int ) ), this, SLOT ( findSignalSender() ) );
    elements.push_back(spinBox);
}

void ParamSetWdgt::addString ( gparam::Param strParam )
{
    QLineEdit *lineEdit = new QLineEdit();
    lineEdit->setText ( QString::fromStdString ( strParam.asString() ) );
    lineEdit->setToolTip ( QString::fromStdString ( strParam.getDescription() ) );
    lineEdit->setWhatsThis ( QString::fromStdString ( strParam.getDescription() ) );
    _layout->addRow ( QString::fromStdString ( strParam.getName() ), lineEdit );
    lineEdit->setObjectName ( QString::fromStdString ( strParam.getName() ) );
    connect ( lineEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( findSignalSender() ) );
    elements.push_back(lineEdit);
}

void ParamSetWdgt::addStringList ( gparam::Param strlistParam )
{
  //cout<<"ADD STRLIST="<<strlistParam<<endl;
    QComboBox* comboBox = new QComboBox();
    for ( size_t i=0; i<strlistParam.getStringList().size(); i++ )
        comboBox->addItem ( QString::fromStdString ( strlistParam.getStringList() [i] ) );
    
    comboBox->setCurrentIndex ( comboBox->findText ( QString::fromStdString ( strlistParam.asString() ) ) );
    comboBox->setToolTip ( QString::fromStdString ( strlistParam.getDescription() ) );
    comboBox->setWhatsThis ( QString::fromStdString ( strlistParam.getDescription() ) );
    _layout->addRow ( QString::fromStdString ( strlistParam.getName() ), comboBox );
    comboBox->setObjectName ( QString::fromStdString ( strlistParam.getName() ) );
    connect ( comboBox, SIGNAL ( currentIndexChanged ( int ) ), this, SLOT ( findSignalSender() ) );
    elements.push_back(comboBox);
}

void ParamSetWdgt::addReal ( gparam::Param realParam )
{
    QDoubleSpinBox* doublespinBox = new QDoubleSpinBox();
    doublespinBox->setDecimals ( 4 );
    doublespinBox->setToolTip ( QString::fromStdString ( realParam.getDescription() ) );
    doublespinBox->setWhatsThis ( QString::fromStdString ( realParam.getDescription() ) );
    if ( realParam.hasLimits() )
    {
        doublespinBox->setMinimum ( realParam.getLowerLimit<double>() );
        doublespinBox->setMaximum ( realParam.getUpperLimit<double>() );
    }
    else
    {
        doublespinBox->setMinimum ( std::numeric_limits<int>::min() );
        doublespinBox->setMaximum ( std::numeric_limits<int>::max() );
    }
    //set value must be after setting limits
    doublespinBox->setValue ( realParam.get<double>() );
    if ( realParam.getStepIncrement() !=0 )
        doublespinBox->setSingleStep ( realParam.getStepIncrement() );

    _layout->addRow ( QString::fromStdString ( realParam.getName() ), doublespinBox );
    doublespinBox->setObjectName ( QString::fromStdString ( realParam.getName() ) );
    connect ( doublespinBox, SIGNAL ( valueChanged ( double ) ), this, SLOT ( findSignalSender() ) );
    elements.push_back(doublespinBox);
}

void ParamSetWdgt::addBoolean ( gparam::Param boolParam )
{
    QCheckBox* checkBox = new QCheckBox();
    checkBox->setChecked ( boolParam.get<bool>() );
    checkBox->setToolTip ( QString::fromStdString ( boolParam.getDescription() ) );
    checkBox->setWhatsThis ( QString::fromStdString ( boolParam.getDescription() ) );
    _layout->addRow ( QString::fromStdString ( boolParam.getName() ), checkBox );
    checkBox->setObjectName ( QString::fromStdString ( boolParam.getName() ) );
    connect ( checkBox, SIGNAL ( toggled ( bool ) ), this, SLOT ( findSignalSender() ) );
    elements.push_back(checkBox);
}

void ParamSetWdgt::addFilepath ( gparam::Param filepathParam )
{
    QFilePathLineEdit *filepathLineEdit = new QFilePathLineEdit();
    filepathLineEdit->setText ( QString::fromStdString ( filepathParam.asString() ) );
    if ( filepathParam.getType() ==gparam::Param::FILEPATH ) filepathLineEdit->getFileDialog()->setFileMode ( QFileDialog::AnyFile );
    else if ( filepathParam.getType() ==gparam::Param::EXISTING_FILEPATH ) filepathLineEdit->getFileDialog()->setFileMode ( QFileDialog::ExistingFile );
    else if ( filepathParam.getType() ==gparam::Param::DIRPATH ) filepathLineEdit->getFileDialog()->setFileMode ( QFileDialog::Directory );
    filepathLineEdit->setToolTip ( QString::fromStdString ( filepathParam.getDescription() ) );
    filepathLineEdit->setWhatsThis ( QString::fromStdString ( filepathParam.getDescription() ) );
    _layout->addRow ( QString::fromStdString ( filepathParam.getName() ), filepathLineEdit );
    filepathLineEdit->setObjectName ( QString::fromStdString ( filepathParam.getName() ) );
    connect ( filepathLineEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( findSignalSender() ) );
    elements.push_back(filepathLineEdit);
}





void ParamSetWdgt::okPressed()
{
    emit okBtnPressed();
}
void ParamSetWdgt::cancelPressed()
{
    emit cancelBtnPressed();;
}
void ParamSetWdgt::applyPressed()
{
    emit applyBtnPressed();
    _buttonBox->button ( QDialogButtonBox::Apply )->setEnabled ( false );

}
void ParamSetWdgt::paramSetModified()
{
    if ( _buttonBox!=NULL )
    {
        QPushButton *applyButton=_buttonBox->button ( QDialogButtonBox::Apply );
        if(applyButton!=NULL) applyButton->setEnabled ( true );
    }
}


}














