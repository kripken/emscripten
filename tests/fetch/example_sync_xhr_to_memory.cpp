#include <string.h>
#include <stdio.h>
#include <math.h>
#include <emscripten/fetch.h>

int main()
{
  emscripten_fetch_attr_t attr;
  emscripten_fetch_attr_init(&attr);
  strcpy(attr.requestMethod, "GET");
  attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
  emscripten_fetch_t *fetch = emscripten_fetch(&attr, "file.dat"); // Blocks here until the operation is complete.
  if (fetch->status == 200) {
    printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
  } else {
    printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);    
  }
  emscripten_fetch_close(fetch);
}
