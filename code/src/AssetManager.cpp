#include <AssetManager.h>

string AssetManager::APP_PATH;

// Set the app path for the asset manager
void AssetManager::SetAppPath(string path)
{
			APP_PATH = path;
};

// Get the main app path
string AssetManager::GetAppPath()
{
	return APP_PATH;
}