TARGET = practica1
TEMPLATE = app
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS += -std=c++11
mac:QMAKE_CXXFLAGS += -stdlib=libc++
mac:QMAKE_LFLAGS += -stdlib=libc++

#Ensure that project builds in 32 bit mode
#QMAKE_CFLAGS += -m32
#QMAKE_CXXFLAGS += -m32
#QMAKE_LFLAGS += -m32

DEFINES += _CRT_SECURE_NO_WARNINGS GLEW_STATIC

INCLUDEPATH += ../lib

LIBS += -L../lib/glew -L../lib/glfw
win32:LIBS += -lglew32s.win32 -lglfw3.win32 -lopengl32 -lgdi32 -luser32
mac:LIBS += -lGLEW.mac -lglfw3.mac -framework AppKit -framework CoreVideo -framework IOKit -framework OpenGL
unix:!mac:LIBS += -lGLEW.linux -lglfw3.linux -lGL -lX11 -lXrandr

SOURCES += \
    ../src/camera.cpp \
    ../src/entity.cpp \
    ../src/main.cpp \
    ../src/mesh.cpp \
    ../src/model.cpp \
    ../src/renderer.cpp \
    ../src/resourcemanager.cpp \
    ../src/scene.cpp \
    ../src/screen.cpp \
    ../src/submesh.cpp

HEADERS += \
    ../include/array.h \
    ../include/camera.h \
    ../include/entity.h \
    ../include/mesh.h \
    ../include/model.h \
    ../include/renderer.h \
    ../include/resourcemanager.h \
    ../include/scene.h \
    ../include/screen.h \
    ../include/smartptr.h \
    ../include/string.h \
    ../include/submesh.h \
    ../include/texture.h \
    ../include/types.h \
    ../include/u-gine.h \
    ../include/vertex.h \
    ../lib/stb_image.h

DISTFILES += \
    ../data/fragment.glsl \
    ../data/vertex.glsl
