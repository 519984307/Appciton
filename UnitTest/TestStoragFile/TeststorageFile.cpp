#include <QCoreApplication>
#include "StorageFile.h"
#include <QIODevice>
#include <QDebug>
#include <QDateTime>


#define FILE_NAME "data/storagetest"
#define ADDITIONAL_STRING "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define STR1 "abcedfghijklmnopqrstuvwxyz0123456789"
#define STR2 "ABCEDFGHIJKLMNOPQRSTUVWXYZ0123456789"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    IStorageBackend * storage1 = new StorageFile();
    storage1->reload(FILE_NAME,QIODevice::ReadWrite);
    storage1->setReservedSize(strlen(ADDITIONAL_STRING));
    storage1->writeAdditionalData(ADDITIONAL_STRING, strlen(ADDITIONAL_STRING));
    QDateTime startTime = QDateTime::currentDateTime();
    int counter = 0;
    while (storage1->getBlockNR() < 1000000)
    {
        storage1->appendBlockData(0, STR1, strlen(STR1));
        storage1->appendBlockData(0, STR2, strlen(STR2));
        counter +=2;
    }
    delete storage1;
    if (counter > 0) {
        qDebug()<<"wirte "<<counter<<"blocks in "<<startTime.msecsTo(QDateTime::currentDateTime())<<"ms.";
    }

    char buffer[1024];
    int readlen;
    IStorageBackend * storage2 = new StorageFile(FILE_NAME, QIODevice::ReadOnly);
    if(storage2->getReservedSize() > 0)
    {
        storage2->readAdditionalData(buffer, storage2->getReservedSize());
        buffer[storage2->getReservedSize()] = '\0';
        qDebug()<<"additional data: "<<buffer;
    }

    int blocknum =  storage2->getBlockNR();
    qDebug()<<"block number:"<<blocknum;
    startTime = QDateTime::currentDateTime();
    for(int i = 0; i< blocknum; i++)
    {
        readlen = storage2->readBlockData(i, buffer, 1024);
        buffer[readlen] = '\0';
    }
    qDebug()<<"Read "<< blocknum<<"blocks in"<<startTime.msecsTo(QDateTime::currentDateTime())<<"ms.";

    delete storage2;

    return a.exec();
}
