CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -I/usr/include/opencv4 -I/usr/local/Qt-5.15.9/include -I/usr/local/Qt-5.15.9/include/QtWidgets -I/usr/local/Qt-5.15.9/include/QtGui -I/usr/local/Qt-5.15.9/include/QtCore
LDFLAGS = -L/usr/lib/aarch64-linux-gnu -L/usr/local/Qt-5.15.9/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopencv_dnn -lQt5Core -lQt5Gui -lQt5Widgets

OBJS = src/main.o src/utils.o src/mainwindow_moc.o src/VideoWidget.o src/videowidget_moc.o src/ui_MainWindow.h src/MainWindow.o

yolov7_example: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

src/main.o: src/main.cpp src/utils.h src/ui_MainWindow.h src/MainWindow.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
src/MainWindow.o: src/MainWindow.cpp src/MainWindow.h src/VideoWidget.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/utils.o: src/utils.cpp src/utils.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/mainwindow_moc.o: src/mainwindow_moc.cpp src/MainWindow.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/mainwindow_moc.cpp: src/MainWindow.h
	/usr/local/Qt-5.15.9/bin/moc $< -o $@
	
src/videowidget_moc.o: src/videowidget_moc.cpp src/VideoWidget.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/videowidget_moc.cpp: src/VideoWidget.h
	/usr/local/Qt-5.15.9/bin/moc $< -o $@

src/ui_MainWindow.h: src/MainWindow.ui
	/usr/local/Qt-5.15.9/bin/uic $< -o $@

clean:
	rm -f $(OBJS) src/mainwindow_moc.cpp src/videowidget_moc.cpp src/ui_MainWindow.h yolov7_example

.PHONY: clean
