#ifndef CALIBRATIONCONTROLPANEL_H
#define CALIBRATIONCONTROLPANEL_H

#include <QWidget>
#include <QListWidgetItem>
#include <QLabel>
#include "aruco/aruco.h"
#include <opencv2/core.hpp>
namespace Ui {
class calibrationControlPanel;
}

class calibrationControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit calibrationControlPanel(QWidget *parent = 0);
    ~calibrationControlPanel();

    void add(const cv::Mat &image,const std::vector<aruco::Marker> &markers,QString info);
private slots:
    void on_pb_images_remove_clicked();

    void on_pb_images_clear_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::calibrationControlPanel *ui;
    struct ImageInfo{
      cv::Mat image;
        std::vector<aruco::Marker> markers;
        QString info;
    };
    QLabel *shownImage ;

    std::list<ImageInfo> ImagesMarkers;
};

#endif // CALIBRATIONCONTROLPANEL_H
