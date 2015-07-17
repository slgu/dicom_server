#ifndef BRAIN_STROKE_SERVER_H
#define BRAIN_STROKE_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <deque>
/*
 * onconnect
 * 传输para和pid (json) 服务端开启线程 cal, return code {"msg":"start_cal", "ret_code":"0"}
 * 随时stop 服务端stop线程计算 清理内存, return code {"msg":"stop success", "ret_code":"0"}
 * 计算完毕 return code {"msg":"cal success", "ret_code":"0"}
 */
class brain_stroke_server : public QObject
{
    Q_OBJECT
public:
    explicit brain_stroke_server(QObject *parent = 0);
    ~brain_stroke_server();
public slots:
    void accept_new_connection();
    void deal_next_request();
private:
    QTcpServer server;
    QMutex deque_mutex;
    QMutex calculate_flag_mutex;
    std::deque < QTcpSocket *> request_list;
    bool calculate_flag;
};

#endif // BRAIN_STROKE_SERVER_H
