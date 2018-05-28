SET(MODULENAME modulearucotest)
SET(${MODULENAME}_SRC   modulearucotest.cpp videoplayer.cpp)
SET(${MODULENAME}_HDR   modulearucotest.h videoplayer.h  modulearucotest_exports.h)
#SET(${MODULENAME}_UIS  ModuleCalibration.ui)

include_directories(.)
QT5_WRAP_UI(${MODULENAME}_MOC_UIS_APP       ${${MODULENAME}_UIS})
ADD_LIBRARY(${MODULENAME} ${${MODULENAME}_SRC} ${${MODULENAME}_HDR} ${${MODULENAME}_MOC_SRCS_APP} ${${MODULENAME}_MOC_UIS_APP})

qt5_use_modules(${MODULENAME} Widgets Multimedia MultimediaWidgets )

SET_TARGET_PROPERTIES(${MODULENAME} PROPERTIES 				# create *nix style library versions + symbolic links
        DEFINE_SYMBOL APP_MODULE_ARUCOTEST_DSO_EXPORTS
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_SOVERSION}
        CLEAN_DIRECT_OUTPUT 1 					# allow creating static and shared libs without conflicts
        OUTPUT_NAME "${MODULENAME}${PROJECT_DLLVERSION}" 	# avoid conflicts between library and binary target names
)



TARGET_LINK_LIBRARIES(${MODULENAME}  ${QT_LIBRARIES} moduletools opencv_core opencv_imgproc opencv_highgui aruco)

INSTALL(TARGETS ${MODULENAME}
        RUNTIME DESTINATION bin COMPONENT main			# Install the dll file in bin directory
        LIBRARY DESTINATION lib PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT main
        ARCHIVE DESTINATION lib COMPONENT main)			# Install the dll.a file in lib directory
