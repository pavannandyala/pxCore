/*

 pxCore Copyright 2005-2018 John Robinson

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#ifndef _RT_URL_UTILS
#define _RT_URL_UTILS

#include "rtString.h"

// Apply url encoding to the provided string.
rtString rtUrlEscape(const char* s);

// JRJR todo refactor urlencodeparameters to use urlescape
rtString rtUrlEncodeParameters(const char* url);
rtString rtUrlGetOrigin(const char* url);
rtString rtUrlGetHostname(const char* origin);

#endif
