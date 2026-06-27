#include <eadk.h>

#include "../../../source/host.h"
#include "../../../source/hostVmShared.h"
#include "../../../source/nibblehelpers.h"

extern const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "FakeO8";
extern const uint32_t eadk_api_level __attribute__((section(".rodata.eadk_api_level"))) = 0;

int main(int argc, char * argv[]) {

}