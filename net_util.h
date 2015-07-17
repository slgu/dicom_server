#ifndef NET_UTIL_H
#define NET_UTIL_H
#include "QsLog.h"
#include "config.h"
#include "httprequesthandler.h"
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
namespace netutil {
    inline void write(HttpResponse & response, int code, QString msg){
        QLOG_ERROR() << msg;
        response.setStatus(code,msg.toUtf8());
        response.write(msg.toUtf8());
    }
    inline QString make_ret_msg(bool flg, QString msg){
        QJsonObject json;
        if(flg){
            json.insert("status","success");
        }
        else{
            json.insert("status","fail");
        }
        json.insert("content", msg);
        QJsonDocument document;
        document.setObject(json);
        return QString(document.toJson(QJsonDocument::Compact));
    }
}
#endif // NET_UTIL_H
