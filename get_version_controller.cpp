#include "get_version_controller.h"
#include "QsLog.h"
#include "config.h"
#include <QDir>
#include "common_util.h"
#include "net_util.h"
#include <iostream>
#include <QJsonArray>
get_version_controller::get_version_controller(QObject *parent) :
    HttpRequestHandler(parent){

}
void get_version_controller::service(HttpRequest& request, HttpResponse& response){
    QString pid = request.getParameter("pid");
    if(pid == "" || !commutil::check_digit(pid.toStdString().c_str())){
        netutil::write(response, 401, "input para with pid error");
        return;
    }
    QString result_dir = Config::single()->mp["pdata_dir"] + "/" + pid + "/result";
    QLOG_DEBUG() << "debug result dir" << result_dir;
    QDir dir(result_dir);
    if(!dir.exists()){
        QString res = netutil::make_ret_msg(false,"dir not exixts");
        netutil::write(response,401, res);
        return;
    }
    QJsonArray json_arr;
    foreach(QFileInfo file, dir.entryInfoList()){
        if(file.isDir()){
            if(!file.fileName().startsWith(".")){
                //special server tmp data suffix
                if(!file.fileName().startsWith("s_")){
                    json_arr.append(file.fileName());
                }
            }
        }
    }
    QJsonDocument document;
    document.setArray(json_arr);
    response.write(netutil::make_ret_msg(true,document.toJson(QJsonDocument::Compact)).toUtf8());
}
