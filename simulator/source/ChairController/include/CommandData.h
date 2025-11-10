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

#include <stdint.h>


class CommandData {
	public:
        enum class Command {
            UP,
            DOWN,
            TO_TOP,
            TO_CENTER,
            TO_BOTTOM
        };

        enum class Mover {
            REAR_LEFT,
            REAR_RIGHT,
            FRONT,
            ROTATION
        };

		CommandData(
            Command command,
            Mover mover
        ) : mCommand(command), mMover(mover) {}

		~CommandData(){}

        Command GetCommand() const {return mCommand;}
        Mover GetMover() const {return mMover;}

    private:
        Command mCommand;
        Mover mMover;
};