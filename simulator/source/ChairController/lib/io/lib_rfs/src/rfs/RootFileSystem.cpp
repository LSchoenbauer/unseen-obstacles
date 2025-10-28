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

#include <rfs/RfsStandardConfig.h>
#include <rfs/RootFileSystem.h>
#include <hal/rfs/SdMmcPartition.h>
#include <hal/HalFab.h>
#include <hal/rfs/FileAdapter.h>
#include <utils/Log.h>
#include <rfs/RfsLogger.h>

#include <FS.h>
#include <math.h>

#ifdef LOG_RFS_DBG_ENABLED
#ifndef LOG_DEBUG_ENABLED
#define LOG_DEBUG_ENABLED
#endif
#else
#ifdef LOG_DEBUG_ENABLED
#undef LOG_DEBUG_ENABLED
#endif
#endif

namespace Rfs {

RootFileSystem::RootFileSystem() :
        Base(), FS(0), mLastOpenedFile(), mNextPartitionIdx(mMaxPartitionCnt) {
	RfsLogDbg("C'tor RootFileSystem");
	_impl = Hal::HalFab::GetRfsImplPtr(*this);
	RfsLogDbg("RfsImplPtr: %p", _impl.get());
}

RootFileSystem::~RootFileSystem() {
	Unmount(PartitionType::All);
}

bool RootFileSystem::Start() {
	bool rc = true;
#ifdef RFS_USE_SPIFFS
	RfsLogDbg("RFS: Mounting SPIFFS part. ...");
	rc = Mount(PartitionType::SPI_FFS) && rc;
	RfsLogDbg("RFS: Mounted SPIFFS part.: %c", rc ? 'Y' : 'N');
#endif
#ifdef RFS_USE_SDMMC
	RfsLogDbg("RFS: Mounting SD-MMC part. ...");
	rc = Mount(PartitionType::SD_MMC_FS) && rc;
	RfsLogDbg("RFS: Mounted SD-MMC part.: %c", rc ? 'Y' : 'N');
#endif
	return rc;
}

void RootFileSystem::Shutdown() {
	Unmount(PartitionType::All);
}

bool RootFileSystem::Mount(PartitionType::Enum partitionTypes) {
	bool rc = true;
	if ((partitionTypes & PartitionType::SPI_FFS) != 0) {
		rc = MountPartition(PartitionType::SPI_FFS) && rc;
	}
	if ((partitionTypes & PartitionType::SD_MMC_FS) != 0) {
		rc = MountPartition(PartitionType::SD_MMC_FS) && rc;
	}
	return rc;
}

bool RootFileSystem::MountPartition(PartitionType::Enum partitionType) {
	bool rc = false;
	for (size_t i = 0; i < mMaxPartitionCnt && !rc; i++) {
		if ((mPartitionData[i].mPartitionType & partitionType) != 0) {
			// already mounted.
			rc = true;
		}
	}
	RfsLogDbg("RFS: mounting part.: %c", rc ? 'N' : 'Y');

	if (!rc) {
		size_t idx = 0;
		while (idx < mMaxPartitionCnt && mPartitionData[idx].mPartitionType != PartitionType::None) {
			idx++;
		}

		const char* partitionName = "??";
		const char* mountPoint = "??";
		if (idx < mMaxPartitionCnt) {
			FsPartition* partition = FsPartition::GetPartitionInstance(partitionType);
			RfsLogDbg("RFS: part. instance: %p", partition);
			if (partition != 0) {
				switch (partitionType) {
					case PartitionType::SPI_FFS: {
#ifdef RFS_USE_SPIFFS
						partitionName = "SPI FFS";
						mountPoint = gStdRfsCfg.spiffs.mountPoint;
						RfsLogDbg("RFS: part. cfg: %s", mountPoint);
						FsPartition::SpiFfsMountCfg spiFfsCfg(mountPoint, gStdRfsCfg.spiffs.formatOnFail,
						        gStdRfsCfg.spiffs.partitionLabel, gStdRfsCfg.spiffs.maxOpenFiles);
						rc = partition->Mount(spiFfsCfg);
#endif
						break;
					}
					case PartitionType::SD_MMC_FS: {
#ifdef RFS_USE_SDMMC
						partitionName = "SD-MMC FS";
						mountPoint = gStdRfsCfg.sd_mmc.mountPoint;
						FsPartition::SdMmcMountCfg sdMmcCfg(mountPoint, gStdRfsCfg.sd_mmc.formatOnFail,
						        gStdRfsCfg.sd_mmc.mode1Bit);
						rc = partition->Mount(sdMmcCfg);
						if (rc && partitionType == PartitionType::SD_MMC_FS) {
							Hal::SdMmcPartition* sdPartition = (static_cast<Hal::SdMmcPartition*>(partition));
							if (sdPartition == 0 || sdPartition->GetCardType() == Hal::SdMmcPartition::CardType::None
							        || sdPartition->GetCardType() == Hal::SdMmcPartition::CardType::Unknown) {
								LogError("SD-MMC FS mounted at %s, but no SD Card is attached",
										mPartitionData[idx].mMountPoint ? mPartitionData[idx].mMountPoint.c_str() : "NULL");
								rc = false;
							}
						}
#endif
						break;
					}
					case PartitionType::None:
					case PartitionType::All:
						// nothing to do, specified for completeness.
						break;
				}

				if (rc) {
					RfsLogDbg("RFS: mounted part.");
					mPartitionData[idx].mPartitionType = partitionType;
					mPartitionData[idx].mFs = partition;
					mPartitionData[idx].mMountPoint = mountPoint;
				}
			}
		}

		if (!rc) {
			LogError("Mounting %s at '%s' failed.", partitionName, mountPoint != 0 ? mountPoint : "NULL");
		}
	}
	return rc;
}

void RootFileSystem::Unmount(PartitionType::Enum partitionType) {
	for (size_t i = 0; i < mMaxPartitionCnt; i++) {
		if ((mPartitionData[i].mPartitionType & partitionType) != 0) {
			mPartitionData[i].mPartitionType = PartitionType::None;
			if (mPartitionData[i].mFs != 0) {
				mPartitionData[i].mFs->Unmount();
			}
			mPartitionData[i].mMountPoint = String();
			mPartitionData[i].mFs = 0;
		}
	}
}

bool RootFileSystem::IsMounted(PartitionType::Enum partitionTypes) {
	bool querySpiFfs = (partitionTypes & PartitionType::SPI_FFS) != 0;
	bool querySdMmc = (partitionTypes & PartitionType::SD_MMC_FS) != 0;
	bool hasSpiFfs = false;
	bool hasSdMmc = false;
	for (size_t i = 0; i < mMaxPartitionCnt; i++) {
		hasSpiFfs = hasSpiFfs || (mPartitionData[i].mPartitionType & PartitionType::SPI_FFS) != 0;
		hasSdMmc = hasSdMmc || (mPartitionData[i].mPartitionType & PartitionType::SD_MMC_FS) != 0;
	}

	return (querySpiFfs ? hasSpiFfs : true) && (querySdMmc ? hasSdMmc : true);
}

const char* RootFileSystem::GetMountPoint(PartitionType::Enum partition) {
	const char* mntPt = 0;
	for (size_t i = 0; i < mMaxPartitionCnt && mntPt == 0; i++) {
		if ((mPartitionData[i].mPartitionType & partition) != 0) {
			mntPt = mPartitionData[i].mMountPoint.c_str();
		}
	}
	return mntPt;
}

bool RootFileSystem::Format(PartitionType::Enum partitionTypes) {
	bool rc = false;
	for (size_t i = 0; i < mMaxPartitionCnt; i++) {
		if ((mPartitionData[i].mPartitionType & partitionTypes) != 0) {
			rc = mPartitionData[i].mFs->Format() && rc;
		}
	}
	return rc;
}

FsPartition* RootFileSystem::GetPartition(PartitionType::Enum partitionType) {
	FsPartition* fs = 0;
	for (size_t i = 0; i < mMaxPartitionCnt && fs == 0; i++) {
		if ((mPartitionData[i].mPartitionType & partitionType) != 0) {
			fs = mPartitionData[i].mFs;
		}
	}
	return fs;
}

FsPartition* RootFileSystem::GetPartition(const char* mountPoint) {
	FsPartition* fs = 0;
	for (size_t i = 0; i < mMaxPartitionCnt && fs == 0; i++) {
		if (mPartitionData[i].mPartitionType != PartitionType::None && mPartitionData[i].mMountPoint == mountPoint) {
			fs = mPartitionData[i].mFs;
		}
	}
	return fs;
}

File RootFileSystem::Open(const String& path, FileMode::Enum mode, const bool create) {
	String realPath = MapPath(path);
	RfsLogDbg(" - opening file '%s' @ '%s, next partition: #%d", path.c_str(), realPath.c_str(), mNextPartitionIdx);
	if (mLastOpenedFile) {
		mLastOpenedFile.close();
	}
	File f;
	mLastOpenedFile = f;
	if (realPath) {
		if (realPath.length() == 1 && realPath.charAt(0) == '/') {
			mNextPartitionIdx = 0;
			OpenNext(mode); // opens the first file of the next valid partition.
		} else {
			mNextPartitionIdx = mMaxPartitionCnt; // no iteration across partition boundaries.
			PartitionNode* partitionNode = GetPartitionNode(realPath);
			if (partitionNode != 0 && partitionNode->mFs != 0) {
				String partitionPath("/");
				if (realPath.length() > partitionNode->mMountPoint.length()) {
					partitionPath = realPath.substring(partitionNode->mMountPoint.length());
				}
//				RfsLogDbg(" - opening file '%s' on partition #%d", partitionPath.c_str(), mNextPartitionIdx);
				mLastOpenedFile = partitionNode->mFs->open(partitionPath, ToModeStr(mode), create);
			}
		}
	}
	return mLastOpenedFile;
}

File RootFileSystem::OpenNext(FileMode::Enum mode) {
	if (mLastOpenedFile) {
		RfsLogDbg(" - opening file next to '%s' on partition #%d", mLastOpenedFile.name(), mNextPartitionIdx);
		mLastOpenedFile = Hal::FileAdapter::OpenNextFile(mLastOpenedFile, ToModeStr(mode));
	} else if (mNextPartitionIdx < mMaxPartitionCnt) {
		// find next mounted partition
		for (; mNextPartitionIdx < mMaxPartitionCnt && !mLastOpenedFile; mNextPartitionIdx++) {
			if (mPartitionData[mNextPartitionIdx].mPartitionType != PartitionType::None) {
				// open first file from next partition
				RfsLogDbg(" - opening first file on partition #%d", mNextPartitionIdx);
				mLastOpenedFile = File(
				        Hal::HalFab::GetRfsFileImplPtr(
								mPartitionData[mNextPartitionIdx].mFs->open("/", ToModeStr(mode)),
				                mPartitionData[mNextPartitionIdx].mMountPoint));
				// TODO Cleanup
				// mLastOpenedFile = File(
				//         Hal::HalFab::GetRfsFileImplPtr(mPartitionData[mNextPartitionIdx].mFs->open("/", ToModeStr(mode)),
				//                 mPartitionData[mNextPartitionIdx].mMountPoint), mPartitionData[mNextPartitionIdx].mFs);
				RfsLogDbg(" - opened file '%s' on partition #%d", mLastOpenedFile ? mLastOpenedFile.name() : "NONE",
				        mNextPartitionIdx);
			}
		}
	}
	return mLastOpenedFile;
}

void RootFileSystem::TerminateIteration() {
	if (mLastOpenedFile) {
		mLastOpenedFile = File();
	}
}

bool RootFileSystem::Exists(const String& path) {
	return ProcessFile(path, FsOp::OpExists);
}

bool RootFileSystem::Rename(const String& pathFrom, const String& pathTo) {
	bool rc = false;
	if (pathFrom && pathTo != 0) {
		String realFrom = MapPath(pathFrom);
		String realTo = MapPath(pathTo);
		PartitionNode* partitionNode = GetPartitionNode(realFrom);
		if (partitionNode != 0 && partitionNode->mFs != 0) {
			if (realTo.startsWith(partitionNode->mMountPoint) && realTo.length() > partitionNode->mMountPoint.length()
			        && realTo.charAt(partitionNode->mMountPoint.length()) == '/') {

				String partitionPathFrom = realFrom.substring(partitionNode->mMountPoint.length());
				String partitionPathTo = realTo.substring(partitionNode->mMountPoint.length());
				rc = partitionNode->mFs->rename(partitionPathFrom, partitionPathTo);
			} else {
				LogWarn("Rename is only supported within the same partition");
			}
		} else {
			LogWarn("Partition for source path is not found");
		}
	} else {
		LogWarn("Rename paths may not be NULL");
	}
	return rc;
}

bool RootFileSystem::Remove(const String& path) {
	return ProcessFile(path, FsOp::OpRemove);
}

bool RootFileSystem::MkDir(const String& path) {
	return ProcessFile(path, FsOp::OpMkDir);
}

bool RootFileSystem::RmDir(const String& path) {
	return ProcessFile(path, FsOp::OpRmDir);
}

uint64_t RootFileSystem::GetCapacity(PartitionType::Enum partitionTypes) const {
	uint64_t capacity = 0;
	for (size_t i = 0; i < mMaxPartitionCnt; i++) {
		if ((mPartitionData[i].mPartitionType & partitionTypes) != 0 && mPartitionData[i].mFs != 0) {
			capacity += mPartitionData[i].mFs->GetCapacity();
		}
	}
	return capacity;
}

uint64_t RootFileSystem::GetUsedSize(PartitionType::Enum partitions) const {
	size_t used = 0;
	for (size_t i = 0; i < mMaxPartitionCnt; i++) {
		if ((mPartitionData[i].mPartitionType & partitions) != 0 && mPartitionData[i].mFs != 0) {
			used += mPartitionData[i].mFs->GetUsedSize();
		}
	}
	return used;
}

uint64_t RootFileSystem::GetFreeSize(PartitionType::Enum partitions) const {
	uint64_t capacity = GetCapacity(partitions);
	uint64_t used = GetUsedSize(partitions);
	return (capacity >= used ? (capacity - used) : 0);
}

void RootFileSystem::DumpDiagnostics() const {
#ifdef LOG_INFO_ENABLED
	uint64_t fsCapa = GetCapacity();
	uint64_t fsUsed = GetUsedSize();
	uint64_t fsFree = GetFreeSize();

	LogInfo("");
	LogInfo("=============================");
	LogInfo("Root File System Info");
	LogInfo("  Capacity: %8llu B", fsCapa);
	LogInfo("  Used:     %8llu B (%2.1f %%)", fsUsed, ((fsUsed * 100.0) / fsCapa));
	LogInfo("  Free:     %8llu B (%2.1f %%)", fsFree, ((fsFree * 100.0) / fsCapa));
	LogInfo("-----------------------------");

	FsPartition::DumpPartitionTable();

	LogInfo("-----------------------------");
	LogInfo("Mounted partitions:");
	for (size_t i = 0; i < mMaxPartitionCnt; i++) {
		if (mPartitionData[i].mFs != 0) {
			const char* fsName =
			        mPartitionData[i].mPartitionType == PartitionType::SPI_FFS ?
			                "SPIFFS" : (mPartitionData[i].mPartitionType == PartitionType::SD_MMC_FS ? "SD_MMC" : "Unknown");
			LogInfo("  - %s at '%s'", fsName, mPartitionData[i].mMountPoint.c_str());
		}
	}

	LogInfo("-----------------------------");
	LogInfo("Mapped paths:");
	for (size_t i = 0; i < gStdRfsCfg.pathMapSize; i++) {
		LogInfo("  %s -> %s", gStdRfsCfg.pathMap[i].origin, gStdRfsCfg.pathMap[i].destination);
	}
	LogInfo("-----------------------------");
	LogInfo("");
#endif
}

RootFileSystem::PartitionNode* RootFileSystem::GetPartitionNode(const String& path) {
	PartitionNode* node = 0;
	if (path && path.length() > 0 && path[0] == '/') {
		size_t segLen = path.indexOf('/', 1);
		segLen = segLen < 0 ? path.length() : segLen;
		for (size_t i = 0; node == 0 && i < mMaxPartitionCnt; i++) {
//			RfsLogDbg("path: %s, partition path: %s, len %d", path.c_str(), mPartitionData[i].mMountPoint.c_str(), segLen);
			if (mPartitionData[i].mMountPoint.length() == segLen && path.startsWith(mPartitionData[i].mMountPoint)) {
//				RfsLogDbg("Found node: %d", i);
				node = &mPartitionData[i];
			}
		}
	}
	return node;
}

const char* RootFileSystem::GetMountPoint(size_t partitionIdx) {
	return (partitionIdx < mMaxPartitionCnt && mPartitionData[partitionIdx].mPartitionType != PartitionType::None) ?
	        mPartitionData[partitionIdx].mMountPoint.c_str() : 0;
}

String RootFileSystem::GetMountPoint(const String& path) {
	String rootPath = "/";
	if (path && path.length() > 0 && path[0] == '/') {
		int segLen = path.indexOf('/', 1);
		segLen = segLen < 0 ? path.length() : segLen;
		rootPath = path.substring(0, segLen).c_str();
	}
	return rootPath;
}

String RootFileSystem::MapPath(const String& path) {
	String mappedPath = path;
	if (path && !path.isEmpty()) {
		for (size_t i = 0; i < gStdRfsCfg.pathMapSize; i++) {
			if (path.startsWith(gStdRfsCfg.pathMap[i].origin)) {
				mappedPath = gStdRfsCfg.pathMap[i].destination;
				const char* pathCStr = path.c_str();
				mappedPath.concat(&pathCStr[strlen(gStdRfsCfg.pathMap[i].origin)]);
				i = gStdRfsCfg.pathMapSize;
			}
		}
	}
	return mappedPath;
}

bool RootFileSystem::ProcessFile(const String& path, FsOp op) {
	bool rc = false;
	String realPath = MapPath(path);
	PartitionNode* partitionNode = GetPartitionNode(realPath);
	RfsLogDbg("Processing file: '%s' (real path '%s'), partition '%s'", path.c_str(), realPath.c_str(),
	        (partitionNode != 0 ? partitionNode->mMountPoint.c_str() : "NONE"));

	if (partitionNode != 0 && partitionNode->mFs != 0) {
		String partitionPath = realPath.substring(partitionNode->mMountPoint.length());
		switch (op) {
			case FsOp::OpExists:
				rc = partitionNode->mFs->exists(partitionPath);
				break;
			case FsOp::OpRemove:
				rc = partitionNode->mFs->remove(partitionPath);
				break;
			case FsOp::OpMkDir:
				rc = partitionNode->mFs->mkdir(partitionPath);
				break;
			case FsOp::OpRmDir:
				rc = partitionNode->mFs->rmdir(partitionPath);
				break;
		}
	} else {
		const char* opName = "?cmd?";
		switch (op) {
			case FsOp::OpExists:
				opName = "exists";
				break;
			case FsOp::OpRemove:
				opName = "remove";
				break;
			case FsOp::OpMkDir:
				opName = "mkdir";
				break;
			case FsOp::OpRmDir:
				opName = "rmdir";
				break;
		}
		LogWarn("%s %s: PartitionType not mounted", opName, realPath ? realPath.c_str() : "NULL");
	}
	return rc;
}

} // namespace Rfs
