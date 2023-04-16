# YOLO C++ API with GUI

This is a YOLO C++ API project which provides a graphical user interface (GUI) for users to customize settings and perform real-time object detection using their webcam. This project has been developed and tested on a Jetson Nano device.

## Dependencies

The following dependencies are required to build and run this project. Make sure to install the correct versions as specified below:

- OpenCV (4.x.x)
- Qt (5.15.9)

## How to run the project

1. Clone the repository and navigate to the project directory:

   ```
   git clone https://github.com/ethanlaj/yolo-cpp-api.git
   cd yolo-cpp-api
   ```

2. Compile the project:

   ```
   make
   ```

3. Run the compiled binary:

   ```
   ./yolo_detect
   ```

4. The GUI will open, allowing you to choose a specific model to use. This project only supports .weights formatted models.

5. Click the "Start" button to begin real-time object detection using your webcam. Detected objects will be displayed with bounding boxes and labels.

## GUI Features

- Ability to use different models
- While running, can customize object detection settings such as confidence and NMS thresholds.
- Start and stop real-time object detection.
- Display detected objects with bounding boxes and labels.

## Troubleshooting

### Camera frames not capturing

If frames cannot be captured by the camera, try restarting the `nvargus-daemon` by running the following command:

```
sudo systemctl restart nvargus-daemon
```
