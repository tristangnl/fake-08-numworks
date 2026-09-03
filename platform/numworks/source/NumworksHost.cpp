#include "eadkpp.h"

using namespace std;

#include "../../../source/host.h"
#include "../../../source/hostVmShared.h"
#include "../../../source/nibblehelpers.h"
//#include "../../../source/logger.h"

extern const char eadk_app_name[]
#if PLATFORM_DEVICE
    __attribute__((section(".rodata.eadk_app_name")))
#endif
    = "FAKEO8";

const uint32_t eadk_api_level
#if PLATFORM_DEVICE
    __attribute__((section(".rodata.eadk_api_level")))
#endif
    = 0;

// Required by the exception stack unwinder.
// Unused on this platform.
void *__exidx_start;
void *__exidx_end;


static bool done = false;

static constexpr int PicoScreenSideWidth = 128;
static int picoLineBufferSize = 320;

EADK::Color _mapped16BitColors[144];

uint32_t last_time;
uint32_t now_time;
uint32_t frame_time;
uint32_t targetFrameTimeMs;

//for p8 cartridge
const char *osd_getromdata(const char *name) {
  return (char*)eadk_external_data;
}

void setRenderParamsFromStretch(StretchOption stretch) {
    if (stretch == PixelPerfect) {
        picoLineBufferSize = 128;
    }
    else if (stretch == StretchToFill) {
        picoLineBufferSize = 240;
    }
    else if(stretch == PixelPerfectStretch){
        picoLineBufferSize = 256;
    }
    else {
        picoLineBufferSize = 320;
    }
}

Host::Host(int windowWidth, int windowHeight)  {
    EADK::Display::pushRectUniform(EADK::Screen::Rect, Black);
}


void Host::oneTimeSetup(Audio* audio){
    stretch = StretchToFit;

    last_time = 0;
    now_time = 0;
    frame_time = 0;
    targetFrameTimeMs = 0;

    for (int i = 0; i < 144; i++) {
        _mapped16BitColors[i] = EADK::Color(
            (_paletteColors[i].Red << 16) |
            (_paletteColors[i].Green << 8) |
            _paletteColors[i].Blue
        );
    }
}

void Host::oneTimeCleanup(){

}

void Host::setTargetFps(int targetFps){
    targetFrameTimeMs = 1000.0 / targetFps;
}

void Host::forceStretch(StretchOption newStretch) {
	EADK::Display::pushRectUniform(EADK::Screen::Rect, Black);
    stretch = newStretch;
}

void Host::changeStretch(){
    if (stretchKeyPressed && resizekey == YesResize) {
        if (stretch == StretchToFit) {
            stretch = StretchToFill;
        }
        else if (stretch == StretchToFill) {
            stretch = PixelPerfect;
        }
        else if (stretch == PixelPerfect) {
            stretch = PixelPerfectStretch;
        }
        else{
            stretch = StretchToFit;
        }
        EADK::Display::pushRectUniform(EADK::Screen::Rect, Black);
    }
    stretchKeyPressed = false;
}

bool Host::shouldFillAudioBuff(){
    return false;
}

void* Host::getAudioBufferPointer(){
    return NULL;
}

size_t Host::getAudioBufferSize(){
    return 0;
}

void Host::playFilledAudioBuffer(){
}

bool Host::shouldRunMainLoop(){
    return true;
}

InputState_t Host::scanInput(){
    static EADK::Keyboard::State previousKeyboardState;

    const EADK::Keyboard::State currentKeyboardState = EADK::Keyboard::scan();

    currKDown = 0;
    currKHeld = 0;

    auto processKey = [&](EADK::Keyboard::Key key, uint8_t p8Key) {
        if (currentKeyboardState.keyDown(key)) {
            currKHeld |= p8Key;
            if (!previousKeyboardState.keyDown(key)) {
                currKDown |= p8Key;
            }
        }
    };

    processKey(EADK::Keyboard::Key::Left,  P8_KEY_LEFT);
    processKey(EADK::Keyboard::Key::Right, P8_KEY_RIGHT);
    processKey(EADK::Keyboard::Key::Up,    P8_KEY_UP);
    processKey(EADK::Keyboard::Key::Down,  P8_KEY_DOWN);
    processKey(EADK::Keyboard::Key::Back,  P8_KEY_O);
    processKey(EADK::Keyboard::Key::OK,    P8_KEY_X);
    processKey(EADK::Keyboard::Key::EXE,   P8_KEY_PAUSE);

    if (currentKeyboardState.keyDown(EADK::Keyboard::Key::Home)) {
        done = true;
    }
    if (currentKeyboardState.keyDown(EADK::Keyboard::Key::Shift) && !previousKeyboardState.keyDown(EADK::Keyboard::Key::Shift)) {
        stretchKeyPressed = true;
    }

    previousKeyboardState = currentKeyboardState;

    return InputState_t{
        currKDown,
        currKHeld
    };
}

bool Host::shouldQuit() {
    return done;
}

void Host::waitForTargetFps(){
    now_time = static_cast<uint32_t>(EADK::Timing::millis());
    frame_time = now_time - last_time;
	last_time = now_time;

	//sleep for remainder of time
	if (frame_time < targetFrameTimeMs) {
		uint32_t msToSleep = targetFrameTimeMs - frame_time;
        
        EADK::Timing::msleep(msToSleep);

		last_time += msToSleep;
	}
}

void Host::drawFrame(uint8_t* picoFb, uint8_t* screenPaletteMap, uint8_t drawMode){
    EADK::Color pico_line_buffer[picoLineBufferSize];
    
    if(stretch==PixelPerfect){
        // Native resolution (128x128)
        #pragma unroll 64
        for (int y = 0; y < PicoScreenSideWidth; y++) {
            for (int x = 0; x < PicoScreenSideWidth; x++) {
                pico_line_buffer[x] = _mapped16BitColors[screenPaletteMap[getPixelNibble(x, y, picoFb)] & 0x8f];
            }
            EADK::Display::pushRect(EADK::Rect((EADK_SCREEN_WIDTH-PicoScreenSideWidth)/2, (EADK_SCREEN_HEIGHT-PicoScreenSideWidth)/2+y, PicoScreenSideWidth, 1), pico_line_buffer);
        }
    }
    else if (stretch==PixelPerfectStretch){
    //Native resolution x2 (256x256), cropped to 256x240 (8 px border on the top and 8 px on the bottom)
    #pragma unroll 64
        for (int y = 4; y < PicoScreenSideWidth-4; y++) {
            for (int x = 0; x < PicoScreenSideWidth; x++) {
                EADK::Color color = _mapped16BitColors[screenPaletteMap[getPixelNibble(x, y, picoFb)] & 0x8f];
                pico_line_buffer[2*x] = color;
                pico_line_buffer[2*x+1] = color;
            }
            EADK::Display::pushRect(EADK::Rect((EADK_SCREEN_WIDTH-256)/2, (EADK_SCREEN_HEIGHT-240)/2+(y-4)*2, 256, 1), pico_line_buffer);
            EADK::Display::pushRect(EADK::Rect((EADK_SCREEN_WIDTH-256)/2, (EADK_SCREEN_HEIGHT-240)/2+(y-4)*2+1, 256, 1), pico_line_buffer);
        }
    }
    else if (stretch==StretchToFit){
        // Nearest neighbor scaling of a 128x128 texture to a 240x240 resolution (to keep the ratio)
        // Horizontally, we multiply by 1.875 (128*1.875 = 240)
        #pragma unroll 64
        for (int y = 0; y < PicoScreenSideWidth; y++) {
            for (int x = 0; x < PicoScreenSideWidth; x++) {
                EADK::Color color = _mapped16BitColors[screenPaletteMap[getPixelNibble(x, y, picoFb)] & 0x8f];
                // We can't use floats for performance reason, so we use a fixed point
                // representation
                pico_line_buffer[1875*x/1000] = color;
                // This line is useless 1/3 times, but using an if is slower
                pico_line_buffer[1875*x/1000+1] = color;
            }

            // Vertically, we want to scale by a 15/8 ratio. So we need to make 15 lines out of 8:  we double 7 lines out of 8.
            uint16_t yOffset = (15*y)/8;
            EADK::Display::pushRect(EADK::Rect((EADK_SCREEN_WIDTH-240)/2, yOffset, 240, 1), pico_line_buffer);
            if (y%8 != 0) {
                EADK::Display::pushRect(EADK::Rect((EADK_SCREEN_WIDTH-240)/2, yOffset + 1, 240, 1), pico_line_buffer);
            }
        }
    }
    else{
        // Nearest neighbor scaling of a 128x128 texture to a 320x240 resolution
        // Horizontally, we want to scale by a 5/2 ratio. So we need to color 5 pixels out of 2:  we triple 1 pixel and we double 1 pixel out of 2 pixels.
        #pragma unroll 64
        for (int y=0; y<PicoScreenSideWidth; y++) {
            for (int x = 0; x < PicoScreenSideWidth; x++) {
                EADK::Color color = _mapped16BitColors[screenPaletteMap[getPixelNibble(x, y, picoFb)] & 0x8f];
                pico_line_buffer[25*x/10] = color;
                pico_line_buffer[25*x/10+1] = color;
                // This line is useless 1/2 time, but using an if is slower
                pico_line_buffer[25*x/10+2] = color;
            }
            // Vertically, we want to scale by a 15/8 ratio. So we need to make 15 lines out of 8:  we double 7 lines out of 8.
            uint16_t yOffset = (15*y)/8;
            EADK::Display::pushRect(EADK::Rect(0, yOffset, 320, 1), pico_line_buffer);
            if (y%8 != 0) {
                EADK::Display::pushRect(EADK::Rect(0, yOffset + 1, 320, 1), pico_line_buffer);
            }
        }
    }
}

vector<string> Host::listcarts(){
    vector<string> carts;
    // carts.push_back(eadk_external_data);
    return carts;
}

const char* Host::logFilePrefix() {
    return "";
}

std::string Host::customBiosLua() {
    return "cartpath = \"none\"\n"
        "selectbtn = \"OK\"\n"
        "pausebtn = \"backspace\"\n"
        "exitbtn = \"Home\"\n"
        "sizebtn = \"EXE to cycle screen sizes\"\n";
}

std::string Host::getCartDirectory() {
    return "";
}

std::vector<std::string> Host::listdirs() {    
    return {};
}