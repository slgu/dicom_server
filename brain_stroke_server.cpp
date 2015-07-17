#include "brain_stroke_server.h"
#include <string>
#include <iostream>
#include <QtCore>
#include "QsLog.h"
#include "QsLogDest.h"
#include "calculate_method.h"
#include "contable.h"
#include "config.h"
#include "tcp_deal.h"
#include <QTcpServer>
#include <QHostAddress>
#include <QMutexLocker>

brain_stroke_server::brain_stroke_server(QObject *parent) :
    QObject(parent){
    connect(&server, SIGNAL(newConnection()), this,SLOT(accept_new_connection()));
    server.listen(QHostAddress::Any, 8888);
    calculate_flag = false;
    puts("listen");
}

void brain_stroke_server::accept_new_connection(){
    QTcpSocket * client = server.nextPendingConnection();
    std::cout << "accept new" << client->peerAddress().toString().toStdString() << std::endl;
    QLOG_DEBUG() << "accept" << client << " " << QThread::currentThread();
    //use a deque to block request: one by one
    calculate_flag_mutex.lock();
    if(!calculate_flag){
        calculate_flag = true;
        calculate_flag_mutex.unlock();
        tcp_deal * tcp_deal_thread = new tcp_deal();
        tcp_deal_thread -> set_socket(client);
        connect(tcp_deal_thread, SIGNAL(finished()), this, SLOT(deal_next_request()));
        connect(tcp_deal_thread, SIGNAL(finished()), tcp_deal_thread, SLOT(deleteLater()));
        tcp_deal_thread -> start();
        return;
    }
    else{
        //push to request_list
        puts("push to request_list");
        QMutexLocker deque_mutex_locker(&deque_mutex);
        request_list.push_back(client);
    }
    calculate_flag_mutex.unlock();
}

brain_stroke_server::~brain_stroke_server(){
    server.close();
}

void brain_stroke_server::deal_next_request(){
    puts("deal next");
    //get next request
    deque_mutex.lock();
    if(request_list.size() != 0){
        puts("has next");
        QTcpSocket * client = request_list.front();
        request_list.pop_front();
        deque_mutex.unlock();
        //ignore closing socket
        if(client->state() == QTcpSocket::UnconnectedState || client->state() == QTcpSocket::ClosingState){
            client->close();
            QLOG_DEBUG() << "close client" << client;
            std::cout << "close client:" << client->peerAddress().toString().toStdString() << std::endl;
            //search next from request_list
            deal_next_request();
            return;
        }
        tcp_deal * tcp_deal_thread = new tcp_deal();
        tcp_deal_thread -> set_socket(client);
        connect(tcp_deal_thread, SIGNAL(finished()), this, SLOT(deal_next_request()));
        connect(tcp_deal_thread, SIGNAL(finished()), tcp_deal_thread, SLOT(deleteLater()));
        tcp_deal_thread -> start();
        return;
    }
    deque_mutex.unlock();
    QMutexLocker calculate_flag_mutex_locker(&calculate_flag_mutex);
    //no calculating now
    calculate_flag = false;
    return;
}
