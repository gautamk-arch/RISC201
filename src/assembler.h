#pragma once
#include <string>
#include <vector>
#include "isa.h"

std::vector<uint32_t> assemble(const std::string& src,std::vector<std::string>& errors);