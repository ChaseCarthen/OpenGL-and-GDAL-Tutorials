#include <Magick++.h> 
#include <iostream> 
using namespace std; 
using namespace Magick; 
int main(int argc,char **argv) 
{ 
  InitializeMagick(*argv);

  // Construct the image object. Separating image construction from the 
  // the read operation ensures that a failure to read the image file 
  // doesn't render the image object useless. 
  if(argc > 1)
  {
  Image image;
  try { 
    // Read a file into image object 
    image.read( argv[1]);

int w = image.columns();
int h = image.rows();
 
unsigned char* data = new unsigned char[w*h];
//Blob blob;
image.write(0,0,w,h,"RGBA",CharPixel,(void*)data);
  } 
  catch( Exception &error_ ) 
    { 
      cout << "Caught exception: " << error_.what() << endl; 
      return 1; 
    } 
  }
  return 0; 
}