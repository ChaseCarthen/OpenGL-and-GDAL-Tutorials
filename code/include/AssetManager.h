#include <string>

using namespace std;

#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

class AssetManager
{
	public:
		AssetManager()
		{};
		static void SetAppPath(string path);
		static string GetAppPath();
		
	private:
		static string APP_PATH;
};

#endif