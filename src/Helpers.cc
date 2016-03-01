#include "Helpers.h"

#include <unistd.h>
#include <fcntl.h>

int SurgeUtil::SetNonBlocking(const int fd) {
    return fcntl(fd, F_SETFL, O_NONBLOCK);
}

