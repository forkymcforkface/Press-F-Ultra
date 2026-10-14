#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "bios.h"

static char *pfu_bios_trim(char *text)
{
    char *end;
    while (isspace((unsigned char)*text))
        text++;
    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1]))
        *--end = '\0';
    return text;
}

bool pfu_bios_path(char *path, size_t size)
{
    FILE *file;
    char line[512];
    bool section = false;
    bool ok;

    if (!size)
        return false;
    path[0] = '\0';
    file = fopen("sd:/phosphor/config.ini", "r");
    if (!file)
        return false;
    while (fgets(line, sizeof(line), file))
    {
        char *text;
        char *end;
        char *key;
        if (!strchr(line, '\n') && !feof(file))
        {
            int ch;
            do
                ch = fgetc(file);
            while (ch != '\n' && ch != EOF);
            path[0] = '\0';
            fclose(file);
            return false;
        }
        text = pfu_bios_trim(line);
        if (!*text || *text == ';' || *text == '#')
            continue;
        if (*text == '[')
        {
            end = strchr(text, ']');
            section = false;
            if (end)
            {
                *end = '\0';
                section = !strcmp(text + 1, "path");
            }
            continue;
        }
        if (!section || !(end = strchr(text, '=')))
            continue;
        *end++ = '\0';
        key = pfu_bios_trim(text);
        for (text = key; *text; text++)
            *text = (char)tolower((unsigned char)*text);
        if (strcmp(key, "bios"))
            continue;
        text = pfu_bios_trim(end);
        path[0] = '\0';
        if (!strncmp(text, "sd:/", 4) && strlen(text) < size)
            strcpy(path, text);
    }
    ok = !ferror(file);
    if (fclose(file))
        ok = false;
    if (!ok)
        path[0] = '\0';
    return path[0] != '\0';
}

bool pfu_bios_load(void *data, const char *root, const char *name)
{
    unsigned char buffer[1024];
    char path[256];
    FILE *file;
    bool ok;
    int length = snprintf(path, sizeof(path), "%s%s%s", root,
        root[0] && root[strlen(root) - 1] == '/' ? "" : "/", name);
    if (length < 0 || (size_t)length >= sizeof(path))
        return false;
    file = fopen(path, "rb");
    if (!file)
        return false;
    ok = fread(buffer, 1, sizeof(buffer), file) == sizeof(buffer);
    if (fgetc(file) != EOF || ferror(file))
        ok = false;
    if (fclose(file))
        ok = false;
    if (ok)
        memcpy(data, buffer, sizeof(buffer));
    return ok;
}
