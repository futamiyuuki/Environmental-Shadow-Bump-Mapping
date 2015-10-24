#include <assert.h>
#include "backend.h"
#include "glut_backend.h"
#include "glfw_backend.h"


static OGLDEV_BACKEND_TYPE sBackendType = OGLDEV_BACKEND_TYPE_GLUT;

void OgldevBackendInit(OGLDEV_BACKEND_TYPE BackendType, int argc, char** argv, bool WithDepth, bool WithStencil)
{
    sBackendType = BackendType;
    
    switch (BackendType) {
        case OGLDEV_BACKEND_TYPE_GLUT:
            GLUTBackendInit(argc, argv, WithDepth, WithStencil);
            break;
        case OGLDEV_BACKEND_TYPE_GLFW:
            GLFWBackendInit(argc, argv, WithDepth, WithStencil);
            break;
        default:
            assert(0);
    }
}


void OgldevBackendTerminate()
{
    switch (sBackendType) {
        case OGLDEV_BACKEND_TYPE_GLUT:
            // nothing to do here...
            break;
        case OGLDEV_BACKEND_TYPE_GLFW:
            GLFWBackendTerminate();
            break;
        default:
            assert(0);
    }
}


bool OgldevBackendCreateWindow(uint Width, uint Height, bool isFullScreen, const char* pTitle)
{
    switch (sBackendType) {
        case OGLDEV_BACKEND_TYPE_GLUT:
            return GLUTBackendCreateWindow(Width, Height, isFullScreen, pTitle);
        case OGLDEV_BACKEND_TYPE_GLFW:
            return GLFWBackendCreateWindow(Width, Height, isFullScreen, pTitle);
        default:
            assert(0);
    }

	return false;
}


void OgldevBackendRun(ICallbacks* pCallbacks)
{
    switch (sBackendType) {
        case OGLDEV_BACKEND_TYPE_GLUT:
            GLUTBackendRun(pCallbacks);
            break;
        case OGLDEV_BACKEND_TYPE_GLFW:
            GLFWBackendRun(pCallbacks);
            break;
        default:
            assert(0);
    }
}


void OgldevBackendSwapBuffers()
{
    switch (sBackendType) {
        case OGLDEV_BACKEND_TYPE_GLUT:
            GLUTBackendSwapBuffers();
            break;
        case OGLDEV_BACKEND_TYPE_GLFW:
            GLFWBackendSwapBuffers();
            break;
        default:
            assert(0);
    }    
}


void OgldevBackendLeaveMainLoop()
{
    switch (sBackendType) {
        case OGLDEV_BACKEND_TYPE_GLUT:
            GLUTBackendLeaveMainLoop();
            break;
        case OGLDEV_BACKEND_TYPE_GLFW:
            GLFWBackendLeaveMainLoop();
            break;
        default:
            assert(0);
    }
}


