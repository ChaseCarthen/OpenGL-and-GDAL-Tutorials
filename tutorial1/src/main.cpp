#include <iostream>
// Gdal includes that are needed for this tutorial
#include <gdal.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>

using namespace std;

int main(int argc, char** argv )
{
	float xright,ybottom,x,y;
	if (argc > 1)
	{
		// Now getting information for this dataset
		cout << "File Inputed: " << argv[1] << endl;

		// Lets open a Tif
		GDALDataset *poDataset;

		// Register all gdal drivers -- load every possible driver gdal has
		GDALAllRegister();

		// lets load a "dataset" which is gdal terminology for your data
		poDataset = (GDALDataset*) GDALOpen(argv[1], GA_ReadOnly);

		// Get width and height of this dataset

		int width = GDALGetRasterXSize(poDataset);
		int height = GDALGetRasterYSize(poDataset);
		cout << "Data size: " << width << " " << height << endl;

		// Get projection information of this dataset
		string proj;
		proj = string(poDataset->GetProjectionRef());
		cout << "Projection: " << proj << endl;

		// get geo transform for this dataset
		double adfGeoTransform[6];
		if ( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
		{
			printf( "Origin = (%.6f,%.6f)\n",
			        adfGeoTransform[0], adfGeoTransform[3] );

			printf( "Pixel Size = (%.6f,%.6f)\n",
			        adfGeoTransform[1], adfGeoTransform[5] );
			x = adfGeoTransform[0];
			y = adfGeoTransform[3];
			xright = x + adfGeoTransform[1] * width;
			ybottom = y + adfGeoTransform[5] * height;
			cout << "East: " << x << " West: " << xright << " North: " << y << " South: " << ybottom << endl; 
			cout << "Pixel Size(x y): " << adfGeoTransform[1] << " " << adfGeoTransform[5] << endl;
		}
		else
		{
			return 1;
		}
		
	}
	return 0;

}
