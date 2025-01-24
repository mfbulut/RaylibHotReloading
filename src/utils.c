#include <windows.h>
#include <stdio.h>

typedef void (*GameUpdateFn)(void*);
GameUpdateFn gameUpdate = 0;
HMODULE gameLib = 0;

static char memory[4096] = {0};
char errorString[4096] = {0};
FILETIME lastWriteTime = {0, 0};

const char* FILE_PATH = "../src/game.c";
const char* BUILD_COMMAND = "cd .. && ninja game.dll";

int HasFileChanged(const char* filename) {
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (GetFileAttributesExA(filename, GetFileExInfoStandard, &fileInfo)) {
        FILETIME currentWriteTime = fileInfo.ftLastWriteTime;
        if (CompareFileTime(&currentWriteTime, &lastWriteTime) != 0) {
            lastWriteTime = currentWriteTime;
            return 1;
        }
    }
    return 0;
}

int ExecuteCommandAndCapture(const char* command, char* outputBuffer, size_t bufferSize) {
    FILE* pipe = _popen(command, "r");
    if (!pipe) {
        snprintf(outputBuffer, bufferSize, "Error opening pipe: %s\n", strerror(errno));
        return -1;
    }

    size_t totalRead = 0;
    while (fgets(outputBuffer + totalRead, bufferSize - totalRead, pipe)) {
        totalRead = strlen(outputBuffer);
        if (totalRead >= bufferSize - 1) break;
    }

    int returnCode = _pclose(pipe);
    return returnCode;
}

int UpdateGame(char** errorMessage) {
    if (HasFileChanged(FILE_PATH)) {
        if (gameLib) {
            FreeLibrary(gameLib);
            gameLib = 0;
            gameUpdate = 0;
        }

        int result = ExecuteCommandAndCapture(BUILD_COMMAND, errorString, sizeof(errorString));
        if (result != 0) {
            *errorMessage = errorString;
            return 1;
        }

        HMODULE newGameLib = LoadLibraryA("game.dll");
        if (!newGameLib) {
            strcpy(errorString, "Failed to load game.dll!\n");
            *errorMessage = errorString;
            return 1;
        }

        gameUpdate = (GameUpdateFn)GetProcAddress(newGameLib, "game_update");

        if (!gameUpdate) {
            strcpy(errorString, "Failed to get function pointers from game.dll!\n");
            *errorMessage = errorString;
            FreeLibrary(newGameLib);
            return 1;
        }

        gameLib = newGameLib;
    }

    if(gameUpdate) {
        gameUpdate(memory);
        return 0;
    }

    return 1;
}
