INCLUDE_DIRECTORIES(.)


SET(hdrs gparam.h exceptions.h encryptor.h)
SET(srcs encryptor.cpp gparam.cpp)


ADD_LIBRARY(gparam ${srcs} ${hdrs})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ) 

SET_TARGET_PROPERTIES(gparam PROPERTIES 				# create *nix style library versions + symbolic links
    DEFINE_SYMBOL  GPARAM_DSO_EXPORTS
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_SOVERSION}
    CLEAN_DIRECT_OUTPUT 1 					# allow creating static and shared libs without conflicts
    OUTPUT_NAME "gparam${PROJECT_DLLVERSION}" 	# avoid conflicts between library and binary target names
    )


INSTALL(TARGETS gparam
    RUNTIME DESTINATION bin COMPONENT main			# Install the dll file in bin directory
    LIBRARY DESTINATION lib PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT main
    ARCHIVE DESTINATION lib COMPONENT main)			# Install the dll.a file in lib directory

IF ( (NOT BUILD_MODE ) OR ("${BUILD_MODE}" STREQUAL "dev"))
INSTALL(FILES ${hdrs_base}
    DESTINATION include/gparam
    COMPONENT main)
ENDIF()

#####################################
#   Qt module
#####################################


IF(BUILD_QT)
SET(SRCS_APP        paramsetwdgt.cpp paramsetdlg.cpp)
SET(MOC_HDRS_APP    paramsetwdgt.h paramsetdlg.h)

IF(BUILD_QT4)
    FIND_PACKAGE(Qt4 REQUIRED)
    SET(REQUIRED_LIBRARIES_QT  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
    INCLUDE(${QT_USE_FILE})
    QT4_WRAP_CPP(MOC_SRCS_APP     ${MOC_HDRS_APP})
    QT4_WRAP_UI(MOC_UIS_APP       ${UIS_APP})
    ADD_LIBRARY(gparam_qt ${MOC_HDRS_APP} ${SRCS_APP} ${MOC_SRCS_APP} ${MOC_UIS_APP} ${RCC_SRCS} )
    TARGET_LINK_LIBRARIES(gparam_qt  gparam ${REQUIRED_LIBRARIES_QT} )

ELSE()

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    find_package(Qt5Widgets REQUIRED)

    ADD_LIBRARY(gparam_qt ${MOC_HDRS_APP} ${SRCS_APP} )
    qt5_use_modules(gparam_qt Widgets Network)
    TARGET_LINK_LIBRARIES(gparam_qt  ${QT_LIBRARIES}  ${REQUIRED_LIBRARIES} )
    SET(REQUIRED_LIBRARIES_QT  ${QT_LIBRARIES})
ENDIF()

SET(PROJECT_QT_CREATED_FLAG "YES" PARENT_SCOPE)
SET(LIB_COMPONENTS ${LIB_COMPONENTS} gparam_qt PARENT_SCOPE)
SET(REQUIRED_LIBRARIES_QT ${REQUIRED_LIBRARIES_QT} PARENT_SCOPE)


SET_TARGET_PROPERTIES(gparam_qt PROPERTIES 				# create *nix style library versions + symbolic links
    DEFINE_SYMBOL GPARAM_DSO_EXPORTS
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_SOVERSION}
    CLEAN_DIRECT_OUTPUT 1 					# allow creating static and shared libs without conflicts
    OUTPUT_NAME "gparam_qt${PROJECT_DLLVERSION}" 	# avoid conflicts between library and binary target names
    )


INSTALL(TARGETS gparam_qt
    RUNTIME DESTINATION bin COMPONENT main			# Install the dll file in bin directory
    LIBRARY DESTINATION lib PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE COMPONENT main
    ARCHIVE DESTINATION lib COMPONENT main)			# Install the dll.a file in lib directory

INSTALL(FILES paramsetwdgt.h paramsetdlg.h
    DESTINATION include/gparam
    COMPONENT main)

ENDIF()
