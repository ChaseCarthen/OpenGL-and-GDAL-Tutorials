#include <OpenGL/gl3.h>
#include <stdlib.h>
#ifndef _GBUFFER_H_
#define _GBUFFER_H_

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

// Giving kudos to http://ogldev.atspace.co.uk/www/tutorial35/tutorial35.html
// Unfortunately I did most this in WebGL
class GBuffer
{
public:

    enum GBUFFER_TEXTURE_TYPE {
        GBUFFER_TEXTURE_TYPE_POSITION,
        GBUFFER_TEXTURE_TYPE_DIFFUSE,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_TEXTURE_TYPE_TEXCOORD,
        GBUFFER_NUM_TEXTURES
    };

	GBuffer();

	static bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    static void BindForWriting();

    static void BindForReading();

    static void DefaultBuffer();
    
    static void SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType);
private:
	// For now I am using static variables as an experiment... sorry this is bad code.
	static GLuint m_fbo;
    static GLuint m_textures[GBUFFER_NUM_TEXTURES];
    static GLuint m_depthTexture;
};
#endif