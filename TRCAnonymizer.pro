TARGET = TRCAnonymizer
TEMPLATE = app

####################################### CONFIG
macx:CONFIG += app_bundle
CONFIG += qt
CONFIG += c++1z
CONFIG -= console
QT += core gui widgets core5compat

COMPUTER = "flo_mac_work" # ben_win_work, flo_win_home, flo_win_work, flo_mac_work, ben_linux_work

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

INCLUDE_BOOST = ""
INCLUDE_FFTW = ""
INCLUDE_EEGFORMAT = ""
INCLUDE_FRAMEWORK = ""
INCLUDE_LOCALIZER = ""
LIB_BOOST = ""
LIB_FFTW = ""
LIB_EEGFORMAT = ""
LIB_FRAMEWORK = ""
LIB_MISC = ""

equals(COMPUTER, "ben_win_work"){
    INCLUDE_BOOST = "C:/boost/boost_1_78_0"
    INCLUDE_FFTW = "D:/HBP/FFTW_3.3.4_x64"
    INCLUDE_EEGFORMAT = "D:/HBP/EEGFormat/EEGFormat/EEGFormat"
    INCLUDE_FRAMEWORK = "D:/HBP/Framework/Framework"
    INCLUDE_LOCALIZER = "D:/HBP/Localizer"
    LIB_BOOST = -L"C:/boost/boost_1_78_0/lib64-msvc-14.2" -l"libboost_filesystem-vc142-mt-x64-1_78"
    LIB_FFTW = -L"D:/HBP/FFTW_3.3.4_x64" -llibfftw3f-3
    LIB_EEGFORMAT = -L"D:/HBP/EEGFormat/EEGFormat/x64/Release/" -lEEGFormat
    LIB_FRAMEWORK = -L"D:/HBP/Framework_x64/release" -lFramework
    LIB_MISC = Comdlg32.lib shell32.lib
}

equals(COMPUTER, "ben_win_work_new"){
    INCLUDE_BOOST = "C:/local/boost_1_78_0"
    INCLUDE_FFTW = "C:/local/FFTW_3.3.4_x64"
    INCLUDE_EEGFORMAT = "C:/HBP/Software/EEGFormat/EEGFormat"
    INCLUDE_FRAMEWORK = "C:/HBP/Software/Framework/Framework"
    INCLUDE_LOCALIZER = "C:/HBP/Software/Localizer"
    LIB_BOOST = -L"C:/local/boost_1_78_0/lib64-msvc-14.2" -l"libboost_filesystem-vc142-mt-x64-1_78"
    LIB_FFTW = -L"C:/local/FFTW_3.3.4_x64" -llibfftw3f-3
    LIB_EEGFORMAT = -L"C:/HBP/Software/EEGFormat/x64/Release/" -lEEGFormat
    LIB_FRAMEWORK = -L"C:/HBP/Software/Framework_x64/release" -lFramework
    LIB_MISC = Comdlg32.lib shell32.lib
}

equals(COMPUTER, "flo_win_home"){
    INCLUDE_BOOST = ""
    INCLUDE_FFTW = "C:/Users/Florian/Documents/Arbeit/Software/DLL/FFTW_3.3.4_x64"
    INCLUDE_EEGFORMAT = "C:/Users/Florian/Documents/Arbeit/Repository/C++/EEGFormat/EEGFormat"
    INCLUDE_FRAMEWORK = "C:/Users/Florian/Documents/Arbeit/Repository/C++/Framework/Framework"
    INCLUDE_LOCALIZER = "C:/Users/Florian/Documents/Arbeit/Repository/C++/Localizer"
    LIB_BOOST = ""
    LIB_FFTW = -L"C:/Users/Florian/Documents/Arbeit/Software/DLL/FFTW_3.3.4_x64" -llibfftw3f-3
    LIB_EEGFORMAT = -L"C:/Users/Florian/Documents/Arbeit/Repository/C++/EEGFormat/x64/Release/" -lEEGFormat
    LIB_FRAMEWORK = -L"C:/Users/Florian/Documents/Arbeit/Repository/C++/Framework/x64/Release/" -lFramework
    LIB_MISC = Comdlg32.lib shell32.lib
}

equals(COMPUTER, "flo_win_work"){
    INCLUDE_BOOST = ""
    INCLUDE_FFTW = "D:/Users/Florian/Documents/Arbeit/Software/DLL/FFTW_3.3.4_x64"
    INCLUDE_EEGFORMAT = "D:/Users/Florian/Documents/Arbeit/Repository/C++/EEGFormat/EEGFormat"
    INCLUDE_FRAMEWORK = "D:/Users/Florian/Documents/Arbeit/Repository/C++/Framework/Framework"
    INCLUDE_LOCALIZER = "D:/Users/Florian/Documents/Arbeit/Repository/C++/Localizer"
    LIB_BOOST = ""
    LIB_FFTW = -L"D:/Users/Florian/Documents/Arbeit/Software/DLL/FFTW_3.3.4_x64" -llibfftw3f-3
    LIB_EEGFORMAT = -L"D:/Users/Florian/Documents/Arbeit/Repository/C++/EEGFormat/x64/Release/" -lEEGFormat
    LIB_FRAMEWORK = -L"D:/Users/Florian/Documents/Arbeit/Repository/C++/Framework/x64/Release/" -lFramework
    LIB_MISC = Comdlg32.lib shell32.lib
}

equals(COMPUTER, "flo_mac_work"){
    INCLUDE_BOOST = "/opt/homebrew/Cellar/boost/1.80.0/include"
    INCLUDE_FFTW = "/opt/homebrew/Cellar/fftw/3.3.10_1/include"
    INCLUDE_EEGFORMAT = "/Users/florian/Documents/Arbeit/Repository/C++/EEGFormat/EEGFormat"
    INCLUDE_FRAMEWORK = "/Users/florian/Documents/Arbeit/Repository/C++/Framework/Framework"
    INCLUDE_LOCALIZER = "/Users/florian/Documents/Arbeit/Repository/C++/TRCAnonymizer"
    LIB_BOOST = -L"/opt/homebrew/Cellar/boost/1.80.0/lib" -lboost_filesystem
    LIB_FFTW = -L"/opt/homebrew/Cellar/fftw/3.3.10_1/lib" -lfftw3f.3 -lfftw3f_threads.3
    LIB_EEGFORMAT = -L"/Users/florian/Documents/Arbeit/Repository/C++/EEGFormat/EEGFormat" -lEEGFormat
    LIB_FRAMEWORK = -L"/Users/florian/Documents/Arbeit/Repository/C++/Framework/Framework" -lFramework
    LIB_MISC = ""
}

equals(COMPUTER, "ben_linux_work"){
    INCLUDE_BOOST = ""
    INCLUDE_FFTW = ""
    INCLUDE_EEGFORMAT = "/home/hbp/software/EEGFormat/EEGFormat"
    INCLUDE_FRAMEWORK = "/home/hbp/software/Framework/Framework"
    INCLUDE_LOCALIZER = "/home/hbp/software/Localizer/Localizer"
    LIB_BOOST = -lboost_filesystem
    LIB_FFTW = -L"/usr/local/lib" -lfftw3f -lfftw3f_threads
    LIB_EEGFORMAT = -L"/home/hbp/software/EEGFormat/EEGFormat" -lEEGFormat
    LIB_FRAMEWORK = -L"/home/hbp/software/Framework_x64" -lFramework
    LIB_MISC = -fopenmp
}

INCLUDEPATH += $$INCLUDE_BOOST
INCLUDEPATH += $$INCLUDE_FFTW
INCLUDEPATH += $$INCLUDE_EEGFORMAT
INCLUDEPATH += $$INCLUDE_FRAMEWORK
INCLUDEPATH += $$INCLUDE_LOCALIZER

LIBS += $$LIB_BOOST
LIBS += $$LIB_FFTW
LIBS += $$LIB_EEGFORMAT
LIBS += $$LIB_FRAMEWORK
LIBS += $$LIB_MISC

####################################### PROJECT FILES

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


