#include <gishandler.h>

void transformToUtm(double& x, double& y)
{
    OGRSpatialReference    oUTM, *poLatLong;
    OGRCoordinateTransformation *poTransform;

    oUTM.SetProjCS("UTM 11 / WGS84");
    oUTM.SetWellKnownGeogCS( "WGS84" );
    oUTM.SetUTM( 11 );

    poLatLong = oUTM.CloneGeogCS();
    
    poTransform = OGRCreateCoordinateTransformation( poLatLong, &oUTM );
    if( poTransform == NULL )
    {
        return;
    }
    

    if( !poTransform->Transform( 1, &x, &y) )
    {
      // Error here
    }
};

void ComputeGeoProperties(GDALDataset *poDataset, int width, int height, double& x, double& y,double& xright, double& ybottom, double& xres, double& yres)
{
    double adfGeoTransform[6];
    if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
    {
        printf( "Origin = (%.6f,%.6f)\n",
                adfGeoTransform[0], adfGeoTransform[3] );

        printf( "Pixel Size = (%.6f,%.6f)\n",
                adfGeoTransform[1], adfGeoTransform[5] );
        x = adfGeoTransform[0];
        y = adfGeoTransform[3];
        xright = x + adfGeoTransform[1]*width;
        ybottom = y + adfGeoTransform[5]*height;
    }
    else
    {
      return;
    }

    string proj;
    proj = string(poDataset->GetProjectionRef());

    OGRSpatialReference sr2;
    char* test = &proj[0];
    sr2.importFromWkt(&test);
    // Get Geography Coordinate System
    OGRSpatialReference* geog2 = sr2.CloneGeogCS(); 

    // Now to create coordinate transform function
    OGRCoordinateTransformation* poTransform2 = OGRCreateCoordinateTransformation( &sr2, geog2 );

    // Compute corners
    poTransform2->Transform(1,&x,&y);
    poTransform2->Transform(1,&xright,&ybottom);

    // Transform to utm on both corners
    transformToUtm(x,y);
    transformToUtm(xright,ybottom);
    cout << xright << " " << ybottom <<endl;
    // Lets compute the resolution -- Despite the one provided by the geotransform
    double absoluteW = xright - x;
    double absoluteH = y - ybottom;

    // now lets compute the average resolution of the DEM
    xres = absoluteW / width;
    yres = absoluteH / height;
};



bool getRawValuesFromFile(string fname,vector<vector<float>>& vecs,float& min, float& max,float& xres, float& yres, string& projection)
{
   
   //vector<float> temp = vector<float>()
   GDALDataset *poDataset;

   
   // Register all gdal drivers
   GDALAllRegister();

   // lets load a "dataset" which is gdal terminology for your data
   poDataset= (GDALDataset*) GDALOpen(fname.c_str(),GA_ReadOnly);

   // error handing!
   if(poDataset == NULL)
   {
      cout << "Failure to load file due to not existing or write permissions!!!" << endl;
      return false;
   }


   // time to find the width of the data and print it for sanity
   cout << "Data size: " << GDALGetRasterXSize(poDataset) << " " << GDALGetRasterYSize(poDataset) << endl;

   // A new term "band" or pertentaining to one layer within a dataset
   GDALRasterBand  *poBand;

   int             nBlockXSize, nBlockYSize;
   int             bGotMin, bGotMax;
   double          adfMinMax[2];
   
   // Get the first band -- that's what this function does to grab one band.     
   poBand = poDataset->GetRasterBand( 1 );

   // Getting the dimensions of the band... only 2D
   poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );

   // Print some more handy information
   printf( "Block=%dx%d Type=%s, ColorInterp=%s\n",
            nBlockXSize, nBlockYSize,
            GDALGetDataTypeName(poBand->GetRasterDataType()),
            GDALGetColorInterpretationName(
            poBand->GetColorInterpretation()) );
   // Get the min and max
   min = adfMinMax[0] = poBand->GetMinimum( &bGotMin );
   max = adfMinMax[1] = poBand->GetMaximum( &bGotMax );

   // this guy will look at your current band and compute min max or you can do the above
   if( ! (bGotMin && bGotMax) )
      GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);

   // dimensions of our datasets
   int width = poBand->GetXSize(); 
   int height = poBand->GetYSize();

   double x,y,xright,ybottom,XRES,YRES;
   // Time to compute the xres and yres
   ComputeGeoProperties(poDataset,width,height,x,y,xright,ybottom,XRES,YRES);
   xres = XRES;
   yres = YRES;

   // something extra not need for this simple function but you can get the number
   // of bands in the dataset
   int bands = poDataset->GetRasterCount();

   // something to hold our values!!
   float *pafScanline;

   // printing the min and max of the dataset
   std::cout << "Before allocation " << "Min: "<< min << " Max: " << max << endl;
   
   // load the data and smile
   pafScanline = (float *) CPLMalloc(sizeof(float)*width*height);

   // I don't like dynamic float array (that is a lie) just a preference
   vector<vector<float>> out = vector<vector<float>>(width,vector<float> (height,0));

   auto err = poBand->RasterIO(GF_Read,0,0,width,height,pafScanline,width,height,GDT_Float32,0,0);
   cout << "Loaded data with status " << err << endl;

   // Lets check the status of the loading of this dataset...
   if(err != 0)
   {
      cout << "Dataset fail to load with the following error: ";
      switch(err)
      {
        // Look up CPLErrorHandler for more information
        case 1:
        cout << "DEBUG" << endl;
        break;
        case 2:
        cout << "Warning" << endl;
        break;
        case 3:
        cout << "Failure" << endl;
        break;
        case 4:
        cout << "Fatal" << endl;
        break;
      } 
   }

   // move everything to a vector -- slow memory but okay for what we are doing
   for(int i = 0; i < height; i++)
   {
    for(int j = 0; j < width; j++)
    {
      if(pafScanline[(height-1-i)*width+j] > 0)
      out[width-1-j][i] = pafScanline[(height-1-i)*width+j];
      else
      out[width-1-j][i] = 0;
    }
   }

   CPLFree(pafScanline);

   //for(auto i : out)
   //for(auto j : i)
   //		cout << j << endl;

  cout << "Done Loading" << endl;
  vecs = out;
   return true;
}

void bilinearintertop(vector<vector<float>>& input, int width, int height    )
{
  vector<vector<float>> vecs = vector<vector<float>>(height,vector<float>(width,0));
  for(int i = 0; i < vecs.size(); i++)
  {
    for(int j =0; j < vecs[i].size(); j++)
    {
      float iratio = (float)i/(float)vecs.size();
      float jratio = (float)j/(float)vecs[i].size();
      float i2 =  iratio * input.size();
      float j2 = jratio * input[0].size();
      float ib = floor(i2);
      float it = ceil(i2);
      float jt = floor(j2);
      float jb = ceil(j2);
      float xdelta = jt - jb;
      float ydelta = it - ib;
      // x interpolation - 0,0 1,0
      float x1 = (jt - j2)/ xdelta * input[ib][jb] + (j2 - jb)/xdelta*input[ib][jt];

      // x interpolation - 0,1 1,1
      float x2 = (jt - j2)/ xdelta * input[it][jb] + (j2 - jb)/xdelta*input[it][jt];

      // y interpolation - 0,0 1,0
      float val = (it - i2)/ ydelta * x1 + (i2 - ib)/ydelta*x2;
      vecs[i][j] = val;

    }
  }


}

void generateTexture(string fname, GLuint& tex, int bandnum)
{

   if(bandnum <= 0 )
   {
     bandnum = 1;
   }
   GDALDataset *poDataset;
   GDALAllRegister();
   poDataset= (GDALDataset*) GDALOpen(fname.c_str(),GA_ReadOnly);
   if(poDataset == NULL)
   {
      cout << "OUCH!" << endl;
      //exit(0);
      return;
   }
   cout << "Data size: " << GDALGetRasterXSize(poDataset) << " " << GDALGetRasterYSize(poDataset) << endl;

   GDALRasterBand  *poBand;
   int             nBlockXSize, nBlockYSize;
   int             bGotMin, bGotMax;
   double          adfMinMax[2];
  int bands = poDataset->GetRasterCount();
   bandnum = bandnum % bands + 1;
   if(bandnum > bands)
   {
      bandnum = 1;
   }
   poBand = poDataset->GetRasterBand( bandnum );
   poBand->GetBlockSize( &nBlockXSize, &nBlockYSize );
   printf( "Block=%dx%d Type=%s, ColorInterp=%s\n",
            nBlockXSize, nBlockYSize,
            GDALGetDataTypeName(poBand->GetRasterDataType()),
            GDALGetColorInterpretationName(
            poBand->GetColorInterpretation()) );

   float max = adfMinMax[0] = poBand->GetMinimum( &bGotMin );
   float min = adfMinMax[1] = poBand->GetMaximum( &bGotMax );
   if( ! (bGotMin && bGotMax) )
      GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
   int width = poBand->GetXSize(); 
   int height = poBand->GetYSize();

   float *pafScanline;
   std::cout << "Before allocation" << adfMinMax[0] << " " << adfMinMax[1] << endl;
   min = adfMinMax[0];
   max = adfMinMax[1];
   int dsize = 256;
   pafScanline = (float *) CPLMalloc(sizeof(float)*512*512);
   vector<vector<float>> out = vector<vector<float>>(height,vector<float> (width,0));
   //vector<vector<unsigned char>> texs = vector<vector<unsigned char>>(height,vector<unsigned char> (width,0));
   unsigned char texs[512*512];
   poBand->RasterIO(GF_Read,0,0,width,height,pafScanline,512,512,GDT_Float32,0,0);
   float no = poBand->GetNoDataValue();
   cout << "NO DATA VALUE: " << no << endl;
   cout << "After allocation" << endl;
   for(int i = 0; i < 512; i++)
   {
    for(int j = 0; j < 512; j++)
    {
      //cout << i << j << endl << pafS;
      if(pafScanline[i*width+j] != no)
      {
        // set tex val
        texs[i*512+j] = (unsigned char)(255*abs((pafScanline[i*512+j] - min)/(max-min)));
        //if(pafScanline[i*512+j] > 0)
        //cout << (int)texs[i*512 +j] << " " << pafScanline[i*512+j] << " " << no << " " << fname << " " << min << " " << max << endl;
      }
      else
      {
        // Set zero val
        texs[i*512+j] = 0;
        //cout << (int)texs[i*512 +j] << fname << endl;
      }
      //texs[i*512+j] = 255;

      //ut[i][j] = pafScanline[i*width+j];
    }
   }
   CPLFree(pafScanline);
   //exit(0);
   // Create a texture
   glGenTextures(1,&tex);
   glBindTexture(GL_TEXTURE_2D,tex);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512,512, 0, GL_RED, GL_UNSIGNED_BYTE,texs);
   GDALClose( (GDALDatasetH) poDataset);

   return;
}


vec3 ComputeNormal(vec3 center, int i, int j, int width, int height,vector<vector<float>>& data, float Max, float xres, float yres )
{
  // Compute center of all values which is the i and j passed in
  vec3 left;
  vec3 right;
  vec3 up;
  vec3 down;
  vec3 sum = vec3(0,0,0);
  bool l = false;
  bool r = false;
  bool u = false;
  bool d = false;

  int count = 0;
  // Compute left
  if(i -1 >= 0)
  {

    left = vec3((i-1)*xres,data[i-1][j],j*yres);
    left = center - left;
    l = true;
  }

  // Compute right
  if(i+1 < width)
  {
    right = vec3((i+1)*xres,data[i+1][j],j*yres);
    right = center - right;
    r = true;
  }

  // Compute up
  if(j-1 >= 0)
  {
    up = vec3((i)*xres,data[i][j-1],(j-1)*yres);
    up = center-up;
    u = true;
  }

  // Compute down
  if(j+1 < height)
  {
    down = vec3((i)*xres,data[i][j+1],(j+1)*yres);
    down = center-down;
    d = true;
  }

  // Compute normals
    if(u  && r)
    {
      vec3 v1 = cross(up,right);
      if(v1.y < 0)
      {
        v1 *= -1;
      }
      sum += v1;
      count = count + 1;
    }
    if(u && l)
    {
      vec3 v1 = cross(up,left);
      if(v1.y < 0)
      {
        v1 *= -1;
      }
      sum += v1;
      count = count + 1;
    }
    if(d && r)
    {
      vec3 v1 = cross(down,right);
      if(v1.y < 0)
      {
        v1 *= -1;
      }
      sum += v1;
      count = count + 1;
    }
    if(d && l)
    {
      vec3 v1 = cross(down,left);
      if(v1.y < 0)
      {
        v1 *= -1;
      }
      sum += v1;
      count = count + 1;
    }

  // Compute average normal
  sum /= count;
  auto t = normalize(sum); 
  //cout << t.x << " " << t.y << " " << t.z << endl;
  // Normalize it and return :D!!!! Enjoy your smoothed normal for some smooth shading!
  return normalize(sum);
};
void createMesh(vector<vector<float>>& input,float xres, float yres,float max,vector<int>& indicies, vector<Vertex>& vertexes)
{
  cout << "MAX: " << max;
  cout << "XRES " << xres << " YRES" << yres << endl;
  // Our vertex information along with normals contained inside
  std::vector<Vertex> vectors = vector<Vertex>();

  // Lets do some index rendering because it will save us some memory to some degree
  vector<int> indexs = vector<int>();

  // Time to construct a height map based on the xres and yres
  for(int i = 0; i < input.size()-1; i++)
  {
    for(int j = 0; j < input[i].size()-1; j++)
    {  

        float UL = (float)(input[i][j])/(float)(max); // Upper left
        float LL = (float)(input[i+1][j])/(float)(max); // Lower left
        float UR = (float)(input[i][j+1])/(float)(max); // Upper right
        float LR = (float)(input[i+1][j+1])/(float)(max); // Lower right
        //cout << input[i][j] << " " << max <<endl;

        if(UL <= 0)
        {
          UL = 0;
        }
        //else
          //UL = .5;
        if(UR <= 0)
        {
          UR = 0;
        }
        //else
          //UR = .5;
        if(LR <= 0)
        {
          LR = 0;
        }
        //else
         // LR = .5;
        if(LL <= 0)
        {
          LL = 0;
        }
        //else
         // LL = .5;

        
        vec3 ULV = {i*xres,UL*max,j*yres};
        vec3 LLV = {(i+1)*xres,LL*max,j*yres};
        vec3 URV = {i*xres,UR*max,(j+1)*yres};
        vec3 LRV = {(i+1)*xres,LR*max,(j+1)*yres};

        // compute smoothed normal
        vec3 a = ComputeNormal(ULV,i,j,input.size(),input[i].size(),input,max,xres,yres);
        vec3 b = ComputeNormal(LLV,i+1,j,input.size(),input[i].size(),input,max,xres,yres);
        vec3 c = ComputeNormal(URV,i,j+1,input.size(),input[i].size(),input,max,xres,yres);
        vec3 d = ComputeNormal(LRV,i+1,j+1,input.size(),input[i].size(),input,max,xres,yres);

        vectors.push_back(Vertex{ {i*xres,UL,j*yres}, a, {(float)i/(float)input.size(),(float)j/(float)input[i].size()} } );
        vectors.push_back(Vertex{ {(i+1)*xres,LL,j*yres}, b, {(float)(i+1)/(float)input.size(),(float)j/(float)input[i].size()} } );
        vectors.push_back(Vertex{ {i*xres,UR,(j+1)*yres}, c, {(float)i/(float)input.size(),(float)(j+1)/(float)input[i].size()} } );
        vectors.push_back(Vertex{ {(i+1)*xres,LR,(j+1)*yres}, d, {(float)(i+1)/(float)input.size(),(float)(j+1)/(float)input[i].size()} } );
        //cout << vectors[vectors.size()-1].normal.z << endl;

        indexs.push_back(vectors.size() - 4);
        indexs.push_back(vectors.size() - 1);
        indexs.push_back(vectors.size() - 2);
        indexs.push_back(vectors.size() - 4);
        indexs.push_back(vectors.size() - 3);
        indexs.push_back(vectors.size() - 1);
        
    }
    //break;
  }
  
  // Pass Vectors out
  indicies = indexs;
  vertexes = vectors;
} 
