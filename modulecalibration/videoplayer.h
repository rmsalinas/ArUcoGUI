
#ifndef modulecalibration_VIDEOPLAYER_H
#define modulecalibration_VIDEOPLAYER_H

#include <QWidget>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "tools/moduletools/exports.h"
class QAbstractButton;
class QSlider;
class QLabel;
class QUrl;
namespace modulecalibration{
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
    void setImage(  cv::Mat &img2Show);
    void nextFrame();
    void addCurrentImage();
signals:
    void imageSelected();
    void newImage(cv::Mat &);
private:
    QAbstractButton *m_playButton,*m_plusButton;
    QSlider *m_positionSlider;
    QLabel *m_errorLabel;
    cv::Mat imIn,selectedImage;
    cv::VideoCapture videoReader;
    QLabel *imgLabel;
    int isPlaying=0;
};
}
#endif
