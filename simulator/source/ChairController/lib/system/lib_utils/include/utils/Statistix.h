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

#include <stdlib.h>

class Statistix {
	public:
		/**
		 * Constructs a statistic calculator based on at maximum 'sampleCount' samples.
		 * @param sampleCount The maximum number of sample to capture.
		 */
		Statistix(size_t sampleCount);

		/**
		 * Destructor
		 */
		~Statistix();

		/**
		 * Provides the number of captured samples.
		 * @return The number of captured samples.
		 */
		int GetSampleCount() const;

		/**
		 * Provides the sum over all captured samples.
		 * @return The sum of the last 'GetSampleCount(...)' samples.
		 */
		int GetSum() const;

		/**
		 * Adds the given value and calculates the sum over all captured samples.
		 * @param value The value to add.
		 * @return The sum of the last 'GetSampleCount(...)' samples.
		 */
		int CalcSum(int value);

		/**
		 * Provides  the average over all captured samples.
		 * @return The average of the last 'GetSampleCount(...)' samples.
		 */
		int GetAverage() const;

		/**
		 * Adds the given value and calculates the average over all captured samples.
		 * @param value The value to add.
		 * @return The average of the last 'GetSampleCount(...)' samples.
		 */
		int CalcAverage(int value);

	private:
		/**
		 * Adds the given value and updates all dependent values.
		 * @param value The value to add.
		 */
		void CaptureSample(int value);

		size_t mCurIndex; // index of current sample in buffer
		size_t mCount; // number of values in buffer
		int mSum;      // sum of all sampled values
		size_t mSampleBufferSize;  // sample buffer size
		int* mSampleBuffer;	// sample buffer
};
