// Copyright Benoit Blanchon 2014-2016
// MIT License
//
// Arduino JSON library
// https://github.com/bblanchon/ArduinoJson
// If you like this project, please add a star!

#pragma once

#ifndef ARDUINO

#include <string>
typedef std::string String;

#else

#if PLATFORM_ID == 88

#include "Arduino.h"

#else

#include <WString.h>

#endif


#endif
