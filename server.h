#pragma once
#include <stdio.h>

bool SendFile(FILE *fileFD, int fileBufSize, int connectFD);
bool ReceivePath(int connectFD, char *path, int length);
FILE *OpenFile(const char *path, const char *mod);
bool ServerInitializing(int &socketFD, int port);
bool WaitForNewConnection(int socketFD, int &connectFD);
bool SendContentOfPath(char *path, int fileBufSize, int connectFD);