CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -I/usr/include/opencv4 -I/usr/include/aarch64-linux-gnu/qt5 -I/usr/include/aarch64-linux-gnu/qt5/QtWidgets -I/usr/include/aarch64-linux-gnu/qt5/QtGui -I/usr/include/aarch64-linux-gnu/qt5/QtCore
LDFLAGS = -L/usr/lib/aarch64-linux-gnu -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopencv_dnn -lQt5Widgets -lQt5Gui -lQt5Core

OBJS = src/main.o src/utils.o src/MainWindow.o src/mainwindow_moc.o src/VideoWidget.o src/videowidget_moc.o

yolov7_example: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

src/main.o: src/main.cpp src/MainWindow.h src/utils.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/utils.o: src/utils.cpp src/utils.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/MainWindow.o: src/MainWindow.cpp src/MainWindow.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/mainwindow_moc.o: src/mainwindow_moc.cpp src/MainWindow.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/mainwindow_moc.cpp: src/MainWindow.h
	/usr/lib/aarch64-linux-gnu/qt5/bin/moc $< -o $@
	
src/videowidget_moc.o: src/videowidget_moc.cpp src/VideoWidget.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/videowidget_moc.cpp: src/VideoWidget.h
	/usr/lib/aarch64-linux-gnu/qt5/bin/moc $< -o $@

clean:
	rm -f $(OBJS) src/mainwindow_moc.cpp yolov7_example

.PHONY: clean
