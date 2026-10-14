#include <chrono>
#include <csignal>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

typedef chrono::high_resolution_clock clock_;
typedef chrono::duration<double, ratio<1>> second_;

volatile sig_atomic_t stopRequested = 0;

void requestStop(int) {

    stopRequested = 1;
}

bool getOptions(int argc, char *argv[], int& rows, int& cols, double& squareSize, double& tol,
                string& filename);
void printUsage(const char *execName);
bool readFrame(VideoCapture& cap, Mat& frame);

vector<Point3f> computeObjPoints(Size patternSize, double squareSize);
bool addCornerPoints(const Mat& image, int rows, int cols, double squareSize,
                     vector<vector<Point2f>>& imagePoints,
                     vector<vector<Point3f>>& objectPoints);

int main(int argc, char *argv[]) {

    signal(SIGINT, requestStop);
    signal(SIGTERM, requestStop);

    int rows, cols;
    double squareSize, tol;
    string filename;

    tol = 1.5; // Допустимая ошибка перепроецирования образца по умолчанию

    if (!getOptions(argc, argv, rows, cols, squareSize, tol, filename)) {

        printUsage(argv[0]);
        return -1;
    }

    if (filename.size() < 4 || filename.compare(filename.size() - 4, 4, ".xml") != 0)
        filename += string(".xml");

    cout << "Calibrating with a " << cols << "x" << rows
         << " chessboard with a square size of " << squareSize << " meters" << endl
         << "Sample reprojection error tolerance is set to " << tol << " pixels" << endl
         << "Camera settings will be saved to \"" << filename << "\"" << endl
         << "Press S, Enter or Esc in the camera window to calibrate and save." << endl
         << "The program will also save automatically after 20 accepted samples." << endl;

    // Инициализируем устройство видеопотока.
    VideoCapture cap(0);

    if (!cap.isOpened()) {

        cerr << "Could not open camera 0" << endl;
        return -1;
    }

    // Создаём окно.
    string window = "Camera Calibration";
    namedWindow(window, WINDOW_AUTOSIZE);

    Mat frame, patternFrame;
    vector<vector<Point2f>> imagePoints;
    vector<vector<Point3f>> objectPoints;

    // Создаём изображение для показа.
    if (!readFrame(cap, frame)) {

        cerr << "Camera opened but did not provide a valid frame" << endl;
        return -1;
    }

    Mat showFrame(frame.rows, 2*frame.cols, frame.type());

    const int targetSamples = 20;
    int i = 0;
    chrono::time_point<clock_> t0 = clock_::now();

    while (true) {

        if (!readFrame(cap, frame)) {

            cerr << endl << "Could not read a valid camera frame; finishing capture" << endl;
            break;
        }

        if (showFrame.rows != frame.rows || showFrame.cols != 2 * frame.cols ||
            showFrame.type() != frame.type())
            showFrame = Mat(frame.rows, 2 * frame.cols, frame.type());

        frame.copyTo(showFrame(Rect(0, 0, frame.cols, frame.rows)));

        double elapsed = chrono::duration_cast<second_>(clock_::now() - t0).count();

        // Оставляем несколько секунд между захватами шаблона.
        if (elapsed > 2) {

            frame.copyTo(patternFrame);

            if (addCornerPoints(patternFrame, rows, cols, squareSize, imagePoints, objectPoints)) {

                patternFrame.copyTo(showFrame(Rect(frame.cols-1, 0, frame.cols, frame.rows)));
                cout << "\r" << ++i << " samples captured" << flush;
                t0 = clock_::now();

                if (i >= targetSamples) {

                    cout << endl << "Target sample count reached; calibrating and saving..." << endl;
                    break;
                }
            }
        }

        imshow(window, showFrame);

        int key = waitKey(1) & 0xff;

        if (key == 27 || key == 10 || key == 13 || key == 's' || key == 'S') {

            cout << endl << "Finishing capture; calibrating and saving..." << endl;
            break;
        }

        if (getWindowProperty(window, WND_PROP_VISIBLE) < 1) {

            cout << endl << "Camera window closed; calibrating and saving..." << endl;
            break;
        }

        if (stopRequested) {

            cout << endl << "Stop requested; calibrating and saving..." << endl;
            break;
        }
    }

    cap.release();
    destroyWindow(window);

    if (i == 0) {

        cout << "Calibration canceled: no samples to work with" << endl;
        return -1;
    }

    cout << endl;

    Size imageSize = frame.size();
    Mat cameraMat, distCoeffs, stdDevIntrinsics, stdDevExtrinsics, perViewErrors;
    vector<Mat> rvecs, tvecs;

    double rmse = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMat, distCoeffs, rvecs, tvecs,
                                  stdDevIntrinsics, stdDevExtrinsics, perViewErrors);

    vector<vector<Point2f>> newImagePoints;
    vector<vector<Point3f>> newObjectPoints;

    // Удаляем образцы с высокой ошибкой перепроецирования.
    for (int i = 0; i < perViewErrors.rows; i++) {

        if (perViewErrors.at<double>(i, 0) < tol) {

            newImagePoints.push_back(imagePoints[i]);
            newObjectPoints.push_back(objectPoints[i]);
        }
    }

    if (newImagePoints.empty()) {

        cout << "Warning: every sample exceeds the reprojection-error tolerance; "
             << "saving the calibration computed from all samples" << endl;
    } else if (newImagePoints.size() < imagePoints.size()) {

        // Повторно калибруем камеру только по хорошим образцам.
        rmse = calibrateCamera(newObjectPoints, newImagePoints, imageSize, cameraMat, distCoeffs, rvecs, tvecs,
                               stdDevIntrinsics, stdDevExtrinsics, perViewErrors);

        cout << "Camera calibrated using only " << newImagePoints.size() << " samples" << endl;
    }

    cout << "Intrinsic matrix:" << endl
         << cameraMat << endl
         << "Distortion coeficients: " << distCoeffs << endl
         << "RMS reprojection error (pixels): " << rmse << endl;

    FileStorage fs(filename, FileStorage::WRITE);

    if (!fs.isOpened()) {

        cout << "Could not open \"" << filename << "\": camera settings not saved" << endl;
        return -1;
    }

    fs << "Image_Size" << imageSize;
    fs << "Intrinsic_Matrix" << cameraMat;
    fs << "Distortion_Coefficients" << distCoeffs;

    fs.release();

    ifstream savedFile(filename, ios::binary);

    if (!savedFile.good()) {

        cout << "Camera settings could not be verified at \"" << filename << "\"" << endl;
        return -1;
    }

    cout << "Camera settings saved to \"" << filename << "\"" << endl;
}

bool getOptions(int argc, char *argv[], int& rows, int& cols, double& squareSize, double& tol,
                string& filename) {

    // Ищем параметр справки.
    for (int i = 1; i < argc; i++)
        if (string(argv[i]) == "-h" || string(argv[i]) == "--help")
            return false;

    // Проверяем количество входных параметров.
    if (argc != 5 && argc != 6)
        return false;

    rows = (int) strtol(argv[1], NULL, 10);
    if (rows == 0 || errno == ERANGE) return false;

    cols = (int) strtol(argv[2], NULL, 10);
    if (cols == 0 || errno == ERANGE) return false;

    squareSize = strtod(argv[3], NULL);
    if (squareSize == 0 || errno == ERANGE) return false;

    if (argc == 6) {

        tol = strtod(argv[4], NULL);
        if (tol == 0 || errno == ERANGE) return false;
    }

    filename = (argc == 6) ? argv[5] : argv[4];

    return true;
}

void printUsage(const char *execName) {

    cerr << "Usage: " << execName << " rows cols square_size [tol] filename (-h, --help for help)" << endl;
}

vector<Point3f> computeObjPoints(Size patternSize, double squareSize) {

    vector<Point3f> objPoints;

    for (int i = 0; i < patternSize.height; i++) {

        for (int j = 0; j < patternSize.width; j++) {

            objPoints.push_back(Point3f(squareSize * j, squareSize * i, 0));
        }
    }

    return objPoints;
}

bool addCornerPoints(const Mat& image, int rows, int cols, double squareSize,
                     vector<vector<Point2f>>& imagePoints,
                     vector<vector<Point3f>>& objectPoints) {

    if (image.empty())
        return false;

    int pointsPerRow = cols - 1;
    int pointsPerCol = rows - 1;

    Size patternSize(pointsPerRow, pointsPerCol);

    vector<Point2f> imgPoints;

    bool patternFound = findChessboardCorners(image, patternSize, imgPoints,
                        CALIB_CB_ADAPTIVE_THRESH +
                        CALIB_CB_NORMALIZE_IMAGE +
                        CALIB_CB_FAST_CHECK);

    if (patternFound) {

        Mat gray;

        cvtColor(image, gray, COLOR_BGR2GRAY);

        cornerSubPix(gray, imgPoints, Size(11, 11), Size(-1, -1),
                     TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 30, 0.1));

        imagePoints.push_back(imgPoints);
        objectPoints.push_back(computeObjPoints(patternSize, squareSize));

        drawChessboardCorners(image, patternSize, imgPoints, patternFound);
    }

    return patternFound;
}

bool readFrame(VideoCapture& cap, Mat& frame) {

    const int maxAttempts = 30;

    for (int attempt = 0; attempt < maxAttempts; attempt++) {

        if (cap.read(frame) && !frame.empty())
            return true;

        frame.release();
        this_thread::sleep_for(chrono::milliseconds(20));
    }

    return false;
}
