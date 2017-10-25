#--------------------------------------------------------------------
# 3rdparty libraries path
#--------------------------------------------------------------------

include(../../config.pri)
URHO3D = $${URHO3D_BUILD_PATH}

#--------------------------------------------------------------------
# target
#--------------------------------------------------------------------

TEMPLATE = app
CONFIG -= console
CONFIG += windows
QT += widgets xml
CONFIG += c++11
CONFIG -= debug_and_release
CONFIG -= debug_and_release_target

#--------------------------------------------------------------------
# output directory
#--------------------------------------------------------------------

CONFIG(debug,debug|release) {
    DESTDIR = $$PWD/../../build
} else {
    DESTDIR = $$PWD/../../build
}

QMAKE_CLEAN += $$DESTDIR/$$TARGET

#--------------------------------------------------------------------
# compilation flags
#--------------------------------------------------------------------

unix:!macx: QMAKE_CXXFLAGS += -Wall
unix:!macx: QMAKE_CXXFLAGS += -Wno-comment
unix:!macx: QMAKE_CXXFLAGS += -Wno-ignored-qualifiers
unix:!macx: QMAKE_CXXFLAGS += -Wno-unused-parameter
unix:!macx: QMAKE_CXXFLAGS += -Wno-reorder
unix:!macx: QMAKE_CXXFLAGS += -std=c++11
#unix:!macx: QMAKE_CXXFLAGS += -fpermissive

CONFIG(debug,debug|release) {
#message( debug )
    QMAKE_CXXFLAGS_DEBUG -= -O0
} else {
#message( release )
    unix:!macx: QMAKE_CXXFLAGS += -Wno-strict-aliasing
    win32:QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
    QMAKE_CXXFLAGS_DEBUG += -O3
}

#--------------------------------------------------------------------
# defines
#--------------------------------------------------------------------

CONFIG(debug,debug|release) {
    #message( debug )
    DEFINES += \
        _DEBUG

} else {
    DEFINES += \

}

#--------------------------------------------------------------------
# libraries includes
#--------------------------------------------------------------------

INCLUDEPATH += $${URHO3D}/include
INCLUDEPATH += $${URHO3D}/include/Urho3D/ThirdParty
INCLUDEPATH += ../../spark/include

#--------------------------------------------------------------------
# check dependencies
#--------------------------------------------------------------------

PRE_TARGETDEPS += $${DESTDIR}/libspark.a

#--------------------------------------------------------------------
# linker
#--------------------------------------------------------------------

LIBS += -L$${URHO3D}/lib
LIBS += -L$${DESTDIR}

unix:!macx:: LIBS += -lUrho3D

win32: CONFIG(debug,debug|release) {
    LIBS += -lUrho3D_d
} else {
    LIBS += -lUrho3D
}

LIBS += -L$${DESTDIR} -lspark

unix:!macx: LIBS += -ldl -lGL -lGLU

win32:LIBS += kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib \
                ole32.lib oleaut32.lib uuid.lib imm32.lib winmm.lib wsock32.lib opengl32.lib glu32.lib version.lib

#--------------------------------------------------------------------
# project files
#--------------------------------------------------------------------

HEADERS += \


SOURCES += \
    main.cpp \
