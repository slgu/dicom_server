#include "tcp_deal.h"
#include <QDataStream>
#include "QsLog.h"
#include <QString>
#include <string>
#include <QJsonParseError>
#include <QJsonObject>
#include "contable.h"
#include "calculate_thread.h"
#include "config.h"
#include <uuid/uuid.h>
tcp_deal::tcp_deal(QObject *parent) :
    QThread(parent){
    //init empty thread
    cal_thread = NULL;
}

void tcp_deal::run(){
    QLOG_DEBUG() << "run" << QThread::currentThread();
    deal_input();
    //init signal slot
    connect(socket, SIGNAL(readyRead()), this, SLOT(deal_input()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
    exec();
    puts("end tcp deal run");
}

//deal calculate para
bool tcp_deal::deal_cal_input(std::string input, calculate_thread * & ret_cal_thread){
    /*
     * input format: json
     * {"pwi_para":"", "dwi_para":"", "pid":""}
     *
     */
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(input.c_str(), &json_error);
    std::map <QString, QString> map_para;
    if(json_error.error == QJsonParseError::NoError){
        if(parse_doucment.isObject()){
            QJsonObject obj = parse_doucment.object();
            for(auto itr = obj.begin(); itr != obj.end(); ++itr){
                map_para[itr.key()] = itr.value().toString();
            }
        }
    }

    QLOG_DEBUG() << "map" << map_para["pwi_para"] << " " << map_para["dwi_para"] << " " <<  map_para["pid"];
    PWIConstTable * pwi_para = new PWIConstTable();
    DWIConstTable * dwi_para = new DWIConstTable();
    pwi_para -> load(map_para["pwi_para"].toStdString());
    dwi_para -> load(map_para["dwi_para"].toStdString());
    calculate_thread * cal_thread = new calculate_thread(this);
    QString data_dir = Config::single() ->mp["pdata_dir"] + "/" + map_para["pid"];
    pid = map_para["pid"];
    version_guid = commutil::generate_uuid();
    QLOG_DEBUG() << "debug guid" << version_guid;
    if(!cal_thread -> set(dwi_para, pwi_para, data_dir, version_guid))
        return false;
    //manage thread
    connect(cal_thread, SIGNAL(finished()), this, SLOT(send_back_msg()));
    connect(cal_thread, SIGNAL(finished()), cal_thread, SLOT(deleteLater()));
    cal_thread -> start();
    //get return value
    ret_cal_thread = cal_thread;
    return true;
}

//deal stop para
void tcp_deal::deal_stop_input(){
    puts("begin stop thread");
    socket->close();
    //等待子进程结束
    //no terminate it is so dangerous just BLOCK
    cal_thread -> wait();
    puts("end exit");
    puts("end stop thread");
    this -> exit();
}

void tcp_deal::deal_input(){
    QLOG_DEBUG() << "deal input" << QThread::currentThread();
    QLOG_DEBUG() << "current obj" << this;
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_3);
    int l = socket->bytesAvailable();
    if(l == 0){
        puts("no data");
        return;
    }
    else{
        printf("%d\n", l);
    }
    QLOG_DEBUG() << "input charater number" << l;
    if(socket -> bytesAvailable() > MAX_LEN){
        QLOG_ERROR() << "transfer data num too large";
        //close socket
        socket -> close();
        //exit event
        this -> quit();
        return;
    }
    char * tmp_str = new char[l + 1];
    in.readRawData(tmp_str, l);
    std::string input(tmp_str, l);
    QLOG_DEBUG() << "deal input string" << QString(input.c_str());
    delete [] tmp_str;
    //stop request
    if(input == "stop"){
        if(cal_thread != NULL){
            deal_stop_input();
            return;
        }
        else{
            socket->write("error stop");
        }
    }
    else{
        if(!deal_cal_input(input,cal_thread))
            socket->write("error calculate");
        else{
            QJsonObject obj;
            obj.insert("status","start");
            QJsonDocument document;
            document.setObject(obj);
            //write data
            socket->write(document.toJson(QJsonDocument::Compact));
        }
    }
}

void tcp_deal::disconnect(){

}

void tcp_deal::send_back_msg(){
    QJsonObject obj;
    obj.insert("status","available");
    obj.insert("pid",pid);
    obj.insert("version",version_guid);
    puts("send_back");
    QJsonDocument document;
    document.setObject(obj);
    //write data
    if(!socket->isValid()){
        puts("fuck socket aaaaa");
        return;
    }
    socket->write(document.toJson(QJsonDocument::Compact));
    socket->flush();
    this->exit();
}
