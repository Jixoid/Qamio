#include <fcntl.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drm_mode.h>

int main() {
  int drm_fd = open("/dev/dri/card0", O_RDWR | __O_CLOEXEC);
  if (drm_fd < 0) {
    perror("Failed to open DRM device");
    return 1;
  }

  drmModeRes *resources = drmModeGetResources(drm_fd);
  if (!resources) {
    perror("Failed to get DRM resources");
    close(drm_fd);
    return 1;
  }

  printf("Number of connectors: %d\n", resources->count_connectors);

  for (int i = 0; i < resources->count_connectors; i++) {
    drmModeConnector *connector = drmModeGetConnector(drm_fd, resources->connectors[i]);
    
    printf("Connector %d: ", connector->connector_id);
    
    printf("Connector_Type: %d\n", connector->connector_type);

    if (connector->connection == DRM_MODE_CONNECTED) {
      printf("Connected (Resolution: %dx%d)\n", 
        connector->modes[0].hdisplay, connector->modes[0].vdisplay);
    } else {
      printf("Not connected\n");
    }
    
    drmModeFreeConnector(connector);
  }

  drmModeFreeResources(resources);
  close(drm_fd);
  return 0;
}
