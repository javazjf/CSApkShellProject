/*
 * ApkFile.h
 *
 *  Created on: 2015年3月30日
 *      Author: zhangjianfa
 */

#ifndef UTILS_APKFILE_H_
#define UTILS_APKFILE_H_
#include <string>
using namespace std;

class ApkFile{
private:
	string mApkFilePath;
	string mUncompressPath;
public:
	ApkFile(string path);
	void unzip();
	void decodeAndroidManifest();
	void modifyApplication();
	string getUncompressPath();
};

#endif /* UTILS_APKFILE_H_ */
