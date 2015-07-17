#include "download_version_controller.h"
#include "config.h"
#include "QsLog.h"
#include "staticfilecontroller.h"
#include <QDir>
#include <QProcess>
#include <QStringList>
#include "common_util.h"
#include "net_util.h"
//getfile?version_name=&pid= GET 得到一个pid的版本内容
extern char * inner_data_trans_key;
download_version_controller::download_version_controller(QObject *parent):
    HttpRequestHandler(parent){
}
void download_version_controller::service(HttpRequest &request, HttpResponse &response){
    QString version_name = request.getParameter("version_name");
    QString pid = request.getParameter("pid");

    //check input data
    if(pid == "" || version_name == ""){
        netutil::write(response, 401, "input para error pid,version_name when download");
        return;
    }
    if(!commutil::check_digit(pid.toStdString().c_str()) && !commutil::check_str(version_name.toStdString().c_str())){
        netutil::write(response, 401, "input para error pid,version_name when download");
    }
    QDir result_dir = Config::single() -> mp["pdata_dir"] + "/" + pid + "/result/" + version_name;
    if(!result_dir.exists()){
        netutil::write(response, 401, "data not exists");
        return;
    }
    QProcess process;
    process.setWorkingDirectory(Config::single() -> mp["pdata_dir"] + "/" + pid + "/result");
    process.start("zip -r " + version_name + ".zip " + version_name);
    bool ret_code = process.waitForFinished();
    if(!ret_code){
        netutil::write(response, 401, "zip error");
        return;
    }
    response.redirect(("/data/"+ pid + "/result/" + version_name + ".zip?key=" + QString(inner_data_trans_key)).toUtf8()) ;
}

