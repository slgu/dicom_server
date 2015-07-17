#ifndef DELETE_VERSION_CONTROLLER_H
#define DELETE_VERSION_CONTROLLER_H

#include <QObject>
#include "httprequesthandler.h"

class delete_version_controller : public HttpRequestHandler{
    Q_OBJECT
public:
    explicit delete_version_controller(QObject *parent = 0);
    void service(HttpRequest &request, HttpResponse &response);
signals:

public slots:

};

#endif // DELETE_VERSION_CONTROLLER_H
