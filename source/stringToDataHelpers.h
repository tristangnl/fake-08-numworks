#pragma once

#include <string>
#include <cstdint>

void copy_string_to_sprite_memory(uint8_t sprite_data[128 * 64], const char* data);

void copy_mini_label_to_sprite_memory(uint8_t sprite_data[128 * 64], const char* data, int labeloffset);

void copy_string_to_memory(uint8_t* sprite_flag_data, std::string data);