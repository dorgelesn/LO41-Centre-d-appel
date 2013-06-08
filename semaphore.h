/* semaphore.h */
extern void erreurFin(const char* msg);
extern void initSem(int nbSem,char* nomFich,int* pvinit);
extern void libereSem();
extern void P(int numSem);
extern void V(int numSem);
