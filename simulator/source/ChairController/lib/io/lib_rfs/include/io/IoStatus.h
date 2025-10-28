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

#pragma once

namespace Io {

/** The supported status codes for operations. */
struct IoStatus {
		typedef enum {
// @formatter:off
			/** Everything went fine. */
			OK,
			/** End of Stream: No more data to read. */
			EOS,
			/** An error occurred. */
			ERROR,
// @formatter:on
		} Enum;
};

} // namespace Io
