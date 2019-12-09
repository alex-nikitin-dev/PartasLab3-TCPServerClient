#pragma once
bool ClientInitialize(int &socketFD, char *ip, int port);
bool SendMenuFlag(int socketFD, ServerFunc menuFunc);
bool ReceiveFileFromServer(int socketFD, char *pathToReceive, char *pathToSave, int pathSize, int bufferSizeToReceiving);
bool ClientInitialize(int &socketFD, char *argv[], ServerFunc func);
bool GetFolderContentFromServer(int socketFD, char *path, int pathSize, int bufferSizeToReceiving);
bool SendString(int socketFD, char *str);