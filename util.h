#ifndef UTIL_H
#define UTIL_H

#include <QString>

struct Paths
{
    inline static const QString evkatOfflinePath = "/namazVakitFiles/evkatOffline.json";
//    inline static const QString applicationDirPath = QApplication::applicationDirPath();
    inline static QString applicationDirPath;
    inline static const QString evkatOnlinePath = "/namazVakitFiles/evkatOnline.json";
};

#endif // UTIL_H
