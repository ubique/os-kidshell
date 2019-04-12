#include <errno.h>  // errno
#include <stdint.h> // size_t
#include <stdio.h>  // fprintf, stderr, snprintf
#include <stdlib.h> // getenv, malloc, free
#include <string.h> // strlen, strncmp, strchr, memcpy

extern char **environ;

size_t strlcpy(char *restrict dst, char const *restrict src, size_t size) {
    size_t src_len = strlen(src);
    if (size != 0) {
        while (*src != '\0' && size > 1) {
            *dst++ = *src++;
            --size;
        }
        *dst = '\0';
    }
    return src_len;
}

int setenv(char const *restrict name, char const *restrict value, int overwrite) {
    fprintf(stderr, "beware, for it is non-library setenv\n");

    static char **env = NULL;

    if (name == NULL || name[0] == '\0' || strchr(name, '=') != NULL) {
        errno = EINVAL;
        return -1;
    }

    if (getenv(name) != NULL && !overwrite) {
        return 0;
    }

    if (unsetenv(name) == -1) {
        return -1;
    }

    size_t combined_size = strlen(name) + strlen(value) + 2;
    char *combined       = malloc(combined_size);
    if (combined == NULL) {
        errno = ENOMEM;
        return -1;
    }
    snprintf(combined, combined_size, "%s=%s", name, value);

    size_t env_len = 0;
    for (char **ep = environ; *ep != NULL; ++ep) {
        ++env_len;
    }
    char **new_environ = malloc((env_len + 2) * sizeof(char *));
    if (new_environ == NULL) {
        errno = ENOMEM;
        return -1;
    }
    memcpy(new_environ, environ, env_len * sizeof(char *));
    new_environ[env_len]     = combined;
    new_environ[env_len + 1] = NULL;
    free(env);
    environ = env = new_environ;
    return 0;
}

int unsetenv(char const *name) {
    fprintf(stderr, "beware, for it is non-library unsetenv\n");

    if (name == NULL || name[0] == '\0' || strchr(name, '=') != NULL) {
        errno = EINVAL;
        return -1;
    }

    size_t name_len = strlen(name);
    for (char **ep = environ; *ep != NULL; ++ep) {
        if (strncmp(name, *ep, name_len) == 0 && (*ep)[name_len] == '=') { // found key, now delete
            for (char **dp = ep; *dp != NULL; ++dp) {
                *dp = *(dp + 1);
            }
            --ep;
        }
    }
    return 0;
}
