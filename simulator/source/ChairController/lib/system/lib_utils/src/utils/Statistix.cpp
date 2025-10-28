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

#include <utils/Statistix.h>
#include <string.h>

Statistix::Statistix(size_t sampleSize) :
        mCurIndex(0), mCount(0), mSum(0), mSampleBufferSize(0), mSampleBuffer(0) {
	mSampleBuffer = new int[sampleSize * sizeof(int)];
	if (0 != mSampleBuffer) {
		memset(mSampleBuffer, 0, sampleSize * sizeof(int));
		mSampleBufferSize = sampleSize;
	}
}

Statistix::~Statistix() {
	if (mSampleBuffer != 0) {
		delete mSampleBuffer;
	}
	mSampleBuffer = 0;
}

void Statistix::CaptureSample(int value) {
	if (mSampleBuffer != 0) {
		mSum -= mSampleBuffer[mCurIndex];
		mSampleBuffer[mCurIndex] = value;
		mSum += mSampleBuffer[mCurIndex];
		mCurIndex++;
		mCurIndex = mCurIndex % mSampleBufferSize;
		if (mCount < mSampleBufferSize) {
			mCount++;
		}
	} else {
		mSum = value;
		mCount = 1;
	}
}

int Statistix::GetSampleCount() const {
	return mCount;
}

int Statistix::GetSum() const {
	return mSum;
}

int Statistix::CalcSum(int value) {
	CaptureSample(value);
	return GetSum();
}

int Statistix::GetAverage() const {
	return mCount != 0 ? mSum / mCount : 0;

}

int Statistix::CalcAverage(int value) {
	CaptureSample(value);
	return GetAverage();
}

