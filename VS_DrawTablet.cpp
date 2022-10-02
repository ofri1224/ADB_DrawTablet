#include <iostream>
#include <Windows.h>
#include <stdio.h>

#define EV_ABS                  "0003"
#define EV_KEY                  "0001"
#define EV_SYN                  "0000"
#define ABS_MT_TRACKING_ID      "0039"
#define ABS_MT_PRESSURE         "003a"
#define ABS_MT_TOUCH_MAJOR      "0030"
#define ABS_MT_POSITION_X       "0035"
#define ABS_MT_POSITION_Y       "0036"
#define BTN_TOUCH               "014a"
#define DOWN                    "00000001"
#define UP                      "00000000"
#define SYN_REPORT              "0000"

int main()
{
    const HSYNTHETICPOINTERDEVICE pointDevice = CreateSyntheticPointerDevice(PT_PEN, 1, POINTER_FEEDBACK_INDIRECT);


    if (pointDevice == nullptr) {
        const DWORD err = GetLastError();

        std::cout << "Pointer: Error code " << err << std::endl;
        return err;
    }
    int _height, _width,height,width;

    LPRECT screenSize = (LPRECT)malloc(sizeof(LPRECT));
    
    if (!GetClientRect(GetDesktopWindow(), screenSize)) {
        const DWORD err = GetLastError();

        std::cout << "screen Size: Error code " << err << std::endl;
        return err;
    }
    height = screenSize->bottom;
    width = screenSize->right;

    FILE* fp;
    char* evMajor, * evMinor;

    int evValue;

    char* next_token;

    fp = _popen("adb shell wm size", "r");
    
    char cBuff[54];
    fgets(cBuff, 54, fp);
    
    _height = (int)strtol(strtok_s(cBuff+15, "x", &next_token), NULL, 10);
    _width = (int)strtol(strtok_s(NULL, "x", &next_token), NULL, 10);

    std::cout << height << std::endl;
    std::cout << width << std::endl;
    std::cout << _height << std::endl;
    std::cout << _width << std::endl;

    fp = _popen("adb shell getevent /dev/input/event3", "r");

    POINTER_TYPE_INFO inputInfo[1];

    inputInfo[0].type = PT_PEN;
    inputInfo[0].penInfo.pointerInfo.pointerType = PT_PEN;

    inputInfo[0].penInfo.pointerInfo.frameId = 0;

    inputInfo[0].penInfo.penMask = PEN_MASK_PRESSURE;

    inputInfo[0].penInfo.pointerInfo.dwTime = 0;
    inputInfo[0].penInfo.pointerInfo.PerformanceCount = 0;




    while (true)
    {
        fgets(cBuff, 54, fp);

        //printf(cBuff);

        evMajor = strtok_s(cBuff, " ", &next_token);
        evMinor = strtok_s(NULL, " ", &next_token);
        evValue = (int)strtol(strtok_s(NULL, " ", &next_token), NULL, 16);

        if (strcmp(evMajor, EV_ABS) == 0) {
            if (strcmp(evMinor, ABS_MT_POSITION_X) == 0)
            {
                //std::cout << "VAL:"<< evValue << std::endl;
                //std::cout << "H1:" << _height << std::endl;
                //std::cout << "H2:" << height << std::endl;
                //std::cout << "ANS:"<< (evValue * height) / _height << std::endl;
                inputInfo[0].penInfo.pointerInfo.ptPixelLocation.y = (evValue*height)/_height;
            }
            if (strcmp(evMinor, ABS_MT_POSITION_Y) == 0)
            {
                //std::cout << "VAL:" << evValue << std::endl;
                //std::cout << "W1:" << _width << std::endl;
                //std::cout << "W2:" << width << std::endl;
                //std::cout << "ANS:" << (evValue * width) / _width << std::endl;
                inputInfo[0].penInfo.pointerInfo.ptPixelLocation.x = (evValue * width) / _width;
            }
            if (strcmp(evMinor, ABS_MT_PRESSURE) == 0)
            {
                inputInfo[0].penInfo.pressure = evValue;
            }
            if (strcmp(evMinor, ABS_MT_TOUCH_MAJOR) == 0)
            {
                //inputInfo[0].penInfo.pointerInfo.
            }
            if (strcmp(evMinor, ABS_MT_TRACKING_ID) == 0)
            {
                inputInfo[0].penInfo.pointerInfo.pointerId = evValue;
            }
        }
        if (strcmp(evMajor, EV_KEY) == 0) {
            if (strcmp(evMinor, DOWN) == 0) {
                inputInfo[0].penInfo.pointerInfo.pointerFlags = POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT | POINTER_FLAG_DOWN;
            }
            if (strcmp(evMinor, UP) == 0) {
                inputInfo[0].penInfo.pointerInfo.pointerFlags = POINTER_FLAG_INRANGE;
            }
        }
        if (strcmp(evMajor, EV_SYN) == 0) {
            if (!InjectSyntheticPointerInput(pointDevice, inputInfo, 1))
            {
                const DWORD err = GetLastError();

                std::cout << "Input: Error code " << err << std::endl;
                return err;
            }
        }
    }
    printf("test3\r\n");


    // Destroy pointer
    free(cBuff);
    DestroySyntheticPointerDevice(pointDevice);
    _pclose(fp);
}




// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
