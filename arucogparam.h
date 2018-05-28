#ifndef ARUCOGPARAM_H
#define ARUCOGPARAM_H
#include "gparam/gparam.h"
#include <aruco/aruco.h>
#include <iostream>
class ArucoGParams: public gparam::ParamSet {
public:
    ArucoGParams(){
        setName("ArucoParams");
        insert(gparam::Param("Dictionary", aruco::Dictionary::getDicTypes(),0));
        insert(gparam::Param("DetectionMode", { "DM_NORMAL","DM_FAST","DM_VIDEO_FAST"},0));
        insert(gparam::Param("CornerMethod", { "CORNER_SUBPIX","CORNER_LINES","CORNER_NONE"},0));
        insert(gparam::Param("MinMarkerSize", 0.02,0.,0.5,0.01));
        insert(gparam::Param("Threshold", 7,1,30,2));
        insert(gparam::Param("Enclosed", false));
    }

    //used as singleton
    static ArucoGParams & get(){

        static ArucoGParams    instance; // Guaranteed to be destroyed.
                                         // Instantiated on first use.
         return instance;

    }

    //loads the detector from the params
    static void loadFromParams(aruco::MarkerDetector &mdetector){

       const ArucoGParams & ap= get();
       mdetector.setDictionary(ap["Dictionary"].asString());
       mdetector.getParameters().setDetectionMode( aruco::MarkerDetector::Params::getDetectionModeFromString( ap["DetectionMode"].asString()),ap["MinMarkerSize"].asDouble());
       mdetector.getParameters().setCornerRefinementMethod( aruco::MarkerDetector::Params::getCornerRefinementMethodFromString(ap["CornerMethod"].asString()) );
       mdetector.getParameters().detectEnclosedMarkers(   ap["Enclosed"].asInt());
       mdetector.getParameters().ThresHold=ap["Threshold"].asInt();
    }
private:
};

class ArucoMarkerDetector{


public:
    static aruco::MarkerDetector & get(bool updateParams=true){
            static aruco::MarkerDetector mdetector;
            if (updateParams) ArucoGParams::loadFromParams(mdetector);
            return mdetector;
    }
};

#endif // ARUCOGPARAM_H
