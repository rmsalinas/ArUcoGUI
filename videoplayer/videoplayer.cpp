#include "videoplayer.h"


#include <QtGui>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <QSlider>
#include <QString>
#include <QAction>
#include <QFileDialog>
#include <QScrollBar>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cstring>
#include "arucogparam.h"

class VideoImagePlayerBase{
public:
     virtual bool isOpened()const=0;
    virtual bool grab()=0;
    virtual void retrieve(cv::Mat &im)=0;
    virtual double get(int prop)=0;
    virtual void set(int prop,double val)=0;
    virtual std::string getInfo()const=0;
    virtual QStringList getSource()=0;
};

class _ImagePlayer:public VideoImagePlayerBase{
    std::vector<std::string> tokenize(const std::string& in, const std::string& delims )
    {
        std::vector<std::string> tokens;
        std::string::size_type pos_begin  , pos_end  = 0;
        std::string input = in;

        input.erase(std::remove_if(input.begin(),
                                  input.end(),
                                  [](char x){return std::isspace(x);}),input.end());

        while ((pos_begin = input.find_first_not_of(delims,pos_end)) != std::string::npos)
        {
            pos_end = input.find_first_of(delims,pos_begin);
            if (pos_end == std::string::npos) pos_end = input.length();

            tokens.push_back( input.substr(pos_begin,pos_end-pos_begin) );
        }
        return tokens;
    }
public:
    std::vector<std::string> files;
    int curFile=-1;
    std::string info;
    cv::Mat image;
    QStringList sources;
    _ImagePlayer(const QStringList &strlist){
        files.clear();
        sources=strlist;
        //divide by ;
        for(int i=0;i<strlist.size();i++){
            files.push_back( strlist.at(i).toLocal8Bit().constData());
        }
        curFile=0;
    }
    virtual bool isOpened()const{return files.size()!=0;}
    virtual bool grab(){
        if (size_t(curFile)>=files.size())return false;
        image=cv::imread(files[curFile]);
        info=files[curFile];
        curFile++;
        return true;
    }
    virtual void retrieve(cv::Mat &im){
        image.copyTo(im);
    }
    virtual double get(int prop){
        switch (prop){
        case CV_CAP_PROP_FRAME_COUNT:return files.size();break;
        case CV_CAP_PROP_FPS:return 1;break;
        case CV_CAP_PROP_POS_FRAMES:return curFile;break;
        };
        return 0;
    }
    virtual void set(int prop,double val){
        switch (prop){
        case CV_CAP_PROP_POS_FRAMES:
            if(val>=files.size())std::cerr<<"_ImagePlayer can not set:CV_CAP_PROP_POS_FRAMES"<<std::endl;
            else curFile=val;
            break;
        };

    }
    virtual std::string getInfo()const{
            return info;
    }
    virtual QStringList getSource(){
        return sources;
    }

};

class _VideoPlayer:public VideoImagePlayerBase{
    cv::VideoCapture videoReader;
    QString fpath;
    std::string info;
    QStringList source;
public:

      _VideoPlayer(const QString & filePath){videoReader.open(filePath.toStdString());
                                                fpath=QFile(filePath).fileName();
                                            source<<filePath;
                                            }
    virtual bool isOpened()const{return videoReader.isOpened();}
    virtual bool grab(){
          info=QString::number(videoReader.get(CV_CAP_PROP_POS_FRAMES)).toStdString();
          return videoReader.grab();
      }
    virtual void retrieve(cv::Mat &im){videoReader.retrieve(im);}
    virtual double get(int prop){return videoReader.get(prop);}
    virtual void set(int prop,double val){videoReader.set(prop,val);}
    virtual std::string getInfo()const{
        return info;
    }
      virtual QStringList getSource(){
          return  source;
      }

};

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
{


    imgLabel=new QLabel();
    imgLabel->setBackgroundRole(QPalette::Base);
    imgLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imgLabel->setScaledContents(true);
    imgLabel->setPixmap(QPixmap ( QString:: fromUtf8 ( ":/images/cityoflove.jpg" ) ));

    imgLabel->adjustSize();

    scrollArea= new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imgLabel);
    scrollArea->setWidgetResizable(true);

    _actions.push_back( new QAction ( QIcon ( ":/images/movie.png" ), tr ( "&Open Video..." ), this ));
    connect(_actions.back(),&QAction::triggered,this,&VideoPlayer::openVideoFile);




    _actions.push_back( new QAction ( QIcon ( ":/images/open.png" ), tr ( "&Open Image(s)..." ), this ));
    connect(_actions.back(),&QAction::triggered,this,&VideoPlayer::openImages);

    _actions.push_back( new QAction ( QIcon ( ":/images/fitToWindow.png" ), tr ( "&Fit..." ), this ));
    connect(_actions.back(),&QAction::triggered,this,&VideoPlayer::normalSize);



    m_playButton = new QPushButton;
    m_playButton->setEnabled(false);
    m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(m_playButton, &QAbstractButton::clicked,this, &VideoPlayer::playPauseButtonClicked);



    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setRange(0, 0);
    std::cerr<<"INF="<<m_positionSlider->singleStep()<<" "<<m_positionSlider->pageStep()<<std::endl;

    connect(m_positionSlider, &QAbstractSlider::sliderReleased,this, &VideoPlayer::sliderReleased);
    connect(m_positionSlider, &QAbstractSlider::valueChanged,this, &VideoPlayer::valueChanged);

    m_errorLabel = new QLabel;
    m_errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);

    controlLayout->addWidget(m_playButton);
    controlLayout->addWidget(m_positionSlider);

    m_playButton->hide();
    m_positionSlider->hide();

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(scrollArea);
    layout->addLayout(controlLayout);
    layout->addWidget(m_errorLabel);

    setLayout(layout);


}

VideoPlayer::~VideoPlayer()
{
}
QStringList VideoPlayer::getSource()const{
    if ( !_reader)return{};
    else return _reader->getSource();
}

void VideoPlayer::addButton(QAbstractButton* btn){
    controlLayout->addWidget(btn);
    addedButtons.push_back(btn);


}

bool VideoPlayer::isVideo()const{
    if(!_reader)return false;
    if ( dynamic_cast<_VideoPlayer*>( _reader.get())!=0)return true;
    return false;
}

void VideoPlayer::openImages()
{
    scaleFactor=1;
    QSettings settings;
    QStringList files = QFileDialog::getOpenFileNames (
                this,
                tr ( "Select one or several images" ),
                settings.value ( "currDir" ).toString(),
                tr ( "Open Images (*.*)" ) );
    if ( files.size()==0) return;

    settings.setValue ( "currDir",QFileInfo ( files.at(0) ).absolutePath() );

    m_errorLabel->clear();
    _reader=std::make_shared<_ImagePlayer>(files);
    if (!_reader->isOpened()){
        m_errorLabel->setText(tr("Could not open image files"));
        return;
    }
    prepareForOpenedReader();
}

void VideoPlayer::openVideoFile(){
    scaleFactor=1;
    QSettings settings;
    QString file = QFileDialog::getOpenFileName (
                this,
                tr ( "Select an input file" ),
                settings.value ( "currDir" ).toString(),
                tr ( "Open Movie (*.*)" ) );
    if ( file==QString() ) return;

    settings.setValue ( "currDir",QFileInfo ( file ).absolutePath() );

    _reader=std::make_shared<_VideoPlayer>(file);
    if (!_reader->isOpened()){
        m_errorLabel->setText(tr("Could not open file ")+file);
        return;
    }
    prepareForOpenedReader();
}


void VideoPlayer::prepareForOpenedReader(){
    emit     openedImageOrVideo();

    m_positionSlider->setPageStep(_reader->get(CV_CAP_PROP_FRAME_COUNT)>=40?10:1);
    m_positionSlider->setRange(0, _reader->get(CV_CAP_PROP_FRAME_COUNT)-1);
    m_positionSlider->setTickInterval(_reader->get(CV_CAP_PROP_FRAME_COUNT)>=40?_reader->get(CV_CAP_PROP_FRAME_COUNT)/10:1);
     m_positionSlider->setTickPosition(QSlider::TicksBelow);
    if (_reader->get(CV_CAP_PROP_FRAME_COUNT)>1){
        m_playButton->show();
        m_positionSlider->show();
    }
    else{
        m_playButton->hide();
        m_positionSlider->hide();
    }
    m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    m_playButton->setEnabled(true);
    isPlaying=0;
    m_positionSlider->setValue(0);
    grabAndShow();

}


std::string VideoPlayer::getCurrentImageInfo(){
    if (!_reader) return "";
    return _reader->getInfo();
}


void VideoPlayer::playPauseButtonClicked()
{
    if (!isPlaying){
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        isPlaying=1;
        playNextFrame();
    }
    else{
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        isPlaying=0;
    }
}

bool VideoPlayer::playNextFrame(){
    bool res=grabAndShow();

    if(res && isPlaying)
        QTimer::singleShot((1000./_reader->get(CV_CAP_PROP_FPS))-2,this,SLOT(playNextFrame()));
    else{
        isPlaying=0;
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
    return res;
}

void VideoPlayer::updateImage(){
    if (imIn.empty()) return ;
    imIn.copyTo(imshown);
    if (_processImageWithArucoDetector){
        detectedMarkers=ArucoMarkerDetector::get().detect(imshown);
        for(auto m:detectedMarkers) m.draw(imshown);
    }
    setImage(imshown);

}


bool VideoPlayer::grabAndShow(){
    m_positionSlider->blockSignals(true);
    m_positionSlider->setValue(_reader->get(CV_CAP_PROP_POS_FRAMES));
    m_positionSlider->blockSignals(false);
    if (!_reader->grab()) return false;
    _reader->retrieve(imIn);

    emit newImage(imIn);


    if (imIn.empty()) return false;
    imIn.copyTo(imshown);

    if (_processImageWithArucoDetector){
        detectedMarkers=ArucoMarkerDetector::get().detect(imshown);
        for(auto m:detectedMarkers) m.draw(imshown);
    }
    setImage(imshown);
    return true;
}

void VideoPlayer::valueChanged(int value){
    std::cerr<<"value changed:"<<value<<"\n";
    _reader->set(CV_CAP_PROP_POS_FRAMES,m_positionSlider->value());
    grabAndShow();
}
void VideoPlayer::sliderReleased(){
    std::cerr<<"sliderReleased\n";
}
void VideoPlayer::setPosition(int position)
{
    std::cerr<<"setPosition\n";
    m_positionSlider->setValue(position);
   // _reader->set(CV_CAP_PROP_POS_FRAMES,position);
   // grabAndShow();
}


void VideoPlayer::setImage(  cv::Mat &img2Show){


    QImage _qimgR ( ( const uchar * ) ( img2Show.ptr<uchar> ( 0 ) ),
                    img2Show.cols,img2Show.rows, QImage::Format_RGB888 ) ;

    imgLabel-> setPixmap ( QPixmap::fromImage ( _qimgR.rgbSwapped() ) );
}

void VideoPlayer::scaleImage(double factor)
{
    scrollArea->setWidgetResizable(false);

    scaleFactor *= factor;
    imgLabel->resize(scaleFactor * imgLabel->pixmap()->size());
    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    //zoomInAct->setEnabled(scaleFactor < 3.0);
    //zoomOutAct->setEnabled(scaleFactor > 0.333);
}
void VideoPlayer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
void VideoPlayer::normalSize(){
    scrollArea->setWidgetResizable(true);
    scaleFactor=1;
    imgLabel->resize(scaleFactor * imgLabel->pixmap()->size());
}
void	VideoPlayer::wheelEvent(QWheelEvent *event){
    if (event->modifiers()== Qt::CTRL){
    QPoint numDegrees = event->angleDelta() / 8;
    scaleImage(1+numDegrees .ry()*0.005);
    }

}
