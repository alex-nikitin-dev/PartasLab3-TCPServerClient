#pragma once
bool ClientInitialize(int &socketFD, char *ip, int port);
bool SendMenuFlag(int socketFD, ServerFunc menuFunc);
bool ReceiveFileFromServer(int socketFD, char *pathToReceive, char *pathToSave, int pathSize, int bufferSizeToReceiving);
