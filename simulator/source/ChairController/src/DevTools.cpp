#include "DevTools.h"

#include <utils/Log.h>

using namespace Rfs;

void DevTools::CheckRfs() {
	// Root file system
	RootFileSystem* rfs = RootFileSystem::GetInstance();
	if (rfs != 0) {
		rfs->Start();
		rfs->DumpDiagnostics();

		LogInfo("File System content:");
		File root = rfs->Open("/", RootFileSystem::FileMode::Read);
		if (root) {
			TraverseDirectory(root, 0);
			root.close();
		} else {
			LogDbg("Failed opening root directory '/'");
		}

		LogInfo("----------------------------------------------------");

		String dirPath("/ffs/temp");
		if (rfs->MkDir(dirPath)) {
			LogDbg("Created directory '%s'", dirPath.c_str());
			String filePath = dirPath + "/myFile.txt";
			File myFile = rfs->Open(filePath, RootFileSystem::FileMode::WriteTruncate);
			if (myFile) {
				LogDbg("Opened file '%s' (%s) for writing: (%d)", filePath.c_str(), myFile.name(), myFile.availableForWrite());
				const char* testText = "This is a test text";
				size_t txtLen = strlen(testText);
				size_t writtenLen = myFile.write((const uint8_t*)testText, txtLen + 1);
				LogDbg("Wrote text '%s' (%d) to file (%s), %d bytes written", testText, txtLen, myFile.name(), writtenLen);
				myFile.flush();
				myFile.close();
				LogDbg("File flushed and closed");

				myFile = rfs->Open(filePath, RootFileSystem::FileMode::Read);
				LogDbg("Opened file '%s' for reading", myFile.name());

				size_t bufferLen = txtLen + 32;
				uint8_t buffer[bufferLen];
				size_t readLen = myFile.read(buffer, bufferLen);
				LogDbg("Read text '%s' from file (%s), %d bytes read", (char* )buffer, myFile.name(), readLen);
				myFile.close();
				LogDbg("File closed");
			} else {
				LogDbg("Failed opening file '%s'", filePath.c_str());
			}
		} else {
			LogDbg("Failed creating directory '%s'", dirPath.c_str());
		}
		LogInfo("----------------------------------------------------");

		LogInfo("--- +++++++ ----");
		root = rfs->Open("/ffs", RootFileSystem::FileMode::Read);
		TraverseDirectory(root, 0);
		root.close();

		LogInfo("----------------------------------------------------");
		LogInfo("");
	}
}

void DevTools::TraverseDirectory(File dir, size_t level) {
	if (dir) {
		LogInfo(" -- [%c] %s, %d B", (dir.isDirectory() ? 'D' : 'F'), dir.name(), dir.size());
		File f = dir.openNextFile("r");
		while (f) {
			if (f.isDirectory()) {
				TraverseDirectory(f, level + 1);
			} else {
				LogInfo("-- %2d: [%c] %s, %d B", level, (f.isDirectory() ? 'D' : 'F'), f.name(), f.size());
			}
			f.close();
			f = dir.openNextFile("r");
		}
	} else {
		LogDbg("-- traversal aborted, invalid directory");
	}
}
