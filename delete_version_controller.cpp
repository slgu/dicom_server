#include "delete_version_controller.h"
#include "common_util.h"
#include "net_util.h"
#include "file_control.h"
#include <iostream>
delete_version_controller::delete_version_controller(QObject *parent) :
    HttpRequestHandler(parent){

}

void delete_version_controller::service(HttpRequest &request, HttpResponse &response){
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
    //delete
    bool flg = file_control::single()-> delete_dir(Config::single() -> mp["pdata_dir"] + "/" + pid + "/result/" + version_name);
    response.write(netutil::make_ret_msg(flg,"").toUtf8());
}
