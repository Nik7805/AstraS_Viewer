#include <errno.h>
#include <iostream>
#include <vector>
#include <array>
#include "libobsensor/ObSensor.hpp"
#include "libobsensor/hpp/Error.hpp"
#include "libobsensor/hpp/Pipeline.hpp"
#include "window.hpp"
#include "udpreceiver.hpp"
#include "opencv4/opencv2/opencv.hpp"

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

int main(int argc, char const *argv[])
{
    std::cout << "Viewer app\r\n";

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
                            cv::normalize(mat, mat, 32768.0, 0.0, cv::NORM_INF);
                            cv::rotate(mat, mat, cv::ROTATE_180);
                            cv::imshow("img", mat);
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
