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


bool done = false;

static constexpr int PicoScreenWidth  = 128;
static constexpr int PicoScreenHeight = 128;
 
static constexpr int ScreenOffsetX = 96;
static constexpr int ScreenOffsetY = 56;

static EADK::Color pico_pixel_buffer[PicoScreenWidth * PicoScreenHeight];


EADK::Color _mapped16BitColors[144];

uint32_t last_time;
uint32_t now_time;
uint32_t frame_time;
uint32_t targetFrameTimeMs;

//for p8 cartridge
const char *osd_getromdata(const char *name) {
  return (char*)eadk_external_data;
}

Host::Host(int windowWidth, int windowHeight)  {
    EADK::Display::pushRectUniform(EADK::Screen::Rect, Black);
}


void Host::oneTimeSetup(Audio* audio){
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
	
}

void Host::changeStretch(){
    
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
    stretchKeyPressed = false;

    auto processKey = [&](EADK::Keyboard::Key key, uint8_t p8Key) {
        const bool current = currentKeyboardState.keyDown(key);
        const bool previous = previousKeyboardState.keyDown(key);

        if (current) {
            currKHeld |= p8Key;
            if (!previous) {
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
    //minimum implementation
    for (int y = 0; y < PicoScreenHeight; y++) {
        for (int x = 0; x < PicoScreenWidth; x++) {
            uint8_t paletteIndex = getPixelNibble(x, y, picoFb);
            EADK::Color color = _mapped16BitColors[screenPaletteMap[paletteIndex] & 0x8f];
            pico_pixel_buffer[y * PicoScreenWidth + x] = color;
        }
    }
    EADK::Display::pushRect(
        EADK::Rect(ScreenOffsetX, ScreenOffsetY, PicoScreenWidth, PicoScreenHeight),
        pico_pixel_buffer
    );
}

vector<string> Host::listcarts(){
    vector<string> carts;
    
    return carts;
}

const char* Host::logFilePrefix() {
    return "";
}

std::string Host::customBiosLua() {
    return "";
}

std::string Host::getCartDirectory() {
    return "";
}

std::vector<std::string> Host::listdirs() {
    std::vector<std::string> dirs;

    
    return dirs;
}