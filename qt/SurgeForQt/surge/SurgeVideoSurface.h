#ifndef SURGEVIDEOSURFACE_H
#define SURGEVIDEOSURFACE_H

#include <QAbstractVideoSurface>
#include <QObject>

class SurgeVideoSurface : public QAbstractVideoSurface {
    Q_OBJECT

public:
    SurgeVideoSurface(QAbstractVideoSurface *videoSurface);

public:
    bool present(const QVideoFrame &frame);
    bool start(const QVideoSurfaceFormat &format);
    void stop();
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const;
    bool isFormatSupported(const QVideoSurfaceFormat &format) const;
    QVideoSurfaceFormat nearestFormat(const QVideoSurfaceFormat &format) const;

private slots:
    void Destroyed();

signals:
    void FrameSize(int width, int height);
    void FrameBitrate(int bits);

private:
    QAbstractVideoSurface *videoSurface;
    bool firstFrame;
    bool surfaceIsActive;
};

#endif // SURGEVIDEOSURFACE_H
