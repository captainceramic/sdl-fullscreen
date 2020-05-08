/*
  This is a little program to see if I can use EGL to get an OpenGLESv2 context
  on the raspberry pi. (zero or 3).

  It is based on the code from:
  https://jonmacey.blogspot.com/2012/05/getting-started-with-egl-on-raspberry.html
  https://docs.imgtec.com/OpenGLES_HelloAPI_Guide/introduction_to_opengl_es_helloapi_guide.html

*/

#include <stdio.h>
#include <assert.h>

#include <EGL/egl.h>

int main(int argc, char* argv[]) {

  /* Let's see if this works - get a display */
  EGLDisplay display;
  display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  assert(display != EGL_NO_DISPLAY);

  /* Get some EGL information */
  int major;
  int minor;
  EGLBoolean result;

  result = eglInitialize(display, &major, &minor);
  assert(result != EGL_FALSE);

  printf("EGL version is %d.%d\n", major, minor);

  /* Now look at how many 'configs' we have. */
  EGLint numConfigs;
  result = eglGetConfigs(display, NULL, 0, &numConfigs);
  assert(result != EGL_FALSE);

  printf("Number of configs found: %d\n", numConfigs);

  /* make an array of 'configs' and put them in there. */
  EGLConfig configs[numConfigs];
  result = eglGetConfigs(display, configs, numConfigs, &numConfigs);
  assert(result != EGL_FALSE);

  /* Now print out all the configs */
  EGLint value;
  for(int i=0; i<numConfigs; i++) {
    printf("Config number: %d\n", i);

    eglGetConfigAttrib(display, configs[i], EGL_BUFFER_SIZE, &value);
    printf("\tBuffer size: %d\n", value);

    eglGetConfigAttrib(display, configs[i], EGL_MAX_PBUFFER_WIDTH, &value);
    printf("\tBuffer max width: %d\n", value);

    eglGetConfigAttrib(display, configs[i], EGL_MAX_PBUFFER_HEIGHT, &value);
    printf("\tBuffer max height: %d\n", value);
  }

  

  
  return 0;
  
}
