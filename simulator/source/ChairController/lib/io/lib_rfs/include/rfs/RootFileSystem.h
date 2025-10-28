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

#include <rfs/FsPartition.h>
#include <utils/Singleton.h>

#include <FS.h>
#include <WString.h>
#include <stddef.h>

using namespace fs;
namespace Rfs {

/**
 * Encapsulates the overall file system of a device.
 * The file system may consist of multiple partitions
 * realized by different kind of storages, e.g. SPI-FFS, SD-Card, etc.
 */
class RootFileSystem : public Singleton<RootFileSystem>, public FS {
		typedef Singleton<RootFileSystem> Base;

	private:
		// align count with partition enum
		static const size_t mMaxPartitionCnt = 2;

	public:
		typedef FsPartition::PartitionType PartitionType;

		/** The enumeration of modes for using files. */
		struct FileMode {
				typedef enum {
					/** Read access. */
					Read,
					/** Write access, overwrites content. */
					WriteTruncate,
					/** Write access, appends content. */
					WriteAppend,
				} Enum;
		};

		static const char* ToModeStr(FileMode::Enum mode) {
			switch(mode) {
				case FileMode::Read: return "r";
				case FileMode::WriteTruncate: return "w";
				case FileMode::WriteAppend: return "a";
			}
			return "r";
		}

		/**
		 * Initializes and starts-up the RFS by mounting all configured partitions.
		 */
		bool Start();

		/**
		 * Shuts-down the RFS by removing all mounted partitions and freeing all allocated resources.
		 */
		void Shutdown();

		/**
		 * Mounts the specified partitions of the file system.
		 * Multiple partitions can be conjunct (PART_A | PART_B | ...).
		 * If the parameter is omitted, all available partitions are mounted.
		 *
		 * @param partitionTypes One or more partitions to mount.
		 * @return True if mounting was successful, false otherwise.
		 */
		bool Mount(PartitionType::Enum partitionTypes = PartitionType::All);

		/**
		 * Removes the mounts the specified partitions of the file system.
		 * Multiple partitions can be conjunct (PART_A | PART_B | ...).
		 * If the parameter is omitted, all available partitions are removed.
		 *
		 * @param partitionTypes One or more partitions to mount.
		 */
		void Unmount(PartitionType::Enum partitionTypes = PartitionType::All);

		/**
		 * Determines whether or not all of the given partitions are mounted.
		 * Multiple partitions can be conjunct (PART_A | PART_B | ...).
		 * If the parameter is omitted, this function determines if at least
		 * one partition is mounted.
		 *
		 * @param partitionTypes One or more partitions to evaluate.
		 * @return True if all specified partitions are mounted, false otherwise.
		 */
		bool IsMounted(PartitionType::Enum partitionTypes = PartitionType::Any);

		/**
		 * Provides the mount point of the specified partition.
		 * Only one partition can be specified.
		 * @param partitionType The mount point of the specified partition.
		 * @return The name of the mount point or 0;
		 */
		const char* GetMountPoint(PartitionType::Enum partitionType);

		/**
		 * Formats all of the specified partitions. Note that not all
		 * kind of partitions support on-device formatting. E.g. SD-Cards
		 * need to be formatted by external tool before attaching them to
		 * the device.
		 * Multiple partitions can be conjuncted (PART_A | PART_B | ...).
		 * Not mounted partitions are attempted being formatted as well.
		 *
		 * @param partitionTypes One or more partitions to format.
		 * @return True if format was successful, false otherwise.
		 */
		bool Format(PartitionType::Enum partitionTypes);

		/**
		 * Provides the underlying files system of the specified partition.
		 * Only one partition can be specified.
		 * @param partitionType The partition for which the file system shall be retrieved.
		 * @return The pointer to the file system of the partition or 0;
		 */
		FsPartition* GetPartition(PartitionType::Enum partitionType);

		/**
		 * Provides the underlying files system of the specified partition.
		 * @param mountPoint The mount point of the requested partition.
		 * @return The pointer to the file system of the partition or 0;
		 */
		FsPartition* GetPartition(const char* mountPoint);

		/**
		 * Opens the first file on the given path in the given mode.
		 * Alias for FS::open(...).
		 *
		 * @param path The path of the file to open
		 * @param mode The mode of the file to open.
		 * @param create True to create the file if it does not exist, false otherwise.
		 * @return The opened file or an invalid file if no such file is available.
		 */
		File Open(const String& path, FileMode::Enum mode = FileMode::Read, const bool create = false);

		/**
		 * Iterates to the next file on any mounted partition.
		 * This is an alternative function to File::openNextFile which
		 * allows to iterate across partition boundaries in contract to
		 * File::openNextFile.
		 * The first file (iteration starting point) need to be opened
		 * using FS::open(...) function. If the file system root ("\") is
		 * applied as path there, this function iterates through
		 * all files of all partitions.
		 * After iteration is completed, allocated resources shall be
		 * released using method TerminateIteration().
		 * @param mode The mode for using the file.
		 * @return The opened file or an invalid file if no such file is available.
		 */
		File OpenNext(FileMode::Enum mode = FileMode::Read);

		/**
		 * Releases any potentially held resources that have been allocated
		 * via open() or OpenNextFile() method. Released resources are not
		 * closed or terminated in an other way.
		 */
		void TerminateIteration();

		/**
		 * Determines if the given path exists.
		 * Alias for FS::exists().
		 *
		 * @param path The path to evaluate.
		 * @return True if the path exists, false otherwise.
		 */
		bool Exists(const String& path);
		/**
		 * Renames the given 'from' path to the given 'to' path.
		 * Renaming across partition boundaries is not supported.
		 * Alias for FS::rename(...).
		 *
		 * @param pathFrom The path to rename.
		 * @param pathTo The new path.
		 * @return True if the operation was successful, false otherwise.
		 */
		bool Rename(const String& pathFrom, const String& pathTo);
		/**
		 * Removes the file at the give path.
		 * Alias for FS::remove(...).
		 *
		 * @param path The file to remove.
		 * @return True if the operation was successful, false otherwise.
		 */
		bool Remove(const String& path);
		/**
		 * Creates a new directory at the given path.
		 * Alias for FS::mkdir(...).
		 *
		 * @param path The path of the new directory.
		 * @return True if the operation was successful, false otherwise.
		 */
		bool MkDir(const String& path);
		/**
		 * Removes the directory at the given path.
		 * Alias for FS::rmdir(...).
		 *
		 * @param path The path of the directory to remove.
		 * @return True if the operation was successful, false otherwise.
		 */
		bool RmDir(const String& path);
		/**
		 * Provides the total capacity of one or more partitions.
		 * Multiple partitions can be conjuncted (PART_A | PART_B | ...).
		 * @param partitionTypes One or more partitions to examine.
		 * @return The total capacity in bytes.
		 */
		uint64_t GetCapacity(PartitionType::Enum partitionTypes = PartitionType::All) const;

		/**
		 * Provides the total occupied size of one or more partitions.
		 * Multiple partitions can be conjuncted (PART_A | PART_B | ...).
		 * @param partitionTypes One or more partitions to examine.
		 * @return The size in bytes that is occupied by files.
		 */
		uint64_t GetUsedSize(PartitionType::Enum partitionTypes = PartitionType::All) const;

		/**
		 * Provides the total size of one or more partitions that is available for
		 * storing files. Multiple partitions can be conjuncted (PART_A | PART_B | ...).
		 * @param partitionTypes One or more partitions to examine.
		 * @return The total available size in bytes.
		 */
		uint64_t GetFreeSize(PartitionType::Enum partitionTypes = PartitionType::All) const;

		void DumpDiagnostics() const;

	protected:
		/** Destructor */
		virtual ~RootFileSystem();

	private:
		friend class Singleton<RootFileSystem> ;
		friend class RfsImplUtils;

		struct PartitionNode {
//@formatter:off
				PartitionNode() : mPartitionType(PartitionType::None), mMountPoint(0), mFs(0) {};
//@formatter:on
				PartitionType::Enum mPartitionType;
				String mMountPoint;
				FsPartition* mFs;
		};

		typedef enum {
			OpExists, OpRemove, OpMkDir, OpRmDir
		} FsOp;

		/** Singleton constructor */
		RootFileSystem();
		bool MountPartition(PartitionType::Enum partitionType);
		PartitionNode* GetPartitionNode(const String& path);
		const char* GetMountPoint(size_t partitionIdx);
		String GetMountPoint(const String& path);
		String MapPath(const String& path);
		bool ProcessFile(const String& path, FsOp op);

		File mLastOpenedFile;
		size_t mNextPartitionIdx;
		PartitionNode mPartitionData[mMaxPartitionCnt];

};

} // namespace Rfs
