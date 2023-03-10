#include "_microcompute.h"

struct mc_State {
    EGLDisplay disp;
    EGLContext ctx;
};

static struct mc_State state;

#ifdef MC_DEBUG_ENABLE
static void gl_debug_cb(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* msg,
    const void* data
) {
    char* sourceStr = (char*[]){
        "API",
        "WINDOW",
        "SHADER",
        "THIRD PARTY",
        "APPLICATION",
        "UNKNOWN",
    }[source - GL_DEBUG_SOURCE_API];

    char* typeStr = (char*[]){
        "error",
        "deprecated",
        "undefined",
        "portability",
        "performance",
        "other",
        "marker",
    }[type - GL_DEBUG_TYPE_ERROR];

    printf("GL: %s (%s): %s\n", sourceStr, typeStr, msg);
}
#endif

k_Result mc_start() {
    state.disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    K_ASSERT_ERR(state.disp != EGL_NO_DISPLAY, "failed to get egl disp");

    EGLint major, minor;
    K_ASSERT_ERR(
        eglInitialize(state.disp, &major, &minor),
        "failed to initialize egl"
    );

    K_ASSERT_ERR(
        !(major < 1 || (major == 1 && minor < 5)),
        "egl version too low (min 1.5)"
    );

    K_ASSERT_ERR(eglBindAPI(EGL_OPENGL_API), "failed to bind opengl to egl");

    EGLConfig eglCfg;

    K_ASSERT_ERR(
        eglChooseConfig(
            state.disp,
            (EGLint[]){EGL_NONE},
            &eglCfg,
            1,
            &(EGLint){0}
        ),
        "failed to choose egl config"
    );

    state.ctx = eglCreateContext(
        state.disp,
        eglCfg,
        EGL_NO_CONTEXT,
        (EGLint[]){
            EGL_CONTEXT_MAJOR_VERSION,
            4,
            EGL_CONTEXT_MINOR_VERSION,
            3,
            EGL_CONTEXT_OPENGL_DEBUG,
            EGL_TRUE,
            EGL_NONE,
        }
    );

    K_ASSERT_ERR(state.ctx != EGL_NO_CONTEXT, "failed to create egl context");

    K_ASSERT_ERR(
        eglMakeCurrent(state.disp, EGL_NO_SURFACE, EGL_NO_SURFACE, state.ctx),
        "failed to make egl context current"
    );

    K_ASSERT_ERR(gladLoadGL() != 0, "failed to load GLAD");

#ifdef MC_DEBUG_ENABLE
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_cb, NULL);
#endif

    return GL_CHECK_ERROR();
}

k_Result mc_stop() {
    if (state.ctx != 0) eglDestroyContext(state.disp, state.ctx);
    if (state.disp != 0) eglTerminate(state.disp);
    return GL_CHECK_ERROR();
}

k_Result mc_memory_barrier() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    return K_OK;
}