QT_PATH = /usr/local/Qt-5.15.9

CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++11 -I /usr/include/opencv4 -I $(QT_PATH)/include -I $(QT_PATH)/include/QtWidgets -I $(QT_PATH)/include/QtGui -I $(QT_PATH)/include/QtCore
LDFLAGS = -L/usr/lib/aarch64-linux-gnu -L $(QT_PATH)/Qt-5.15.9/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopencv_dnn -lQt5Core -lQt5Gui -lQt5Widgets

MAIN_OBJS = src/main.o src/yolo.o src/camera.o
GUI_OBJS = src/gui/VideoWidget.o src/gui/videowidget_moc.o src/gui/MainWindow.o src/gui/mainwindow_moc.o src/gui/TimerHandler.o src/gui/timerhandler_moc.o
OBJS = $(MAIN_OBJS) $(GUI_OBJS)

CLEAN_FILES = $(OBJS) yolo_detect src/gui/mainwindow_moc.cpp src/gui/videowidget_moc.cpp src/gui/timerhandler_moc.cpp src/gui/ui_MainWindow.h

yolo_detect: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

src/main.o: src/main.cpp src/gui/ui_MainWindow.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
src/yolo.o: src/yolo.cpp src/yolo.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/camera.o: src/camera.cpp src/camera.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
src/gui/MainWindow.o: src/gui/MainWindow.cpp src/gui/MainWindow.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/gui/mainwindow_moc.o: src/gui/mainwindow_moc.cpp src/gui/MainWindow.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/gui/mainwindow_moc.cpp: src/gui/MainWindow.h
	$(QT_PATH)/bin/moc $< -o $@
	
src/gui/videowidget_moc.o: src/gui/videowidget_moc.cpp src/gui/VideoWidget.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/gui/videowidget_moc.cpp: src/gui/VideoWidget.h
	$(QT_PATH)/bin/moc $< -o $@

src/gui/ui_MainWindow.h: src/gui/MainWindow.ui
	$(QT_PATH)/bin/uic $< -o $@
	
src/gui/TimerHandler.o: src/gui/TimerHandler.cpp src/gui/TimerHandler.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/gui/timerhandler_moc.o: src/gui/timerhandler_moc.cpp src/gui/TimerHandler.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/gui/timerhandler_moc.cpp: src/gui/TimerHandler.h
	$(QT_PATH)/bin/moc $< -o $@

clean:
	rm -f $(CLEAN_FILES)

.PHONY: clean
