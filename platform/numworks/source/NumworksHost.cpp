#include "eadkpp.h"

using namespace std;

#include "../../../source/host.h"
#include "../../../source/hostVmShared.h"
#include "../../../source/nibblehelpers.h"

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

float targetFrameTimeMs;

// Required by the exception stack unwinder.
// Unused on this platform.
void *__exidx_start;
void *__exidx_end;

//for p8 cartridge
const char *osd_getromdata(const char *name) {
  return (char*)eadk_external_data;
}

Host::Host(int windowWidth, int windowHeight)  {

 }


void Host::oneTimeSetup(Audio* audio){

}

void Host::oneTimeCleanup(){

}

void Host::setTargetFps(int targetFps){
    targetFrameTimeMs = 1000.0 / (float)targetFps;
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
    
    return InputState_t {

    };
}

bool Host::shouldQuit() {
    
    return false;
}

void Host::waitForTargetFps(){
    
}

void Host::drawFrame(uint8_t* picoFb, uint8_t* screenPaletteMap, uint8_t drawMode){

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