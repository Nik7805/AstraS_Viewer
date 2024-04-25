#include <errno.h>
#include <iostream>
#include <vector>
#include <array>
#include "libobsensor/ObSensor.hpp"
#include "libobsensor/hpp/Error.hpp"
#include "libobsensor/hpp/Pipeline.hpp"
// #include "window.hpp"
#include "udpreceiver.hpp"
#include "opencv4/opencv2/opencv.hpp"
#include "opencv4/opencv2/core/mat.inl.hpp"

#define BUFFSIZE 1500
#define IMAGESIZE 640 * 480
#define IMAGEBYTES IMAGESIZE * 2

#pragma pack(push, 1)
struct VideoFrameHeader_t
{
    uint64_t sync = 0xAA00CC55EE77FF99;
    uint64_t frameID;
    uint16_t width;
    uint16_t height;
    uint16_t depth;
};
#pragma pack(pop)

enum class ReceiverStates
{
    Waiting,
    Receiving,
    Displaying
};

static size_t iAvaiting = 0;
static ReceiverStates state = ReceiverStates::Waiting;
static VideoFrameHeader_t sHeader;
static VideoFrameHeader_t* pHeader;
static std::array<uint8_t, BUFFSIZE> buffer;
static std::vector<uint8_t> vctr;

bool IsHeader(uint8_t* data, size_t iBytes);
void ClearRxBuffer();

static const double gamma_ = 1.5;

int main(int argc, char const *argv[])
{
    std::cout << "Viewer app\r\n";
    cv::Mat lookUpTable(1, 256, CV_8U);
    uint8_t* p = lookUpTable.ptr();
    for( int i = 0; i < 256; ++i)
        p[i] = cv::saturate_cast<uint8_t>(pow(i / 255.0, gamma_) * 255.0);

    try
    {
        UDPReceiver receiver("127.0.0.1", 60000);

        while(1)
        {
            size_t n = receiver.Receive(buffer.data(), buffer.size());

            if(n > 0)
            {
                if(IsHeader(buffer.data(), n))
                {
                    // std::cout << "Received header\n";
                    state = ReceiverStates::Receiving;
                    sHeader = *pHeader;
                    iAvaiting = static_cast<size_t>(sHeader.depth) *
                                static_cast<size_t>(sHeader.height) *
                                static_cast<size_t>(sHeader.width);
                    n = 0;
                    ClearRxBuffer();
                }

                switch (state)
                {
                    case ReceiverStates::Waiting:
                    {
                        break;
                    }
                    case ReceiverStates::Receiving:
                    {
                        if(n == 0)
                            break;
                        
                        std::copy(buffer.begin(), buffer.begin() + n, std::back_inserter(vctr));
                        if(vctr.size() >= iAvaiting)
                        {
                            // std::cout << "Received image\n";
                            cv::Mat mat(sHeader.height, sHeader.width, CV_16UC1, vctr.data());
                            cv::Mat res;
                            cv::normalize(mat, res, 32768.0, 0.0, cv::NORM_INF);
                            // mat.convertTo(res, CV_8U);
                            // cv::LUT(res, lookUpTable, res);
                            cv::rotate(res, res, cv::ROTATE_180);
                            cv::imshow("img", res);
                            cv::waitKey(1);

                            vctr.erase(vctr.begin(), vctr.begin() + iAvaiting);
                            iAvaiting = 0;
                            state = ReceiverStates::Waiting;
                        }
                        break;
                    }
                    
                    default:
                        state = ReceiverStates::Waiting;
                        break;
                }
                
            }
            else if(n == 0)
            {
                std::cout << "Zero\n";
            }
            else
            {
                std::cout << "Receive error: " << strerror(errno) << "\n";
            }
        }
    }
    catch(UDPReceiver::Exceptions ex)
    {
        std::cout << "Exception: " << (int)ex << "\n";
        std::cout << "Error: " << strerror(errno) << "\n";
        exit(1);
    }
    
    
    std::cout << "End of app\r\n";
}

bool IsHeader(uint8_t *data, size_t iBytes)
{
    pHeader = reinterpret_cast<VideoFrameHeader_t*>(data);
    bool bValid = iBytes == sizeof(VideoFrameHeader_t) && (pHeader->sync == sHeader.sync);

    return bValid;
}

void ClearRxBuffer()
{
    vctr.clear();
}
