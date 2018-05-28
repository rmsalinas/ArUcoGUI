#ifndef ARUCOGPARAM_H
#define ARUCOGPARAM_H
#include "gparam/gparam.h"
#include "aruco.h"
class ArucoGParams: public gparam::ParamSet {
public:
    ArucoGParams(){
        setName("ArucoParams");
        insert(gparam::Param("Dictionary", aruco::Dictionary::getDicTypes(),0));
        insert(gparam::Param("DetectionMode", { "DM_NORMAL","DM_FAST","DM_VIDEO_FAST"},0));
        insert(gparam::Param("CornerMethod", { "CORNER_SUBPIX","CORNER_LINES","CORNER_NONE"},0));
        insert(gparam::Param("Enclosed", false));
    }

    //used as singleton
    static ArucoGParams & get(){

        static ArucoGParams    instance; // Guaranteed to be destroyed.
                                         // Instantiated on first use.
         return instance;

    }
private:
};
#endif // ARUCOGPARAM_H
