#include "YUV420PRenderer.h"
#include <android/log.h>
#include "log.h"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "YUV420P_Player"
#endif

YUV420PRenderer::YUV420PRenderer() {
    iDrawWidth = 0;
    iDrawHeight = 0;
    iFrameWidth = 0;
    iFrameHeight = 0;
    y_pixels = NULL;
    u_pixels = NULL;
    v_pixels = NULL;
    setup();
}

YUV420PRenderer::~YUV420PRenderer() {
    iDrawWidth = 0;
    iDrawHeight = 0;

    y_pixels = NULL;
    u_pixels = NULL;
    v_pixels = NULL;
}

bool YUV420PRenderer::setup() {
    LOGI("setup");
    setupYUVTexture();
    loadShader(); //load shader
    glUseProgram(gProgram);

    GLuint textureUniformY = glGetUniformLocation(gProgram, "SamplerY");
    GLuint textureUniformU = glGetUniformLocation(gProgram, "SamplerU");
    GLuint textureUniformV = glGetUniformLocation(gProgram, "SamplerV");
    glUniform1i(textureUniformY, 0);
    glUniform1i(textureUniformU, 1);
    glUniform1i(textureUniformV, 2);

    return true;
}

void YUV420PRenderer::setFrameSize(int frameWidth, int frameHeight) {
    iFrameWidth = frameWidth;
    iFrameHeight = frameHeight;
}


void YUV420PRenderer::clearFrame() {
    /* static float grey;
     grey += 0.01f;
     if (grey > 1.0f) {
         grey = 0.0f;
     }*/
    //glClearColor(grey, grey, grey, 1.0f);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


GLuint YUV420PRenderer::compileShader(const char *pSource, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char *buf = (char *) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
            }
        }
    }
    return shader;
}

void YUV420PRenderer::renderFrame(uint8_t **pixels) {
    clearFrame();

    // frameData = pixels;
    /* y_pixels = pixels;
     u_pixels = pixels + iFrameWidth * iFrameHeight;
     v_pixels = pixels + iFrameWidth * iFrameHeight * 5 / 4;*/
    y_pixels = pixels[0];
    u_pixels = pixels[1];
    v_pixels = pixels[2];
    // LOGI("renderFrame:--%d-%d", iFrameWidth, iFrameHeight);

    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXY]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, iFrameWidth, iFrameHeight, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, y_pixels);

    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXU]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, iFrameWidth / 2,
                 iFrameHeight / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, u_pixels);

    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXV]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, iFrameWidth / 2,
                 iFrameHeight / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, v_pixels);

    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, coordVertices);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void YUV420PRenderer::renderFrame() {
    clearFrame();

    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXY]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, iFrameWidth, iFrameHeight, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, y_pixels);

    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXU]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, iFrameWidth / 2,
                 iFrameHeight / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, u_pixels);

    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXV]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, iFrameWidth / 2,
                 iFrameHeight / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, v_pixels);

    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
    glEnableVertexAttribArray(ATTRIB_VERTEX);

    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, coordVertices);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void YUV420PRenderer::setupYUVTexture() {
    if (_textureYUV[TEXY]) {
        glDeleteTextures(3, _textureYUV);
    }

    glGenTextures(3, _textureYUV);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXY]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXU]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXV]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void YUV420PRenderer::loadShader() {
    GLuint vertexShader = compileShader(gVertexShader, GL_VERTEX_SHADER);
    if (!vertexShader) {
        return;
    }
    GLuint fragmentShader = compileShader(gFragmentShader, GL_FRAGMENT_SHADER);
    if (!fragmentShader) {
        return;
    }

    /**2**/
    gProgram = glCreateProgram();
    glAttachShader(gProgram, vertexShader);
    glAttachShader(gProgram, fragmentShader);

    //绑定属性
    glBindAttribLocation(gProgram, ATTRIB_VERTEX, "position");
    glBindAttribLocation(gProgram, ATTRIB_TEXTURE, "TexCoordIn");

    glLinkProgram(gProgram);

    /**3**/
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(gProgram, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLint bufLength = 0;
        glGetProgramiv(gProgram, GL_INFO_LOG_LENGTH, &bufLength);
        if (bufLength) {
            char *buf = (char *) malloc(bufLength);
            if (buf) {
                glGetProgramInfoLog(gProgram, bufLength, NULL, buf);
                //LOG_E(LOG_TAG,"Could not link program:\n%s\n", buf);
                free(buf);
            }
        }
        glDeleteProgram(gProgram);
        gProgram = 0;
    }

    if (vertexShader)
        glDeleteShader(vertexShader);
    if (fragmentShader)
        glDeleteShader(fragmentShader);
}

void YUV420PRenderer::resetWindow(int winWidth, int winHeight) {
    if (iDrawWidth == winWidth && iDrawHeight == winHeight) {
        return;
    }
    iDrawWidth = winWidth;
    iDrawHeight = winHeight;

    glViewport(0, 0, winWidth, winHeight);

}
