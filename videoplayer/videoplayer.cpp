#include "videoplayer.h"


#include <QtGui>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <QSlider>
#include <QBoxLayout>
#include <QString>
#include <QFileDialog>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cstring>

class VideoImagePlayerBase{
public:
     virtual bool isOpened()const=0;
    virtual bool grab()=0;
    virtual void retrieve(cv::Mat &im)=0;
    virtual double get(int prop)=0;
    virtual void set(int prop,double val)=0;
    virtual std::string getInfo()const=0;
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
    _ImagePlayer(const QStringList &strlist){
        files.clear();
        //divide by ;
        for(int i=0;i<strlist.size();i++){
            files.push_back( strlist.at(i).toLocal8Bit().constData());
        }
        curFile=0;
    }
    virtual bool isOpened()const{return files.size()!=0;}
    virtual bool grab(){
        if (curFile>=files.size())return false;
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
};

class _VideoPlayer:public VideoImagePlayerBase{
    cv::VideoCapture videoReader;
    QString fpath;
    std::string info;
public:

      _VideoPlayer(const QString & filePath){videoReader.open(filePath.toStdString());
                                                fpath=QFile(filePath).fileName();}
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
};

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
{


    //  QVideoWidget *videoWidget = new QVideoWidget;
    imgLabel=new QLabel();
    imgLabel->setScaledContents(true);
    imgLabel->setMinimumSize(QSize(640,480));
    imgLabel->setPixmap(QPixmap ( QString:: fromUtf8 ( ":/images/cityoflove.jpg" ) ));

    QAbstractButton *openVideoButton = new QPushButton(tr("Open Video..."));
    connect(openVideoButton, &QAbstractButton::clicked, this, &VideoPlayer::openVideoFile);
    openVideoButton->setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/movie.png" )));


    QAbstractButton *openImagesButton = new QPushButton(tr("Open Image(s)..."));
    connect(openImagesButton, &QAbstractButton::clicked, this, &VideoPlayer::openImages);
    openImagesButton->setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/open.png" )));

    m_playButton = new QPushButton;
    m_playButton->setEnabled(false);
    m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(m_playButton, &QAbstractButton::clicked,this, &VideoPlayer::playPauseButtonClicked);



    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setRange(0, 0);
    std::cerr<<"INF="<<m_positionSlider->singleStep()<<" "<<m_positionSlider->pageStep()<<std::endl;

  //  connect(m_positionSlider, &QAbstractSlider::sliderMoved,this, &VideoPlayer::setPosition);
    connect(m_positionSlider, &QAbstractSlider::sliderReleased,this, &VideoPlayer::sliderReleased);
    connect(m_positionSlider, &QAbstractSlider::valueChanged,this, &VideoPlayer::valueChanged);

    m_errorLabel = new QLabel;
    m_errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openVideoButton);
    controlLayout->addWidget(openImagesButton);
    controlLayout->addWidget(m_playButton);
    controlLayout->addWidget(m_positionSlider);
    m_playButton->hide();
    m_positionSlider->hide();

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(imgLabel);
    layout->addLayout(controlLayout);
    layout->addWidget(m_errorLabel);

    setLayout(layout);


}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::openImages()
{
    QSettings settings;
    QStringList files = QFileDialog::getOpenFileNames (
                this,
                tr ( "Select one or several images" ),
                settings.value ( "currDir" ).toString(),
                tr ( "Open Images (*.jpg *.jpeg *.png *.bmp *.ppm *.pgm)" ) );
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

    m_positionSlider->setPageStep(_reader->get(CV_CAP_PROP_FRAME_COUNT)>=40?10:1);
    m_positionSlider->setRange(0, _reader->get(CV_CAP_PROP_FRAME_COUNT)-1);
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

void VideoPlayer::playNextFrame(){
    bool res=grabAndShow();

    if(res && isPlaying)
        QTimer::singleShot((1000./_reader->get(CV_CAP_PROP_FPS))-2,this,SLOT(playNextFrame()));
    else{
        isPlaying=0;
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

bool VideoPlayer::grabAndShow(){
    m_positionSlider->blockSignals(true);
    m_positionSlider->setValue(_reader->get(CV_CAP_PROP_POS_FRAMES));
    m_positionSlider->blockSignals(false);
    if (!_reader->grab()) return false;
    _reader->retrieve(imIn);
   // m_errorLabel->setText(_reader->getInfo().c_str());
    emit newImage(imIn);
    setImage(imIn);
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

void VideoPlayer::addCurrentImage(){
    imIn.copyTo(selectedImage);
    emit imageSelected();
}
void VideoPlayer::setImage(  cv::Mat &img2Show){


    QImage _qimgR ( ( const uchar * ) ( img2Show.ptr<uchar> ( 0 ) ),
                    img2Show.cols,img2Show.rows, QImage::Format_RGB888 ) ;

    imgLabel-> setPixmap ( QPixmap::fromImage ( _qimgR.rgbSwapped() ) );
}

