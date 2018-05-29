
#ifndef  _VIDEOPLAYER_H
#define _VIDEOPLAYER_H

#include <QWidget>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <memory>
#include "videoplayer_exports.h"
class QAbstractButton;
class QSlider;
class QLabel;
class QUrl;
 class VideoImagePlayerBase;
class APP_VIDEOPLAYER_TOOLS_API VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer();


    cv::Mat getLastImageSelected();
    std::string getCurrentImageInfo();
public slots:
    void openVideoFile();
    void openImages();
    void setImage(  cv::Mat &img2Show);

private slots:
    void playPauseButtonClicked( );
    void setPosition(int position);
    void sliderReleased( );
    void valueChanged(int);
    void playNextFrame();
    void addCurrentImage();
signals:
    void imageSelected();
    void newImage(cv::Mat &);
private:
    QAbstractButton *m_playButton;
    QSlider *m_positionSlider;
    QLabel *m_errorLabel;
    cv::Mat imIn,selectedImage;
    QLabel *imgLabel;
    int isPlaying=0;
    std::shared_ptr<VideoImagePlayerBase> _reader;

    void prepareForOpenedReader();
    bool grabAndShow();
}; 

#endif
