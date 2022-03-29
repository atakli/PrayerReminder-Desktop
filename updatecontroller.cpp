#include "updatecontroller.h"

UpdateController::UpdateController()
{

}

//bool urlExists (QString url_string) {
//    QUrl url(url_string);
//    QTcpSocket socket;
//    socket.connectToHost(url.host(), 80);
//    if (socket.waitForConnected()) {
//        socket.write("HEAD " + url.path().toUtf8() + " HTTP/1.1\r\n"
//                     "Host: " + url.host().toUtf8() + "\r\n\r\n");
//        if (socket.waitForReadyRead()) {
//            QByteArray bytes = socket.readAll();
//            if (bytes.contains("200 OK")) {
//                return true;
//            }
//        }
//    }
//    return false;
//}
