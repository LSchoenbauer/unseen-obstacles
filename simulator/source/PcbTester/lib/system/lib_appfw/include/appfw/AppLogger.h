/*
 * ===============================================================================
 * (c) HTL Leonding
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Licensed under MIT License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * All trademarks used in this document are property of their respective owners.
 * ===============================================================================
 */

#pragma once

#include <utils/Log.h>

// App logging

#ifdef LOG_APP_DBG_ENABLED
#define AppLogDbg(format, ...) LogDbg(format, ##__VA_ARGS__)
#define AppEvLogDbg(ev, evType, format, ...) if (ev && ev->IsOf(evType)) { LogDbg(format, ##__VA_ARGS__); }
#define AppOtherEvLogDbg(ev, evType, format, ...) if (ev && !ev->IsOf(evType)) { LogDbg(format, ##__VA_ARGS__); }
#else
#define AppLogDbg(format, ...)
#define AppEvLogDbg(ev, evType, format, ...)
#define AppOtherEvLogDbg(ev, evType, format, ...)
#endif
