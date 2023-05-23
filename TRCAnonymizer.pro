TARGET = TRCAnonymizer
TEMPLATE = app

####################################### CONFIG
macx:CONFIG += app_bundle
CONFIG += qt
CONFIG += c++1z
CONFIG -= console
QT += core gui widgets core5compat

####################################### COMPILATION FLAGS
win32{
    QMAKE_CXXFLAGS += /O2 /openmp /W3
    DEFINES += _CRT_SECURE_NO_WARNINGS
}
unix:!macx{
    QMAKE_CXXFLAGS += -fopenmp -O3 -fpermissive
    QMAKE_CXXFLAGS_WARN_ON += -Wno-maybe-uninitialized -Wno-unused-result -Wno-unused-function -Wno-unused-parameter -Wno-comment -Wno-sign-compare -Wno-deprecated-declarations -Wno-unused-variable -Wno-unused-local-typedefs -Wno-reorder -Wno-switch #-Wfatal-errors -Werror
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
macx{
    QMAKE_CXXFLAGS += -O3
    QMAKE_CXXFLAGS_WARN_ON += -Wno-maybe-uninitialized -Wno-unused-result -Wno-unused-function -Wno-unused-parameter -Wno-comment -Wno-sign-compare -Wno-deprecated-declarations -Wno-unused-variable -Wno-unused-local-typedefs -Wno-reorder -Wno-switch #-Wfatal-errors -Werror
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15 #allow acces to C++ 17 std::filesystem
}

####################################### PROJECT FILES
RESOURCES += \
    trcanonymizer.qrc

SOURCES += \
    MicromedFile.cpp \
    TRCAnonymizer.cpp \
    Utility.cpp \
    main.cpp

FORMS += \
    TRCAnonymizer.ui

HEADERS += \
    MicromedFile.h \
    TRCAnonymizer.h \
    TRCParameters.h \
    Utility.h

### Define Icon on Mac : https://doc.qt.io/qt-5/appicon.html
macx{
    ICON =  Resources/trcanonymizer.icns
}
