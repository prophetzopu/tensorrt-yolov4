//
// Created by Hlld on 20-7-23.
//

#include <trt.h>
#include <getopt.h>
#include <memory>
#include <string>
#include <chrono>
#include <map>
#include <vector>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

std::string get_name_by_id(int id)
{
    static std::map<int, std::string> coco_ids;

    if (coco_ids.empty()) {
        coco_ids[0] = "person"; coco_ids[1] = "bicycle"; coco_ids[2] = "car"; coco_ids[3] = "motorcycle"; coco_ids[4] = "airplane";
        coco_ids[5] = "bus"; coco_ids[6] = "train"; coco_ids[7] = "truck"; coco_ids[8] = "boat"; coco_ids[9] = "traffic light";
        coco_ids[10] = "fire hydrant"; coco_ids[11] = "stop sign"; coco_ids[12] = "parking meter"; coco_ids[13] = "bench"; coco_ids[14] = "bird";
        coco_ids[15] = "cat"; coco_ids[16] = "dog"; coco_ids[17] = "horse"; coco_ids[18] = "sheep"; coco_ids[19] = "cow"; 
        coco_ids[20] = "elephant"; coco_ids[21] = "bear"; coco_ids[22] = "zebra"; coco_ids[23] = "giraffe"; coco_ids[24] = "backpack";
        coco_ids[25] = "umbrella"; coco_ids[26] = "handbag"; coco_ids[27] = "tie"; coco_ids[28] = "suitcase"; coco_ids[29] = "frisbee"; 
        coco_ids[30] = "skis"; coco_ids[31] = "snowboard"; coco_ids[32] = "sports ball"; coco_ids[33] = "kite"; coco_ids[34] = "baseball bat";
        coco_ids[35] = "baseball glove"; coco_ids[36] = "skateboard"; coco_ids[37] = "surfboard"; coco_ids[38] = "tennis racket"; coco_ids[39] = "bottle"; 
        coco_ids[40] = "wine glass"; coco_ids[41] = "cup"; coco_ids[42] = "fork"; coco_ids[43] = "knife"; coco_ids[44] = "spoon"; 
        coco_ids[45] = "bowl"; coco_ids[46] = "banana"; coco_ids[47] = "apple"; coco_ids[48] = "sandwich"; coco_ids[49] = "orange";
        coco_ids[50] = "broccoli"; coco_ids[51] = "carrot"; coco_ids[52] = "hot dog"; coco_ids[53] = "pizza"; coco_ids[54] = "donut"; 
        coco_ids[55] = "cake"; coco_ids[56] = "chair"; coco_ids[57] = "couch"; coco_ids[58] = "potted plant"; coco_ids[59] = "bed"; 
        coco_ids[60] = "dining table"; coco_ids[61] = "toilet"; coco_ids[62] = "tv"; coco_ids[63] = "laptop"; coco_ids[64] = "mouse"; 
        coco_ids[65] = "remote"; coco_ids[66] = "keyboard"; coco_ids[67] = "cell phone"; coco_ids[68] = "microwave"; coco_ids[69] = "oven";
        coco_ids[70] = "toaster"; coco_ids[71] = "sink"; coco_ids[72] = "refrigerator"; coco_ids[73] = "book"; coco_ids[74] = "clock"; 
        coco_ids[75] = "vase"; coco_ids[76] = "scissors"; coco_ids[77] = "teddy bear"; coco_ids[78] = "hair drier"; coco_ids[79] = "toothbrush";
    }

    if (id < 0 || id > 90) return coco_ids[0];
    else return coco_ids[id];
}

cv::Scalar get_color_by_id(int id) {
    const int num_classes = 80;
    static int color_tables[num_classes][3];
    static bool is_valid = false;

    if (is_valid == false) {
        srand(0);
        for (int k = 0; k < num_classes; k++) {
            color_tables[k][0] = rand() % 256;
            color_tables[k][1] = rand() % 256;
            color_tables[k][2] = rand() % 256;
        }
        is_valid = true;
    }

    if (id < 0 || id >= num_classes) return cv::Scalar(255, 255, 255);
    else return cv::Scalar(color_tables[id][0], color_tables[id][1], color_tables[id][2]);
}

void draw_outputs(cv::Mat& image, float* outputs, int num_detections)
{
    float score_thresh = 0.35;
    for (int k = 0; k < num_detections; k++) {
        float* det = &outputs[0] + k * 7;
        if (det[2] < score_thresh) {
            continue;
        }
        int xt = int(det[0]);
        int yt = int(det[1]);
        int w = int(det[2] - det[0]);
        int h = int(det[3] - det[1]);

        cv::Rect rect(xt, yt, w, h);
        cv::Scalar color = get_color_by_id(int(det[5]));
        int thick = int(0.7 * (image.rows + image.cols) / 700.F);
        cv::rectangle(image, rect, color, thick);

        std::string id_s = get_name_by_id(int(det[5]));
        char buffer[20] = {0};
        sprintf(buffer, "%s %.2f", id_s.data(), det[4]);
        std::string text(buffer);
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.4;
        int baseline;
        cv::Size size = cv::getTextSize(text, fontFace, fontScale, thick / 2, &baseline);
        cv::Rect rect_t(xt, yt - (size.height + 8), size.width + 4, size.height + 8);
        cv::rectangle(image, rect_t, color, -1);
        cv::putText(image, text, cv::Point(xt, yt - 5), fontFace, fontScale, cv::Scalar(255, 255, 255), thick / 2);
    }
}

void video_detect(std::string engine, int cam_id)
{
    cv::VideoCapture capture(cam_id);
	capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    capture.set(cv::CAP_PROP_FPS, 30);

    yolodet::yoloNet net(engine);
    std::unique_ptr<float[]> outputData(new float[net.mBindBufferSizes[1]]);
    cv::Mat frame;
    while(true) {
        auto start_time = std::chrono::system_clock::now();
        capture >> frame;
        if (frame.empty()) break;

        net.infer(frame, outputData.get());
        int num_detections = static_cast<int>(outputData[0]);
        float *outputs = outputData.get() + 1;
        draw_outputs(frame, outputs, num_detections);
        auto stop_time = std::chrono::system_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time);
        float duration_sec = float(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
        float FPS = 1.F / duration_sec;

        char buf[20] = {0};
        sprintf(buf, "FPS: %d", int(FPS));
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.5;
        cv::putText(frame, std::string(buf), cv::Point(20, 30), fontFace, fontScale, cv::Scalar(255, 255, 255), 2);
		
        cv::imshow("video", frame);
        if (char(cv::waitKey(1)) == 'q') break;
    }
}

int main(int argc, char** argv)
{
	int opt = 0,option_index = 0;
    static struct option opts[]={{"tensorrt-engine",required_argument, nullptr,'t'},
                                 {"camera-index",required_argument,nullptr,'c'},
                                 {0,0,0,0}};
	std::string engine = "model/yolov4.engine";
    std::string camera_index = "1";
    while((opt = getopt_long_only(argc,argv,"t:c:",opts,&option_index))!= -1)
    {
        switch (opt){
            case 't': engine = std::string(optarg);
                break;
            case 'c': camera_index = std::string(optarg);
                break;
            default:
                break;
        }
    }
    std::cout<<"tensorrt-engine: "<< engine << std::endl
             <<"camera-index: "<< camera_index << std::endl

    int cam_id = atoi(camera_index.data());
    video_detect(engine, cam_id);

    return 0;
}
