
#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoFrame>
#include <QVideoProbe>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
class QAbstractButton;
class QSlider;
class QLabel;
class QUrl;

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer();


    cv::Mat getLastImageSelected();
public slots:
    void openFile();

private slots:
    void playPauseButtonClicked( );
    void setPosition(int position);
    void setImage(const cv::Mat &img2Show);
    void nextFrame();
    void addCurrentImage();
signals:
    void imageSelected();
private:
    QAbstractButton *m_playButton,*m_plusButton;
    QSlider *m_positionSlider;
    QLabel *m_errorLabel;
    cv::Mat imIn,selectedImage;
    cv::VideoCapture videoReader;
    QLabel *imgLabel;
    int isPlaying=0;
};

#endif
