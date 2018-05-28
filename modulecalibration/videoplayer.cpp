#include "videoplayer.h"

#include <QtWidgets>
#include <QVideoWidget>
#include <QtGlobal>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cstring>

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

    m_plusButton= new QPushButton;
    m_plusButton->setEnabled(false);
    m_plusButton->setIcon(QPixmap ( QString:: fromUtf8 ( ":/images/plus.png" )));

    connect(m_plusButton, &QAbstractButton::clicked,this, &VideoPlayer::addCurrentImage);


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
    controlLayout->addWidget(m_plusButton);

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
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Movie"));
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted){
        auto url=fileDialog.selectedUrls().constFirst();
        videoReader.open(url.toString().toStdString());
        if (!videoReader.isOpened()){
            m_errorLabel->setText(tr("Could not open url ")+url.toString());
            return;
        }

        std::cerr<<"Opened successfully\n";

        m_positionSlider->setRange(0, videoReader.get(CV_CAP_PROP_FRAME_COUNT));
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        m_playButton->setEnabled(true);
        m_plusButton->setEnabled(true);
        isPlaying=0;
        setPosition(0);
        setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());
    }
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
    setImage(imIn);
}

void VideoPlayer::setImage(const cv::Mat &img2Show){

    QImage _qimgR ( ( const uchar * ) ( img2Show.ptr<uchar> ( 0 ) ),
                    img2Show.cols,img2Show.rows, QImage::Format_RGB888 ) ;

    imgLabel-> setPixmap ( QPixmap::fromImage ( _qimgR.rgbSwapped() ) );
}

