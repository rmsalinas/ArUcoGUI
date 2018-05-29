
#ifndef modulearucotest_VIDEOPLAYER_H
#define modulearucotest_VIDEOPLAYER_H

#include <QWidget>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <memory>
class QAbstractButton;
class QSlider;
class QLabel;
class QUrl;
namespace modulearucotest {
class VideoImagePlayerBase;
class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer();


    cv::Mat getLastImageSelected();
    int getFramePos();
public slots:
    void openVideoFile();
    void openImages();
    void setImage(  cv::Mat &img2Show);

private slots:
    void playPauseButtonClicked( );
    void setPosition(int position);
    void nextFrame();
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
};
}

#endif
