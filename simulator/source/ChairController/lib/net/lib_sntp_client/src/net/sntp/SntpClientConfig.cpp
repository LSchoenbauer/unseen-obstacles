/*
 * ===============================================================================
 * c) HTL Leonding
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

#include <net/sntp/SntpClientConfig.h>

namespace Net {
namespace Sntp {

	const char* SntpClientConfig::mDefaultServerName   = "pool.ntp.org";
	uint16_t SntpClientConfig::mDefaultUpdateIntervalS = 60 * 60; // 1 hour

	SntpClientConfig::SntpClientConfig() : 
		mLocationValid(true),
		mTzInfoValid(true),
		mUpdateInterval(mDefaultUpdateIntervalS),
		mNtpServerName(mDefaultServerName),
		mLocation(0),
		mTzInfo(0)
	{}

	SntpClientConfig::SntpClientConfig(const char* ntpServerName) : 
		mLocationValid(true),
		mTzInfoValid(true),
		mUpdateInterval(mDefaultUpdateIntervalS),
		mNtpServerName(ntpServerName),
		mLocation(0),
		mTzInfo(0)
	{}

	SntpClientConfig::SntpClientConfig(String ntpServerName) : 
		mLocationValid(true),
		mTzInfoValid(true),
		mUpdateInterval(mDefaultUpdateIntervalS),
		mNtpServerName(ntpServerName),
		mLocation(0),
		mTzInfo(0)
	{}

} // namespace Sntp
} // namespace Net
