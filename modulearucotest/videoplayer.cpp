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
namespace modulearucotest {

VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
{


    //  QVideoWidget *videoWidget = new QVideoWidget;
    imgLabel=new QLabel();
    imgLabel->setScaledContents(true);
    imgLabel->setMinimumSize(QSize(640,480));
    imgLabel->setPixmap(QPixmap ( QString:: fromUtf8 ( ":/images/cityoflove.jpg" ) ));

    QAbstractButton *openButton = new QPushButton(tr("Open..."));
    connect(openButton, &QAbstractButton::clicked, this, &VideoPlayer::openFile);

    openButton->setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/open.png" )));

    m_playButton = new QPushButton;
    m_playButton->setEnabled(false);
    m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(m_playButton, &QAbstractButton::clicked,this, &VideoPlayer::playPauseButtonClicked);



    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setRange(0, 0);

    connect(m_positionSlider, &QAbstractSlider::sliderMoved,this, &VideoPlayer::setPosition);

    m_errorLabel = new QLabel;
    m_errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
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

void VideoPlayer::openFile()
{
      QSettings settings;
      QString file = QFileDialog::getOpenFileName (
                            this,
                            tr ( "Select an input file" ),
                            settings.value ( "currDir" ).toString(),
                            tr ( "Open Movie or Image (*.*)" ) );
      if ( file!=QString() ) {
          settings.setValue ( "currDir",QFileInfo ( file ).absolutePath() );

        videoReader.open(file.toStdString());
        if (!videoReader.isOpened()){
            m_errorLabel->setText(tr("Could not open file ")+file);
            return;
        }

        m_positionSlider->setRange(0, videoReader.get(CV_CAP_PROP_FRAME_COUNT));
        if (videoReader.get(CV_CAP_PROP_FRAME_COUNT)>1){
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
        setPosition(0);
     }
}



int VideoPlayer::getFramePos(){
    if (!videoReader.isOpened() )return -1;
    else return videoReader.get(CV_CAP_PROP_POS_FRAMES);
}


void VideoPlayer::playPauseButtonClicked()
{
    if (!isPlaying){
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        isPlaying=1;
        nextFrame();
    }
    else{
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        isPlaying=0;
    }
}

void VideoPlayer::nextFrame(){
    if (videoReader.grab()){
        videoReader.retrieve(imIn);
        m_positionSlider->setValue(videoReader.get(CV_CAP_PROP_POS_FRAMES));
        emit newImage(imIn);
        setImage(imIn);
        if (isPlaying) {
            double fps=videoReader.get(CV_CAP_PROP_FPS);
            QTimer::singleShot((1000./fps)-2,this,SLOT(nextFrame()));
        }
    }
    else{
        isPlaying=0;
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

void VideoPlayer::addCurrentImage(){
    imIn.copyTo(selectedImage);
    emit imageSelected();
}

void VideoPlayer::setPosition(int position)
{
    m_positionSlider->setValue(position);
    videoReader.set(CV_CAP_PROP_POS_FRAMES,position);
    videoReader.grab();
    videoReader.retrieve(imIn);
    emit newImage(imIn);
    setImage(imIn);
}

void VideoPlayer::setImage(  cv::Mat &img2Show){


    QImage _qimgR ( ( const uchar * ) ( img2Show.ptr<uchar> ( 0 ) ),
                    img2Show.cols,img2Show.rows, QImage::Format_RGB888 ) ;

    imgLabel-> setPixmap ( QPixmap::fromImage ( _qimgR.rgbSwapped() ) );
}

}
