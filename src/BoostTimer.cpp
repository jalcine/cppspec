/*
 * Copyright 2008 Timo Puronen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "BoostTimer.h"
#include <iomanip>
#include <sstream>

namespace CppSpec {

BoostTimer::BoostTimer() {}

BoostTimer::~BoostTimer() {
}

void BoostTimer::start() {
    startTime = boost::chrono::system_clock::now();
}

std::string BoostTimer::stop() {
    boost::chrono::duration<double> secs = boost::chrono::system_clock::now() - startTime;
    std::stringstream stream;
    stream << std::fixed << std::setprecision(6) << secs.count();
    return stream.str();
}

}
