#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include <QIODevice>

class ByteStream : public QIODevice {
public:
    ByteStream();

    ~ByteStream();

    void close() override;

    qint64 bytesAvailable() const override;

    bool isSequential () const override {
        return true;
    }

    qint64 size() const override {
        return bytesAvailable();
    }

    qint64 readData(char *data, qint64 maxSize) override;

    qint64 writeData(const char *data, qint64 maxSize) override;

    bool atEnd() const override {
        return false;
    }

    qint64 pos() const override {
        return 0;
    }

    virtual bool open(QIODevice::OpenMode mode) override;


private:
    QByteArray *byteArray;
    bool emittedReadyRead = false;
};

#endif // BYTESTREAM_H
