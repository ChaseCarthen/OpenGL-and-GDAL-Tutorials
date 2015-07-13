#include <AssetManager.h>

string AssetManager::APP_PATH;

void AssetManager::SetAppPath(string path)
{
			APP_PATH = path;
};

string AssetManager::GetAppPath()
{
	return APP_PATH;
}