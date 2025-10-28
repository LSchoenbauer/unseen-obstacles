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

#ifndef SW_VERSION
#define SW_VERSION undefined
#endif

#include <devices/boards/Cfg_Board_Common.h>
#include <rfs/RfsStandardConfig.h>
#include <net/Cfg_Wifi_Std.h>
#include <net/Cfg_Sntp_Std.h>

struct StdAppConfig {
		const char* vLeoIot = "SW_VERSION";
		const BaseBoardCfg device;
		const StdWifiConfig wifi;
		const StdSntpConfig sntp;
		const StdRfsConfig fs;
};
