/*
 * ApkFile.cpp
 *
 *  Created on: 2015年3月30日
 *      Author: zhangjianfa
 */
#include "ApkFile.h"
#include "../zlib/unzip.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <direct.h>
#include "AxmlParser.h"
using namespace std;

const string ANDROIDMANIFEST_XML = "AndroidManifest.xml";

int mkdir_r(const char *path) {
	if (path == NULL) {
		return -1;
	}
	char *temp = strdup(path);
	char *pos = temp;

	/* 去掉开头的 './' 或 '/' */
	if (strncmp(temp, "/", 1) == 0) {
		pos += 1;
	} else if (strncmp(temp, "./", 2) == 0) {
		pos += 2;
	}
	/* 循环创建目录 */
	for (; *pos != '\0'; ++pos) {
		if (*pos == '/') {
			*pos = '\0';
			mkdir(temp);
			printf("for %s\n", temp);
			*pos = '/';
		}
	}
	/* 如果最后一级目录不是以'/'结尾，
	 遇到'\0'就中止循环，
	 不会创建最后一级目录 */
	if (*(pos - 1) != '/') {
		printf("if %s\n", temp);
		mkdir(temp);
	}
	free(temp);
	return 0;
}

ApkFile::ApkFile(string path) {
	this->mApkFilePath = path;
	this->mUncompressPath = string("");
}

void ApkFile::unzip() {
	char *dir;
	dir = getcwd(NULL, 0);
	cout << "Current Working Directory:" << dir << ",Apk File:"
			<< this->mApkFilePath << endl;
	int loc = this->mApkFilePath.find_last_of("/");
	int dot = this->mApkFilePath.find_last_of(".");
	if (loc == -1) {
		this->mUncompressPath += this->mApkFilePath.substr(0,
				this->mApkFilePath.length() - 4);
	} else {
		this->mUncompressPath += this->mApkFilePath.substr(0, loc + 1);
		this->mUncompressPath += this->mApkFilePath.substr(loc + 1,
				dot - loc - 1);
	}
	char dos_cmd[256];
	strcpy(dos_cmd, this->mUncompressPath.c_str());
	_mkdir(dos_cmd);

	unzFile uf = NULL;
	unzFile data[1024 * 100];
	unz_global_info64 gi;
	unz_file_info64 FileInfo;
	//打开zip文件
	uf = unzOpen64(this->mApkFilePath.c_str());
	int result = unzGetGlobalInfo64(uf, &gi);
	if (result != UNZ_OK)
		cout << "get global info error!" << endl;
	char fileName[256];
	int i = 0;
	string subDir, subFile;
	//循环解压缩文件
	for (i = 0; i < gi.number_entry; ++i) {
		if (unzGetCurrentFileInfo64(uf, &FileInfo, fileName, 256, NULL, 0, NULL,
				0) != UNZ_OK)
			printf("%s", "error");
		if (!(FileInfo.external_fa & 0x10)) { //文件，否则为目录
			string cfile = string(fileName);
			int loc = cfile.find_last_of("/");
			if (loc == -1) {
				subDir = this->mUncompressPath;
				subFile = this->mUncompressPath + "/" + cfile;
			} else {
				subDir = this->mUncompressPath + "/" + cfile.substr(0, loc);
				subFile = this->mUncompressPath + "/" + cfile;
			}
			strcpy(dos_cmd, subDir.c_str());
			if (_access(dos_cmd, 0) == -1) {
				mkdir_r(dos_cmd);
			}

			//打开文件
			result = unzOpenCurrentFile(uf);/* 无密码 */
			//			result=unzOpenCurrentFilePassword(uf,"123"); /* 有密码 */
			//读取内容
			strcpy(fileName, subFile.c_str());
			FILE* destFile = fopen(fileName, "wb");
			int size = 0;
			while ((size = unzReadCurrentFile(uf, data, sizeof(data))) != 0) {
				fwrite(data, 1, size, destFile);
			}
			fclose(destFile);
		} else {
			printf("d:%s\n", fileName);
		}
		//关闭当前文件
		unzCloseCurrentFile(uf);

		//出错
		if (i < gi.number_entry - 1 && unzGoToNextFile(uf) != UNZ_OK) {
			cout << "error!" << endl;
		}
	}
	//关闭流
	unzClose(uf);
	free(dir);
	free(fileName);
}

void ApkFile::decodeAndroidManifest() {
	string manifest = this->mUncompressPath + "/" + ANDROIDMANIFEST_XML;
	FILE *fp;
	char *inbuf;
	size_t insize;
	char *outbuf;
	size_t outsize;
	int ret;
	fp = fopen(manifest.c_str(), "rb");
	if (fp == NULL) {
		cout << "AndroidManifest.xml not found!!" << endl;
		return;
	}
	fseek(fp, 0, SEEK_END);
	insize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	inbuf = (char *) malloc(insize * sizeof(char));
	if (inbuf == NULL) {
		fprintf(stderr, "Error: init file buffer.\n");
		fclose(fp);
		return;
	}

	ret = fread(inbuf, 1, insize, fp);
	if (ret != insize) {
		fprintf(stderr, "Error: read file.\n");
		free(inbuf);
		fclose(fp);
		return;
	}

	ret = AxmlToXml(&outbuf, &outsize, inbuf, insize);

	free(inbuf);
	fclose(fp);

	if (ret == 0){
		printf("%s", outbuf);
		FILE *fout;
		string out_path = this->mUncompressPath + "/" + ANDROIDMANIFEST_XML + "_bak";
		fout = fopen(out_path.c_str(),"w");
		fprintf(fout,outbuf);
		fclose(fout);
	}

	free(outbuf);
}
string ApkFile::getUncompressPath() {
	return this->mUncompressPath;
}

