#include <pbuffer.h>

bool pbuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    cout << glGenFramebuffers << endl;
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

    // Create the gbuffer textures
    glGenTextures(ARRAY_SIZE_IN_ELEMENTS(m_textures), m_textures);
    glGenTextures(1, &m_depthTexture);

    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_textures) ; i++) {
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i+4, GL_TEXTURE_2D, m_textures[i], 0);
    }

    // depth
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        //printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    // restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}

void pbuffer::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void pbuffer::BindForReading()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
}

void pbuffer::SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}

void pbuffer::DefaultBuffer()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //cout << ARRAY_SIZE_IN_ELEMENTS(m_textures) << endl;
    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
        glActiveTexture(GL_TEXTURE0 + i+4);
        glBindTexture(GL_TEXTURE_2D, m_textures[GBUFFER_TEXTURE_TYPE_PROJECTOR + i]);
    }
}

GLuint pbuffer::m_fbo;
GLuint pbuffer::m_textures[GBUFFER_NUM_TEXTURES];
GLuint pbuffer::m_depthTexture;