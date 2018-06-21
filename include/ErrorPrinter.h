#ifndef ERRORPRINTER_H
#define ERRORPRINTER_H

namespace Socket {

int socketError();
int bindError();
int listenError();
int connectError();
int selectError();
int acceptError();
int readError();
int sendError();
int closeError();
int printErrno();

};

#endif // ERRORPRINTER_H
