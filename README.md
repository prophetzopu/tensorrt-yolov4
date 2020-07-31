## TensorRT-YOLOv4 For TensorRT-7.1.0

### Performance
| Model       | Input Size | GPU      | Mode   | Inference Time / FPS |
|----------------|------------|----------|--------|---------------|
| [YOLOv4](https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov4.cfg)   | 608x608    | gtx 1080Ti |float32 |    23.3 ms    |
| [YOLOv4](https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov4.cfg)   | 416x416    | gtx 1080Ti |float32 |    13.0 ms    |
| [YOLOv3](https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov3.cfg)   | 608x608   | gtx 1080Ti |float32 |    18.2 ms    |
| [YOLOv3](https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov3.cfg)   | 416x416   | gtx 1080Ti |float32 |    10.0 ms    |
| [YOLOv3-Tiny](https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov3-tiny.cfg)   |608x608   | gtx 1080Ti |float32 |    3.31 ms    |
| [YOLOv3-Tiny](https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov3-tiny.cfg)   | 416x416   | gtx 1080Ti |float32 |    2.01 ms    |
| [YOLOv3-Tiny-Prn](https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov3-tiny-prn.cfg) | 608x608    | gtx 1080Ti |float32 |    3.05 ms    |
| [YOLOv3-Tiny-Prn](https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov3-tiny-prn.cfg) | 416x416   | gtx 1080Ti |float32 |    2.01 ms    |
| [YOLOv3](https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov3.cfg)   | 608x608   | TX2 MAXN |float16 |    6 FPS    |
| [YOLOv4](https://github.com/AlexeyAB/darknet/blob/master/cfg/yolov4.cfg)   | 416x416   | TX2 MAXN |float16 |    7 FPS    |
- Note that including pre-processing and post-processing time.

### Enviroments
- Jetson TX2
```
Ubuntu 18.04
TensorRT 7.1.0
CUDA 10.2
cuDNN 8.0.0
onnx 1.4.1
OpenCV 4.1.1
CMake 3.10.2
```

### Models
- Modify [net] and [yolo] segments of yolovx.cfg.
```
[net]
batch=1
subdivisions=8
width=416
height=416
...

[yolo]
mask = 0,1,2
anchors = ...
down_stride = 8
classes=80
infer_thresh = 0.25
num=9
jitter=.3
ignore_thresh = .7
truth_thresh = 1
scale_x_y = 1.2
iou_thresh=0.213
...

[yolo]
mask = 3,4,5
anchors = ...
down_stride = 16
infer_thresh = 0.25
classes=80
num=9
jitter=.3
ignore_thresh = .7
truth_thresh = 1
scale_x_y = 1.1
iou_thresh=0.213
...

[yolo]
mask = 6,7,8
anchors = ...
classes=80
down_stride = 32
infer_thresh = 0.25
num=9
jitter=.3
ignore_thresh = .7
truth_thresh = 1
random=1
scale_x_y = 1.05
iou_thresh=0.213
...
```

- Convert darknet yolo to onnx. 
```
python3 tools/yolo_to_onnx.py --cfg model/yolov4.cfg --weights model/yolov4.weights --out model/yolov4.onnx
```

### Example
```bash
git clone https://github.com/hlld/tensorrt-yolov4.git
cd tensorrt-yolov4
mkdir build
cd build && cmake .. && make
cd ..

## YOLOv3
./buildEngine -i model/yolov3.onnx -o model/yolov3.engine -b 1 -m 0
./runDet -i model/yolov3.engine -p dog.jpg -v nuscenes_mini.mp4
### use web camera.
./videoDet -t model/yolov3.engine -c 1

## YOLOv4
./buildEngine -i model/yolov4.onnx -o model/yolov4.engine -b 1 -m 0
./runDet -i model/yolov4.engine -p dog.jpg -v nuscenes_mini.mp4
### use web camera.
./videoDet -e model/yolov4.engine -c 1
```

### Related projects
* [TensorRT-YOLOv4](https://github.com/CaoWGG/TensorRT-YOLOv4)
* [DarkNet](https://github.com/AlexeyAB/darknet)
* [TensorRT-CenterNet](https://github.com/CaoWGG/TensorRT-CenterNet)
* [TensorRT](https://github.com/NVIDIA/TensorRT)
* [onnx-tensorrt](https://github.com/onnx/onnx-tensorrt)
