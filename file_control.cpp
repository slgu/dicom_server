#include "file_control.h"
#include "QsLog.h"
#include <QDir>
#include <QMutexLocker>
#include <QMutex>
file_control::file_control()
{
}


bool file_control::rename(QString base_name, QString old_name, QString new_name){
    //lock mutex
    QMutexLocker locker(&mutex);
    QDir base_dir(base_name);
    QDir old_dir(base_name + "/" + old_name);
    QDir new_dir(base_name + "/" + new_name);
    QLOG_DEBUG() << "old name" << base_name + "/" + old_name;
    QLOG_DEBUG() << "new name" << base_name + "/" + new_name;
    if(old_dir.exists() && (!new_dir.exists()));
    else{
        return false;
    }
    //rename
    base_dir.rename(old_name, new_name);
    return true;
}

bool file_control::delete_dir(QString basename){
    //lock mutex
    QMutexLocker locker(&mutex);
    QDir dir(basename);
    if(!dir.exists()){
        return false;
    }
    dir.removeRecursively();
    return true;
}
