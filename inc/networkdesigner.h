#ifndef NETWORKDESIGNER_H
#define NETWORKDESIGNER_H
#include <QDebug>
#include <QObject>
#include <QString>
#include <QProcess>
#include <QFile>

class NetworkDesigner : public QObject
{
    Q_OBJECT
public:
  explicit NetworkDesigner(QObject *parent = nullptr);
  bool convertKmlToTnm(QString kmlFileName);
  bool importTno(QString tnoFileName);

signals:
  void kmlToTnmConversionDone(QString tnmOutput);
  void tnoFileOpened(QString tnoOutput);

public slots:
};

#endif // NETWORKDESIGNER_H
