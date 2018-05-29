#include "calibrationcontrolpanel.h"
#include "ui_calibrationcontrolpanel.h"
#include <QListWidgetItem>
#include <QMessageBox>
calibrationControlPanel::calibrationControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::calibrationControlPanel)
{
    ui->setupUi(this);
    shownImage=new QLabel(0);
}

calibrationControlPanel::~calibrationControlPanel()
{
    delete ui;
}

void calibrationControlPanel::on_pb_images_remove_clicked()
{
    if (ui->listWidget->currentItem()==nullptr) return;
    if ( QMessageBox::question ( this, tr ( "Remove Image" ), tr ( "Remove the selected image?" ), QMessageBox::Yes,QMessageBox::No ) ==QMessageBox::Yes ){
        int ci=ui->listWidget->currentRow();
        ui->listWidget->takeItem(ci);

        if (ImagesMarkers.size()==1) ImagesMarkers.clear();
        else {
            auto it=ImagesMarkers.begin();
            advance(it,ci);
            ImagesMarkers.erase(it);
        }
        shownImage->hide();
      }
}

void calibrationControlPanel::on_pb_images_clear_clicked()
{
    if(ui->listWidget->count()==0) return;
    if ( QMessageBox::question ( this, tr ( "Remove All Images" ), tr ( "Remove all the images?" ), QMessageBox::Yes,QMessageBox::No ) ==QMessageBox::Yes ){
        ImagesMarkers.clear();
        ui->listWidget->clear();
        shownImage->hide();
      }
}

void calibrationControlPanel::add(const cv::Mat &image,const std::vector<aruco::Marker> &markers,QString info)
{
    //if the image is already, replace it
    for(auto &elem:ImagesMarkers){
        if ( elem.info==info){
            elem={image.clone(),markers,info};
            return;
        }
    }
    ui->listWidget->addItem(info);
    ImagesMarkers.push_back( {image.clone(),markers,info});

}

void calibrationControlPanel::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    int ci=ui->listWidget->currentRow();
    if(ci<0)return;
    auto elem=ImagesMarkers.begin();
    std::advance(elem,ci);
    shownImage->setWindowTitle(elem->info);
    shownImage->setScaledContents(true);
    QImage _qimgR ( ( const uchar * ) ( elem->image.ptr<uchar> ( 0 ) ),
                    elem->image.cols,elem->image.rows, QImage::Format_RGB888 ) ;

    shownImage-> setPixmap ( QPixmap::fromImage ( _qimgR.rgbSwapped() ) );
    shownImage->show();


}
